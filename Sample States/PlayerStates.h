#ifndef __EKH_SCRATCH_GRAPHICS_1_SAMPLE_PLAYER_STATES__
#define __EKH_SCRATCH_GRAPHICS_1_SAMPLE_PLAYER_STATES__

#include "../State.h"
class GameObject;

#ifndef NULL
#define NULL 0
#endif//NULL

class PassThruState :public State {
public:
	PassThruState* handleEvent(Event event) { return NULL; };
	void update(PhysicsComponent* physics, float dt);
	static PassThruState* getEntry(GameObject* owner) { return new PassThruState(); };
};

class PlayerState :public State {
public:
	static PlayerState* getEntry(GameObject* owner);
};

class PlayerOnFloor :public PlayerState {
private:
	float dx, dy;
public:
	PlayerOnFloor(float dx, float dy) :dx(dx), dy(dy) {};
	PlayerState* handleEvent(Event event);
	void update(PhysicsComponent* physics, float dt);
	void enter(PhysicsComponent* physics);
};

class PlayerJumping :public PlayerState {
private:
	float dx, dy;
public:
	PlayerJumping(float dx, float dy) :dx(dx), dy(dy) {};
	PlayerState* handleEvent(Event event);
	void update(PhysicsComponent* physics, float dt);
	void enter(PhysicsComponent* physics);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SAMPLE_PLAYER_STATES__