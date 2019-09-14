#include "Sound/SoundHandler.h"
#include "OpenAL/al.h"
#include "Components/Physics.h"
#include "glm/gtc/type_ptr.hpp"

PhysicsComponent* SoundHandler::sListener = nullptr;

SoundHandler::SoundHandler() {
	// Set up the OpenAL context
	sDevice = alcOpenDevice(nullptr);
	sContext = alcCreateContext(sDevice, nullptr);
	alcMakeContextCurrent(sContext);
}

SoundHandler::~SoundHandler() {
	// Destroy OpenAL context
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(sContext);
	alcCloseDevice(sDevice);
}

SoundHandler& SoundHandler::getInstance() {
	static SoundHandler* sInstance = new SoundHandler();
	return *sInstance;
}

void SoundHandler::registerListener(PhysicsComponent* listener) {
	sListener = listener;
}

void SoundHandler::update() {
	//float orientation[6] = { -1.0,-1.0,-0.5,-0.5,-0.5,1.0 };	// Approximate, but functional
	if (sListener) {
		alListenerfv(AL_POSITION, glm::value_ptr(sListener->getGlobalPosition()));
		alListenerfv(AL_VELOCITY, glm::value_ptr(sListener->getVelocity()));
		float* orientation = sListener->getOrientationVectors();
		alListenerfv(AL_ORIENTATION, orientation);
		delete[] orientation;
	}
}

void SoundHandler::setVolume(float volume) {
	alListenerf(AL_GAIN, volume);
}