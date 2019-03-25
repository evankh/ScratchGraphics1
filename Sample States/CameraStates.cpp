#include "CameraStates.h"
#include "../GameObject.h"
#include "../MouseHandler.h"
#include "../ServiceLocator.h"

bool CameraState::sRegistered = State::setBaseState("camera", new CameraState(NULL));

CameraState* CameraState::getEntry(GameObject* owner) {
	MouseHandler::getInstance().registerReceiver(MouseButton::EKH_MOUSE_BUTTON_LEFT, owner);
	MouseHandler::getInstance().registerReceiver(MouseButton::EKH_MOUSE_BUTTON_RIGHT, owner);
	return new CameraState(owner);
}

void CameraState::destroy() {
	MouseHandler::getInstance().unregisterReceiver(mOwner);
}

CameraState* CameraState::handleEvent(Event event) {
	switch (event.mType) {
	case EventType::BUTTON_PRESSED:
		switch (event.mData.mouse.button) {
		case MouseButton::EKH_MOUSE_BUTTON_LEFT:
			mLeftDrag = true;
			mLeftDragStartPos[0] = event.mData.mouse.mouse_x;
			mLeftDragStartPos[1] = event.mData.mouse.mouse_y;
			break;
		case MouseButton::EKH_MOUSE_BUTTON_RIGHT:
			mRightDrag = true;
			mRightDragStartPos[0] = event.mData.mouse.mouse_x;
			mRightDragStartPos[1] = event.mData.mouse.mouse_y;
			mOrbitCenter = glm::vec3(event.mData.mouse.world_pos[0], event.mData.mouse.world_pos[1], event.mData.mouse.world_pos[2]);
			break;
		}
		break;
	case EventType::BUTTON_RELEASED:
		switch (event.mData.mouse.button) {
		case MouseButton::EKH_MOUSE_BUTTON_LEFT:
			mLeftDrag = false;
			break;
		case MouseButton::EKH_MOUSE_BUTTON_RIGHT:
			mRightDrag = false;
			break;
		}
		break;
	}
	return NULL;
}

void CameraState::update(PhysicsComponent* physics, float dt) {
	static float outputcooldown = 0.0f;
	float dx, dy;
	int current_x = MouseHandler::getInstance().getMouseX();
	int current_y = MouseHandler::getInstance().getMouseY();
	if (mLeftDrag) {
		dx = current_x - mLeftDragStartPos[0];
		dy = current_y - mLeftDragStartPos[1];
		// Something magic
		outputcooldown += dt;
		if (outputcooldown >= 0.1f) {
			ServiceLocator::getLoggingService().log("Currently leftdragging with dx = " + std::to_string(dx) + " and dy = " + std::to_string(dy));
			outputcooldown -= 0.1f;
		}
	}
	if (mRightDrag) {
		dx = current_x - mRightDragStartPos[0];
		dy = current_y - mRightDragStartPos[1];
		// Something magic
		outputcooldown += dt;
		if (outputcooldown >= 0.1f) {
			ServiceLocator::getLoggingService().log("Currently rightdragging with dx = " + std::to_string(dx) + " and dy = " + std::to_string(dy));
			outputcooldown -= 0.1f;
		}
	}
}