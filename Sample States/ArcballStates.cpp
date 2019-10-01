#include "Sample States/ArcballStates.h"
#include "Core/GameObject.h"
#include "Input/MouseHandler.h"

bool ArcballState::sRegistered = State::setBaseState("arcball", new ArcballState(nullptr));

ArcballState* ArcballState::getEntry(GameObject* owner) {
	MouseHandler::getInstance().registerReceiver(MouseButton::EKH_MOUSE_BUTTON_LEFT, owner);
	glm::quat identity;
	identity.w = 1.0f;
	owner->getPhysicsComponent()->setQuaternionRotation(identity);
	return new ArcballState(owner);
}

ArcballState* ArcballState::handleEvent(Event event) {
	switch (event.mType) {
	case EventType::BUTTON_PRESSED: {
		// A nice addition: circles of different radii, and ones which are not centered on the screen
		glm::vec2 click = MouseHandler::getInstance().getAspectCorrectedNDC();
		float z;
		if (glm::length(click) >= 1.0f) {
			glm::normalize(click);
			z = 0.0f;
		}
		else z = glm::sqrt(1 - click.x * click.x - click.y * click.y);
		mStartingClickVector = glm::vec3(click, z);
		mClicked = true;
		mStartingOrientation = mOwner->getPhysicsComponent()->getQuaternionRotation();
		break;
	}
	case EventType::BUTTON_RELEASED:
		mClicked = false;
		break;
	}
	return nullptr;
}

void ArcballState::update(PhysicsComponent* physics, float dt) {
	if (mClicked) {
		glm::vec2 click = MouseHandler::getInstance().getAspectCorrectedNDC();
		float z;
		if (glm::length(click) >= 1.0f) {
			glm::normalize(click);
			z = 0.0f;
		}
		else z = glm::sqrt(1 - click.x * click.x - click.y * click.y);
		glm::vec3 current = glm::vec3(click, z);
		glm::quat delta;
		// Using the dot and cross products directly means the ball will rotate twice as much as the difference of the click vectors,
		// but avoids several expensive trigonometric functions. It's a feature, not a bug; this allows you to rotate all the way
		// around the object with a single drag.
		delta.w = glm::dot(current, mStartingClickVector);
		glm::vec3 axis = glm::cross(current, mStartingClickVector);
		delta.x = axis.x;
		delta.y = axis.y;
		delta.z = axis.z;
		physics->setQuaternionRotation(delta * mStartingOrientation /* glm::conjugate(delta)*/);
	}
}
void ArcballState::destroy() {
	MouseHandler::getInstance().unregisterReceiver(mOwner);
}