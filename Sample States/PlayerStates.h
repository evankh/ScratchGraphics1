#ifndef __EKH_SCRATCH_GRAPHICS_1_SAMPLE_PLAYER_STATES__
#define __EKH_SCRATCH_GRAPHICS_1_SAMPLE_PLAYER_STATES__

#include "Core/State.h"

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
	float dx, dy, dz = 0.0f;
public:
	PlayerOnFloor(GameObject* owner, float dx, float dy, float dz) :dx(dx), dy(dy), dz(dz) { mOwner = owner; };
	PlayerState* handleEvent(Event event);
	void update(PhysicsComponent* physics, float dt);
	void enter(PhysicsComponent* physics);
	PlayerState* copy(GameObject* owner) const { return new PlayerOnFloor(owner,dx,dy,dx ); };
};

class PlayerJumping :public PlayerState {
private:
	float dx, dy, dz;
public:
	PlayerJumping(GameObject* owner, float dx, float dy, float dz) :dx(dx), dy(dy), dz(dz) { mOwner = owner; };
	PlayerState* handleEvent(Event event);
	void update(PhysicsComponent* physics, float dt);
	void enter(PhysicsComponent* physics);
	PlayerState* copy(GameObject* owner) const { return new PlayerJumping(owner, dx, dy, dz); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SAMPLE_PLAYER_STATES__