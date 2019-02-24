#ifndef __EKH_SCRATCH_GRAPHICS_1_SAMPLE_PLAYER_STATES__
#define __EKH_SCRATCH_GRAPHICS_1_SAMPLE_PLAYER_STATES__

#include "../State.h"
class GameObject;

#ifndef NULL
#define NULL 0
#endif//NULL

class PlayerState :public State {
	static bool sRegistered;
protected:
	GameObject* mOwner;
public:
	virtual PlayerState* getEntry(GameObject* owner);
	virtual void destroy();
};

class PlayerOnFloor :public PlayerState {
private:
	float dx, dy;
public:
	PlayerOnFloor(GameObject* owner, float dx, float dy) :dx(dx), dy(dy) { mOwner = owner; };
	PlayerState* handleEvent(Event event);
	void update(PhysicsComponent* physics, float dt);
	void enter(PhysicsComponent* physics);
};

class PlayerJumping :public PlayerState {
private:
	float dx, dy;
public:
	PlayerJumping(GameObject* owner, float dx, float dy) :dx(dx), dy(dy) { mOwner = owner; };
	PlayerState* handleEvent(Event event);
	void update(PhysicsComponent* physics, float dt);
	void enter(PhysicsComponent* physics);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SAMPLE_PLAYER_STATES__