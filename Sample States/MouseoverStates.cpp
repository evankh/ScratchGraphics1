#include "MouseoverStates.h"
#include "../GameObject.h"
#include "../MouseHandler.h"

bool MouseoverState::sRegistered = State::setBaseState("mouseover", new MouseoverState(NULL));

MouseoverState* MouseoverState::getEntry(GameObject* owner) {
	MouseHandler::getInstance().registerMouseoverReceiver(owner);
	return new MouseoverState(owner);
}

void MouseoverState::update(PhysicsComponent* physics, float dt) {
	physics->update(dt);
}

void MouseoverState::destroy() {
	MouseHandler::getInstance().unregisterReceiver(mOwner);
}
