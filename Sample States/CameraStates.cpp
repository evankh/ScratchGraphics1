#include "CameraStates.h"
#include "../GameObject.h"
#include "../MouseHandler.h"
#include "../ServiceLocator.h"

bool CameraState::sRegistered = State::setBaseState("camera", new CameraState(nullptr));

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
		mDragStart = mOwner->getPhysicsComponent()->copy();
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
		delete mDragStart;
		mDragStart = nullptr;
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
	return nullptr;
}

void CameraState::update(PhysicsComponent* physics, float dt) {
	float dx, dy;
	int current_x = MouseHandler::getInstance().getMouseX();
	int current_y = MouseHandler::getInstance().getMouseY();
	if (mLeftDrag) {
		dx = current_x - mLeftDragStartPos[0];
		dy = current_y - mLeftDragStartPos[1];
		physics->set(mDragStart);
		physics->rotateGlobalX(mSpeed * dx);
		physics->rotateGlobalY(mSpeed * dy);
	}
	if (mRightDrag) {
		dx = current_x - mRightDragStartPos[0];
		dy = current_y - mRightDragStartPos[1];
		physics->set(mDragStart);
		glm::vec3 diff = physics->getPosition() - mOrbitCenter;
		physics->translate(-diff);
		diff.x *= cosf(mSpeed * dx) - sinf(mSpeed * dx);
		diff.y *= -sinf(mSpeed * dx) + cosf(mSpeed * dx);
		diff.z *= sinf(mSpeed * dy);
		physics->translate(diff);
		physics->rotateGlobalX(mSpeed * dx);
		physics->rotateGlobalY(mSpeed * dy);
	}
}