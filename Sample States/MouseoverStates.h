#ifndef __EKH_SCRATCH_GRAPHICS_1_MOUSEOVER_STATES__
#define __EKH_SCRATCH_GRAPHICS_1_MOUSEOVER_STATES__

#include "../State.h"
class GameObject;

class MouseoverState :public State {
	static bool sRegistered;
private:
	GameObject* mOwner;
public:
	MouseoverState(GameObject* owner) :mOwner(owner) {};
	MouseoverState* getEntry(GameObject* owner) final;
	MouseoverState* handleEvent(Event event) final { return NULL; };	// It's actually the GameObject that's holding the bool to determine what color it will be drawn as
	void update(PhysicsComponent* physics, float dt) final;
	MouseoverState* copy(GameObject* owner) const final { return new MouseoverState(owner); };
	void destroy() final;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_MOUSEOVER_STATES__