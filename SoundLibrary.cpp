#include "SoundLibrary.h"
#include "Sound.h"

void SoundLibrary::add(std::string name, Sound* sound) {
	mSounds[name] = sound;
}

Sound* SoundLibrary::get(std::string name) const {
	if (mSounds.count(name))
		return mSounds.at(name);
	return NULL;
}

void SoundLibrary::clear() {
	// This should only be called by Game, never by GameObjects
	for (auto sound : mSounds)
		delete sound.second;
	mSounds.clear();
}