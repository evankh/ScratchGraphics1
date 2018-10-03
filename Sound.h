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
	float* mData = NULL;
	float mGain = 1.0f;
public:
	~Sound();
	void transfer();
	float getGain() const { return mGain; };
	unsigned int getBuffer()const { return mHandle; };
};

struct Point {
	float time;
	float freq;
	float gain = 1.0f;
	float phase = 0.0f;
};

enum InterType {
	INTER_LINEAR,
	INTER_LOG,
	INTER_CONSTANT
};

enum WaveType {
	WAVE_SINE,
	WAVE_SQUARE,
	WAVE_SAWTOOTH,
	WAVE_NOISE
};

struct Sweep {
	Point* p1;
	Point* p2;
	InterType type;
	WaveType wave;
};

class ProceduralSound :public Sound {
private:
	static bool sHaveGeneratedNoteTable;
	static std::map<std::string, float>* sNoteTable;
	static std::vector<std::string> sNoteNames;
	static void generateNoteTable();

	std::vector<Point> mPoints;
	std::vector<Sweep> mSweeps;
	void build();
public:
	ProceduralSound(std::string filename);
};

class FileSound :public Sound {
public:
	FileSound(std::string filename);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SOUND__