#include "Sound.h"
#include "al.h"
//#include "alut.h"	// Where do I find this? I don't fancy parsing WAV files myself...
#include "ServiceLocator.h"

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

FileSound::FileSound(std::string filename) :Sound(filename) {
	FileService& file = ServiceLocator::getFileService(mFilename.data());
	if (file.good()) {
		// Do stuff
	}
	else {
		ServiceLocator::getLoggingService().badFileError(mFilename);
	}
	file.close();
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
	FileService& file = ServiceLocator::getFileService(mFilename.data());
	if (file.good()) {
		while (file.good()) {
			char* err;
			Point fpoint;
			struct { float time; int note, sharp; int octave; float gain = 1.0f; } npoint;
			struct { char* interp; char* waveform; int p1, p2; } sweep;
			if (file.extract("//\\S\\L", NULL));
			else if (file.extract("Point time:\\F freq:\\F", &fpoint)) {
				if (file.extract(" gain:\\F", &fpoint.gain));
				mPoints.push_back(fpoint);
				if (file.extract("\\?S\\L", &err)) {
					if (err) {
						ServiceLocator::getLoggingService().error("Unexpected contents in point defintion", err);
						delete err;
					}
				}
			}
			else if (file.extract("Point time:\\F note:\\C\\C", &npoint)) {	// How do I use extract to get either one or two characters, bounded by an int?
				if (npoint.sharp != '#' && npoint.sharp != 'b') {
					file.putBack(npoint.sharp);
					npoint.sharp = '\0';
				}
				std::string note = std::string(1, npoint.note);
				if (npoint.sharp) note += std::string(1, npoint.sharp);
				file.extract("\\I", &npoint.octave);
				if (file.extract(" gain:\\F", &npoint.gain));
				if (sNoteTable[npoint.octave].count(note)) {
					Point p;
					p.time = npoint.time;
					p.freq = sNoteTable[npoint.octave][note];
					p.gain = npoint.gain;
					mPoints.push_back(p);
				}
				else
					ServiceLocator::getLoggingService().error("Malformed note string", note);
				if (file.extract("\\?S\\L", &err)) {
					if (err) {
						ServiceLocator::getLoggingService().error("Unexpected contents in point defintion", err);
						delete err;
					}
				}
			}
			else if (file.extract("Sweep \\S \\S \\I \\I\\L", &sweep)) {
				Sweep s;
				s.p1 = &mPoints[sweep.p1];
				s.p2 = &mPoints[sweep.p2];
				if (strcmp(sweep.interp, "Constant") == 0)	// Testing string equality, bleh
					s.type = INTER_CONSTANT;
				else if (strcmp(sweep.interp, "Linear") == 0)
					s.type = INTER_LINEAR;
				else if (strcmp(sweep.interp, "Logarithmic") == 0)
					s.type = INTER_LOG;
				else {
					ServiceLocator::getLoggingService().error("Unrecognized interpolation mode (defaulting to Constant)", sweep.interp);
					s.type = INTER_CONSTANT;
				}
				if (strcmp(sweep.waveform, "Sine") == 0)
					s.wave = WAVE_SINE;
				else if (strcmp(sweep.waveform, "Square") == 0)
					s.wave = WAVE_SQUARE;
				else if (strcmp(sweep.waveform, "Sawtooth") == 0)
					s.wave = WAVE_SAWTOOTH;
				else if (strcmp(sweep.waveform, "Noise") == 0)
					s.wave = WAVE_NOISE;
				else {
					ServiceLocator::getLoggingService().error("Unrecognized waveform (defaulting to Sine)", sweep.waveform);
					s.wave = WAVE_SINE;
				}
				mSweeps.push_back(s);
			}
			else if (file.extract("\\S\\L", &err)) {
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
	file.close();
}

float lerp(float v1, float v2, float fac) {
	return (1 - fac)*v1 + fac * v2;
}

float logp(float v1, float v2, float fac) {
	return exp(lerp(log(v1), log(v2), fac));
}

#include <math.h>
#ifndef PI
#define PI 3.14159
#endif

float Sine(float x) {
	return sin(x);
}

float Square(float x) {
	return (fmod(x, 2 * PI) < PI) ? -1 : 1;
}

float Sawtooth(float x) {
	x = fmod(x, 2 * PI);
	return x / PI - 1;
}

void ProceduralSound::build() {
	float length = 0.0;
	for (Point point : mPoints)
		if (point.time > length) length = point.time;
	mNumSamples = int(length * sSampleRate);
	mData = new float[mNumSamples];
	for (int i = 0; i < mNumSamples; i++)
		mData[i] = 0.0f;
	for (auto sweep : mSweeps) {
		int startIndex = sweep.p1->time * sSampleRate;
		int endIndex = sweep.p2->time * sSampleRate;
		float time = (float)(endIndex - startIndex) / sSampleRate;
		if (sweep.wave == WAVE_NOISE) {
			// Skip the interpolation, since frequency has no effect on noise
			for (int i = startIndex; i < endIndex; i++)
				mData[i] += lerp(sweep.p1->gain, sweep.p2->gain, (float)(i - startIndex) / (endIndex - startIndex)) * (float)rand() / RAND_MAX;
		}
		else switch (sweep.type) {
		case INTER_CONSTANT:
			switch (sweep.wave) {
			case WAVE_SINE:
				for (int i = 0; i < endIndex - startIndex; i++) {
					float fac = (float)i / (endIndex - startIndex);
					mData[startIndex + i] += sweep.p1->gain * Sine(sweep.p1->freq * fac * time * 2*PI);
				}
				break;
			case WAVE_SQUARE:
				for (int i = 0; i < endIndex - startIndex; i++) {
					float fac = (float)i / (endIndex - startIndex);
					mData[startIndex + i] += sweep.p1->gain * Square(sweep.p1->freq * fac * time * 2 * PI);
				}
				break;
			case WAVE_SAWTOOTH:
				for (int i = 0; i < endIndex - startIndex; i++) {
					float fac = (float)i / (endIndex - startIndex);
					mData[startIndex + i] += sweep.p1->gain * Sawtooth(sweep.p1->freq * fac * time * 2 * PI);
				}
				break;
			}
			break;
		case INTER_LINEAR:
			switch (sweep.wave) {
			case WAVE_SINE:
				for (int i = 0; i < endIndex - startIndex; i++) {
					float fac = (float)i / (endIndex - startIndex);
					mData[startIndex + i] += lerp(sweep.p1->gain, sweep.p2->gain, fac) * Sine(lerp(sweep.p1->freq, sweep.p2->freq, fac) * fac * time * 2*PI);
				}
				break;
			case WAVE_SQUARE:
				for (int i = 0; i < endIndex - startIndex; i++) {
					float fac = (float)i / (endIndex - startIndex);
					mData[startIndex + i] += lerp(sweep.p1->gain, sweep.p2->gain, fac) * Square(lerp(sweep.p1->freq, sweep.p2->freq, fac) * fac * time * 2 * PI);
				}
				break;
			case WAVE_SAWTOOTH:
				for (int i = 0; i < endIndex - startIndex; i++) {
					float fac = (float)i / (endIndex - startIndex);
					mData[startIndex + i] += lerp(sweep.p1->gain, sweep.p2->gain, fac) * Sawtooth(lerp(sweep.p1->freq, sweep.p2->freq, fac) * fac * time * 2 * PI);
				}
				break;
			}
			break;
		case INTER_LOG:	// Be sure to check and make sure neither frequency is 0
			if (sweep.p1->freq == 0.0f) sweep.p1->freq += 0.01;
			if (sweep.p2->freq == 0.0f) sweep.p2->freq += 0.01;
			switch (sweep.wave) {
			case WAVE_SINE:
				for (int i = 0; i < endIndex - startIndex; i++) {
					float fac = (float)i / (endIndex - startIndex);
					mData[startIndex + i] += lerp(sweep.p1->gain, sweep.p2->gain, fac) * Sine(logp(sweep.p1->freq, sweep.p2->freq, fac) * fac * time * 2 * PI);
				}
				break;
			case WAVE_SQUARE:
				for (int i = 0; i < endIndex - startIndex; i++) {
					float fac = (float)i / (endIndex - startIndex);
					mData[startIndex + i] += lerp(sweep.p1->gain, sweep.p2->gain, fac) * Square(logp(sweep.p1->freq, sweep.p2->freq, fac) * fac * time * 2 * PI);
				}
				break;
			case WAVE_SAWTOOTH:
				for (int i = 0; i < endIndex - startIndex; i++) {
					float fac = (float)i / (endIndex - startIndex);
					mData[startIndex + i] += lerp(sweep.p1->gain, sweep.p2->gain, fac) * Sawtooth(logp(sweep.p1->freq, sweep.p2->freq, fac) * fac * time * 2 * PI);
				}
				break;
			}
			break;
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