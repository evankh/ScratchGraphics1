#ifndef __EKH_SCRATCH_GRAPHICS_1_STATE__
#define __EKH_SCRATCH_GRAPHICS_1_STATE__

#include "Event System/Event.h"
class PhysicsComponent;

class State {
public:
	virtual State* handleEvent(Event event) = 0;
	virtual void update(PhysicsComponent* physics, float dt) = 0;
	virtual void enter(PhysicsComponent* physics) {};
	virtual void exit() {};
};

#endif//__EKH_SCRATCH_GRAPHICS_1_STATE__