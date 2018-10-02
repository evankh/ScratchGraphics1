#include "SoundHandler.h"
#include "al.h"
#include "PhysicsComponent.h"
#include "glm\gtc\type_ptr.hpp"

PhysicsComponent* SoundHandler::sListener = NULL;

SoundHandler::SoundHandler() {
	// Set up the OpenAL context
	sDevice = alcOpenDevice(NULL);
	sContext = alcCreateContext(sDevice, NULL);
	alcMakeContextCurrent(sContext);
}

SoundHandler::~SoundHandler() {
	// Destroy OpenAL context
	alcSuspendContext(sContext);
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
	if (sListener) {
		alListenerfv(AL_POSITION, glm::value_ptr(sListener->getPosition()));
		alListenerfv(AL_VELOCITY, glm::value_ptr(sListener->getVelocity()));
	}
}