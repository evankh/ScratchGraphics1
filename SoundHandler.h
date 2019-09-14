#ifndef __EKH_SCRATCH_GRAPHICS_1_SOUND_HANDLER__
#define __EKH_SCRATCH_GRAPHICS_1_SOUND_HANDLER__

class PhysicsComponent;
#include "OpenAL/alc.h"

class SoundHandler {
private:
	SoundHandler();
	static PhysicsComponent* sListener;
	ALCdevice* sDevice;
	ALCcontext* sContext;
public:
	~SoundHandler();
	static SoundHandler& getInstance();
	static void registerListener(PhysicsComponent* listener);
	static void update();
	static void setVolume(float volume);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SOUND_HANDLER__