#ifndef __EKH_SCRATCH_GRAPHICS_1_STATE__
#define __EKH_SCRATCH_GRAPHICS_1_STATE__

#include "Event System/Event.h"
#include "NamedContainer.h"
class PhysicsComponent;
class GameObject;

class State {
protected:
	static NamedContainer<State*> sBaseStateLibrary;
public:
	virtual State* getEntry(GameObject* parent) = 0;
	virtual State* handleEvent(Event event) = 0;
	virtual void update(PhysicsComponent* physics, float dt) = 0;
	virtual void enter(PhysicsComponent* physics) {};
	virtual void exit() {};
	static State* getNewEntryState(std::string name, GameObject* parent) { return sBaseStateLibrary.get(name)->getEntry(parent); };
	static bool setBaseState(std::string name, State* func) { sBaseStateLibrary.add(name, func); return true; };
};

// The one and only default state: simply applies the object's physics and ignores all input
class PassThruState :public State {
	static bool sRegistered;
public:
	PassThruState* handleEvent(Event event) { return NULL; };
	void update(PhysicsComponent* physics, float dt);
	virtual PassThruState* getEntry(GameObject* owner) { return new PassThruState(); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_STATE__