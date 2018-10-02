#ifndef __EKH_SCRATCH_GRAPHICS_1_SOURCE__
#define __EKH_SCRATCH_GRAPHICS_1_SOURCE__

class PhysicsComponent;
class Sound;

class Source {
private:
	PhysicsComponent* mPhysics;
	unsigned int mHandle;
	float mGain = 1.0;
	bool mLooping = false;
public:
	Source(PhysicsComponent* physics, bool looping);
	~Source();
	void update();
	void playSound(Sound* sound);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SOURCE__