#include "Sample States/MouseoverStates.h"
#include "Core/GameObject.h"
#include "Input/MouseHandler.h"

bool MouseoverState::sRegistered = State::setBaseState("mouseover", new MouseoverState(nullptr));

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
