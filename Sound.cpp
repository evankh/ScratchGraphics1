#include "Sound.h"
#include "al.h"
//#include "alut.h"	// Where do I find this? I don't fancy parsing WAV files myself...
#include "ServiceLocator.h"

#include <algorithm>

int Sound::sSampleRate = 44100;

Sound::Sound(std::string filename) :mFilename(filename) {}

Sound::~Sound() {
	delete mData;
	alDeleteBuffers(1, &mHandle);
}

void Sound::transfer() {
	if (mHandle) {
		alDeleteBuffers(1, &mHandle);
		mHandle = 0;
	}
	short* data = new short[mNumSamples];
	for (int i = 0; i < mNumSamples; i++)
		data[i] = short(INT16_MAX*mData[i]);
	alGenBuffers(1, &mHandle);
	alBufferData(mHandle, AL_FORMAT_MONO16, data, mNumSamples * sizeof(short), sSampleRate);
	delete data;
}

void FileSound::transfer() {
	if (mHandle) {
		alDeleteBuffers(1, &mHandle);
		mHandle = 0;
	}
	alGenBuffers(1, &mHandle);
	ALenum format;
	if (mNumChannels == 2)
		format = AL_FORMAT_STEREO16;
	else
		format = AL_FORMAT_MONO16;
	alBufferData(mHandle, format, mShortData, mNumSamples * sizeof(short), mSampleRate);
}

FileSound::FileSound(std::string filename) :Sound(filename) {
	FileService file(mFilename);
	if (file.good()) {
		// Do stuff
		int fileSize;
		if (file.extract("RIFF`rIWAVE", &fileSize)) {
			struct {
				int cksize;
				short wFormatTag,
					nChannels;
				int nSamplesPerSec,
					nAvgBytesPerSec;
				short nBlockAlign,
					wBitsPerSample;
					/*cbSize,
					wValidBitsPerSample;
				int dwChannelMask;
				long long SubFormat;*/	// Although the standard gives no indication of why these might not be present, my example file doesn't have them
			} formatData;
			if (file.extract("fmt `rI`rs`rs`rI`rI`rs`rs", &formatData)) {
				assert(formatData.wFormatTag == 0x0001);
				assert(formatData.wBitsPerSample == 16);	// Should be reasonably straightforward to make it work with other types of data
				mSampleRate = formatData.nSamplesPerSec;
				mNumChannels = formatData.nChannels;
				int dataSize;
				if (file.extract("data`rI", &dataSize)) {
					mNumSamples = dataSize / 2;
					mShortData = new short[mNumSamples];
					for (int i = 0; i < dataSize / 2; i++)
						file.extract("`rs", &mShortData[i]);	// Do this more efficiently with e.g. memcpy or write a bulk extraction function in FileService
					if (dataSize & 0x1)
						file.extract("`C", NULL);
					transfer();
				}
			}
		}
		else {
			ServiceLocator::getLoggingService().error("Not a valid WAV file", filename);
		}
	}
	else {
		ServiceLocator::getLoggingService().badFileError(mFilename);
	}
}

bool ProceduralSound::sHaveGeneratedNoteTable = false;
std::map<std::string, float>* ProceduralSound::sNoteTable = new std::map<std::string, float>[10];
std::vector<std::string> ProceduralSound::sNoteNames = { "A","A#","B","C","C#","D","D#","E","F","F#","G","G#" };

void ProceduralSound::generateNoteTable() {
	float baseFreq = 27.5f;
	for (int octave = 0; octave < 10; octave++) {
		float iterFreq = baseFreq;
		for (std::string note : sNoteNames) {
			sNoteTable[octave][note] = iterFreq;
			iterFreq *= 1.05946309435f;	// 12th root of 2
		}
		baseFreq *= 2.0f;
	}
	sHaveGeneratedNoteTable = true;
}

