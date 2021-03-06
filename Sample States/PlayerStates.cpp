#include "Sample States/PlayerStates.h"
#include "Components/Physics.h"
#include "Core/GameObject.h"
#include "Input/KeyboardHandler.h"

bool PlayerState::sRegistered = State::setBaseState("player", new PlayerOnFloor(NULL, 0.0, 0.0, 0.0));	// It's fine for this to be unowned, because it's only used to generate copies

void PlayerState::destroy() {
	KeyboardHandler::getInstance().unregisterReceiver(mOwner);
}

PlayerState* PlayerState::getEntry(GameObject* owner) {
	KeyboardHandler::getInstance().registerReceiver("wasde j", owner);
	return new PlayerOnFloor(owner, 0.0f, 0.0f, 0.0f);
}

PlayerState* PlayerOnFloor::handleEvent(Event event) {
	if (event.mType == EventType::COMMAND) {
		// Probably will only work for joystick input, since W & S aren't mutually exclusive like up/down positions of the stick, nor automatically restting
		switch (event.mData.command.command) {
		case Command::MOVE_FORWARD:
			dy = event.mData.command.factor;
			break;
		case Command::MOVE_LEFT:
			dx = -event.mData.command.factor;
			break;
		case Command::MOVE_BACKWARD:
			dy = -event.mData.command.factor;
			break;
		case Command::MOVE_RIGHT:
			dx = event.mData.command.factor;
			break;
		case Command::JUMP:
			return new PlayerJumping(mOwner, dx, dy, dz);
		}
	}
	if (event.mType == EventType::KEY_PRESSED) {
		switch (event.mData.keyboard.key) {
		case 'w':
			dy += 1.0f;
			break;
		case 'a':
			dx -= 1.0f;
			break;
		case 's':
			dy -= 1.0f;
			break;
		case 'd':
			dx += 1.0f;
			break;
		case 'e':
			dz -= 1.0f;
			break;
		case ' ':
			dz += 1.0f;
			break;
		case 'j':
			return new PlayerJumping(mOwner, dx, dy, dz);
		}
	}
	if (event.mType == EventType::KEY_RELEASED) {
		switch (event.mData.keyboard.key) {
		case 'w':
			dy -= 1.0f;
			break;
		case 'a':
			dx += 1.0f;
			break;
		case 's':
			dy += 1.0f;
			break;
		case 'd':
			dx -= 1.0f;
			break;
		case 'e':
			dz += 1.0f;
			break;
		case ' ':
			dz -= 1.0f;
			break;
		}
	}
	return NULL;
}

PlayerState* PlayerJumping::handleEvent(Event event) {
	if (event.mType == EventType::COLLISION) {
		// More advanced stuff will store the PhysicsComponents in the CollisionData so it can do proper response
		return new PlayerOnFloor(mOwner, dx, dy, dz);
	}
	// Even though velocity is maintained through the jump, it should be reset properly on hitting the ground
	if (event.mType == EventType::KEY_PRESSED) {
		switch (event.mData.keyboard.key) {
		case 'w':
			dy += 1.0f;
			break;
		case 'a':
			dx -= 1.0f;
			break;
		case 's':
			dy -= 1.0f;
			break;
		case 'd':
			dx += 1.0f;
			break;
		case 'e':
			dz -= 1.0f;
			break;
		case ' ':
			dz += 1.0f;
			break;
		}
	}
	if (event.mType == EventType::KEY_RELEASED) {
		switch (event.mData.keyboard.key) {
		case 'w':
			dy -= 1.0f;
			break;
		case 'a':
			dx += 1.0f;
			break;
		case 's':
			dx -= 1.0f;
			break;
		case 'd':
			dy += 1.0f;
			break;
		case 'e':
			dz += 1.0f;
			break;
		case ' ':
			dz -= 1.0f;
			break;
		}
	}
	return NULL;
}

void PlayerOnFloor::enter(PhysicsComponent* physics) {
	physics->setAcceleration({ 0.0f,0.0f,0.0f });
}

void PlayerJumping::enter(PhysicsComponent* physics) {
	physics->setVelocity({ dx,dy,5.0f });
	physics->setAcceleration({ 0.0f,0.0f,-9.8f });
}

void PlayerOnFloor::update(PhysicsComponent* physics, float dt) {
	physics->setVelocity({ dx,dy,dz });
	physics->update(dt);
}

void PlayerJumping::update(PhysicsComponent* physics, float dt) {
	physics->update(dt);
}