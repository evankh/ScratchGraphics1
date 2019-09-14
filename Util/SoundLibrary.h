#ifndef __EKH_SCRATCH_GRAPHICS_1_SOUND_LIBRARY__
#define __EKH_SCRATCH_GRAPHICS_1_SOUND_LIBRARY__

#include <map>
class Sound;
#include <string>

class SoundLibrary {
private:
	std::map<std::string, Sound*> mSounds;
public:
	void add(std::string name, Sound* sound);
	Sound* get(std::string name) const;
	void clear();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SOUND_LIBRARY__