ProceduralSound::ProceduralSound(std::string filename) :Sound(filename) {
	if (!sHaveGeneratedNoteTable)
		generateNoteTable();
	FileService file(mFilename);
	if (file.good()) {
		Envelope envelope;
		while (file.good()) {
			char* err;
			Point fpoint;
			struct { float time; int note, sharp; int octave; float gain = 1.0f; } npoint;
			struct { char* interp; char* waveform; int p1, p2; } sweep;
			if (file.extract("//`S`L", NULL));
			else if (file.extract("Point time:`F freq:`F", &fpoint)) {
				if (file.extract(" gain:`F", &fpoint.gain));
				mPoints.push_back(fpoint);
				if (file.extract("`?S`L", &err)) {
					if (err) {
						ServiceLocator::getLoggingService().error("Unexpected contents in point defintion", err);
						delete err;
					}
				}
			}
			else if (file.extract("Point time:`F note:`C`C", &npoint)) {	// How do I use extract to get either one or two characters, bounded by an int?
				if (npoint.sharp != '#' && npoint.sharp != 'b') {
					file.putBack(npoint.sharp);
					npoint.sharp = '\0';
				}
				std::string note = std::string(1, npoint.note);
				if (npoint.sharp) note += std::string(1, npoint.sharp);
				file.extract("`I", &npoint.octave);
				if (file.extract(" gain:`F", &npoint.gain));
				if (sNoteTable[npoint.octave].count(note)) {
					Point p;
					p.time = npoint.time;
					p.freq = sNoteTable[npoint.octave][note];
					p.gain = npoint.gain;
					mPoints.push_back(p);
				}
				else
					ServiceLocator::getLoggingService().error("Malformed note string", note);
				if (file.extract("`?S`L", &err)) {
					if (err) {
						ServiceLocator::getLoggingService().error("Unexpected contents in point defintion", err);
						delete err;
					}
				}
			}
			else if (file.extract("Sweep `S `S `I `I`L", &sweep)) {
				Sweep s;
				s.adsr = envelope;
				s.p1 = &mPoints[sweep.p1];
				s.p2 = &mPoints[sweep.p2];
				if (strcmp(sweep.interp, "Constant") == 0) {	// Testing string equality, bleh
					s.gain = interpConstant;
					s.freq = interpConstant;
				}
				else if (strcmp(sweep.interp, "Linear") == 0) {
					s.gain = interpLinear;
					s.freq = interpLinear;
				}
				else if (strcmp(sweep.interp, "Logarithmic") == 0) {
					s.gain = interpLinear;
					s.freq = interpLogarithmic;
				}
				else
					ServiceLocator::getLoggingService().error("Unrecognized interpolation mode (defaulting to Constant)", sweep.interp);
				if (strcmp(sweep.waveform, "Sine") == 0)
					s.wave = waveSine;
				else if (strcmp(sweep.waveform, "Square") == 0)
					s.wave = waveSquare;
				else if (strcmp(sweep.waveform, "Sawtooth") == 0)
					s.wave = waveSawtooth;
				else if (strcmp(sweep.waveform, "Noise") == 0)
					s.wave = waveNoise;
				else
					ServiceLocator::getLoggingService().error("Unrecognized waveform (defaulting to Sine)", sweep.waveform);
				mSweeps.push_back(s);
			}
			else if (file.extract("Envelope `F:`F `F:`F `F:`F `F:`F`L", &envelope));	// Will continue to use the envelope until another one is specified
			else if (file.extract("`S`L", &err)) {
				ServiceLocator::getLoggingService().error("Unexpected line in sound file", err);
				delete err;
			}
		}
		build();
		transfer();
	}
	else {
		ServiceLocator::getLoggingService().badFileError(mFilename);
	}
}

float interpConstant(float v1, float v2, float fac) {
	return v1;
}

float interpLinear(float v1, float v2, float fac) {
	return (1 - fac)*v1 + fac * v2;
}

