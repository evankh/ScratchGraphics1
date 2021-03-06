#ifndef __EKH_SCRATCH_GRAPHICS_1_SOUND__
#define __EKH_SCRATCH_GRAPHICS_1_SOUND__

#include <map>
#include <string>
#include <vector>

class Sound {
protected:
	Sound(std::string filename);
	static int sSampleRate;
	std::string mFilename;
	unsigned int mHandle = 0;
	int mNumSamples = 0;
	float* mData = nullptr;
	float mGain = 1.0f;
public:
	~Sound();
	virtual void transfer();
	float getGain() const { return mGain; };
	unsigned int getBuffer()const { return mHandle; };
};

float interpConstant(float v1, float v2, float fac);
float interpLinear(float v1, float v2, float fac);
float interpLogarithmic(float v1, float v2, float fac);
float waveSine(float x);
float waveSquare(float x);
float waveSawtooth(float x);
float waveNoise(float x);

struct Point {
	float time;
	float freq;
	float gain = 1.0f;
	float phase = 0.0f;
};

struct Envelope {
	float delayTime = 0.0f, delayAmplitude = 1.0f;
	float attackTime = 0.0f, attackAmplitude = 1.0;;
	float decayTime = 0.0f, decayAmplitude = 1.0f;
	float releaseTime = 0.0f, releaseAmplitude = 0.0f;
};

struct Sweep {
	Point* p1;
	Point* p2;
	float(*gain)(float, float, float) = interpConstant;
	float(*freq)(float, float, float) = interpConstant;
	float(*wave)(float) = waveSine;
	Envelope adsr;
};

class ProceduralSound :public Sound {
	static bool sHaveGeneratedNoteTable;
	static std::map<std::string, float>* sNoteTable;
	static std::vector<std::string> sNoteNames;
	static void generateNoteTable();
private:
	std::vector<Point> mPoints;
	std::vector<Sweep> mSweeps;
	void build();
public:
	ProceduralSound(std::string filename);
	~ProceduralSound();
};

class FileSound :public Sound {
private:
	short* mShortData;
	int mNumChannels;
	int mSampleRate;
public:
	FileSound(std::string filename);
	~FileSound();
	virtual void transfer();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SOUND__