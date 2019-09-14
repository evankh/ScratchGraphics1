#include "Core/State.h"
#include "Components/Physics.h"

//NamedContainer<State*> State::sBaseStateLibrary;	// OK, so it can't go here for some reason, but it works at the top of Game.cpp?
bool PassThruState::sRegistered = State::setBaseState("passthru", new PassThruState());

void PassThruState::update(PhysicsComponent* physics, float dt) {
	physics->update(dt);
}