float interpLogarithmic(float v1, float v2, float fac) {
	return expf(interpLinear(logf(v1), logf(v2), fac));
}

float waveSine(float x) {
	return sinf(x);
}

float waveSquare(float x) {
	return (fmodf(x, 2 * PI) < PI) ? -1.0f : 1.0f;
}

float waveSawtooth(float x) {
	x = fmodf(x, 2 * PI);
	return x / PI - 1.0f;
}

float waveNoise(float x) {
	return (float)rand() / RAND_MAX;
}

// Non-normalized [0,duration] time coordinate, not [0,1] fac
float ADSR(Envelope env, float time, float duration) {
	if (time < env.delayTime)
		return 0.0;
	if (time < env.attackTime)
		return interpLinear(env.delayAmplitude, env.attackAmplitude, (time - env.delayTime) / (env.attackTime - env.delayTime));
	if (time < env.decayTime)
		return interpLinear(env.attackAmplitude, env.decayAmplitude, (time - env.attackTime) / (env.decayTime - env.attackTime));
	if (duration < env.decayTime)
		duration = env.decayTime;
	if (time < duration)
		return env.decayAmplitude;
	if (time < duration + env.releaseTime)
		return interpLinear(env.decayAmplitude, env.releaseAmplitude, (time - duration) / env.releaseTime);
	return 0.0;
}

void ProceduralSound::build() {
	float length = 0.0;
	for (Sweep sweep : mSweeps)
		if (sweep.p2->time + sweep.adsr.releaseTime > length) length = sweep.p2->time + sweep.adsr.releaseTime;	// Still doesn't handle if the attack-decay of the last note is shorter than the note, in which case it should extend even further... Also doesn't pad the end with silence!
	for (Point point : mPoints)
		if (point.time>length) length = point.time;
	mNumSamples = int(length * sSampleRate);
	mData = new float[mNumSamples];
	for (int i = 0; i < mNumSamples; i++)
		mData[i] = 0.0f;
	std::sort(mSweeps.begin(), mSweeps.end(), [](Sweep const& a, Sweep const& b) { return a.p1->time < b.p1->time; });
	for (Sweep sweep : mSweeps) {
		float startTime = sweep.p1->time + sweep.adsr.delayTime;
		int startIndex = int(sweep.p1->time * sSampleRate);
		float endTime = fmax(sweep.p2->time + sweep.adsr.releaseTime, sweep.p1->time + sweep.adsr.decayTime + sweep.adsr.releaseTime);
		int endIndex = int(endTime * sSampleRate);
		float duration = sweep.p2->time - sweep.p1->time;
		sweep.p2->phase = fmod((sweep.p1->freq * (sweep.p2->time - sweep.p1->time) - sweep.p2->freq) * 2 * PI + sweep.p1->phase, 2 * PI);
		for (int i = startIndex; i < endIndex; i++) {
			float time = (endTime - sweep.p1->time) * (float)(i - startIndex) / (endIndex - startIndex);
			float fac = (time - startTime) / (sweep.p2->time - startTime);
			fac = fmax(fmin(fac, 1.0f), 0.0f);	// Clamp to [0,1]
			mData[i] += ADSR(sweep.adsr, time, duration) * sweep.gain(sweep.p1->gain, sweep.p2->gain, fac) * sweep.wave(sweep.freq(sweep.p1->freq, sweep.p2->freq, fac) * (time+startTime) * 2 * PI + sweep.p1->phase);
		}
	}
	// Analyze samples to find the largest float
	for (int i = 0; i < mNumSamples; i++)
		if (abs(mData[i]) > mGain) mGain = abs(mData[i]);
	// Save it in mGain, to be returned to the Source when the sound is played
	// Use that to normalize all floats to [-1,1]	(or should I require and enforce all points at a given timecode to sum to <= 1? That gets real hard when there's unaligned points in the middle of a sweep)
	for (int i = 0; i < mNumSamples; i++)
		mData[i] /= mGain;
}