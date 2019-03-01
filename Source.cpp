#include "Source.h"
#include "al.h"
#include "glm\gtc\type_ptr.hpp"
#include "PhysicsComponent.h"
#include "Sound.h"

Source::Source(PhysicsComponent* physics, bool looping) {
	mPhysics = physics;
	mLooping = looping;
	alGenSources(1, &mHandle);
}

Source::~Source() {
	alSourceStop(mHandle);
	alDeleteSources(1, &mHandle);
}

void Source::update() {
	alSourcefv(mHandle, AL_POSITION, glm::value_ptr(mPhysics->getPosition()));
	alSourcefv(mHandle, AL_VELOCITY, glm::value_ptr(mPhysics->getVelocity()));
	alSourcei(mHandle, AL_LOOPING, mLooping);
}

void Source::playSound(Sound* sound) {
	alGetSourcei(mHandle, AL_SOURCE_STATE, &mState);
	if (mState != AL_PLAYING) {
		alSourcef(mHandle, AL_GAIN, mGain * sound->getGain());
		alSourcei(mHandle, AL_BUFFER, sound->getBuffer());
		alSourcePlay(mHandle);
	}
}

void Source::setVolume(float volume) {
	alSourcef(mHandle, AL_GAIN, volume);
}

Source* Source::copy(PhysicsComponent* phys) const {
	return new Source(phys, mLooping);
}