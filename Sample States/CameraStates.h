#ifndef __EKH_SCRATCH_GRAPHICS_1_CAMERA_STATES__
#define __EKH_SCRATCH_GRAPHICS_1_CAMERA_STATES__

#include "../State.h"
#include "glm/vec3.hpp"
class GameObject;

class CameraState :public State {
	static bool sRegistered;
private:
	GameObject* mOwner;
	bool mLeftDrag = false, mRightDrag = false;
	int mLeftDragStartPos[2], mRightDragStartPos[2];
	PhysicsComponent* mDragStart = nullptr;
	glm::vec3 mOrbitCenter;
	float mSpeed = 0.1f;
public:
	CameraState(GameObject* owner) :mOwner(owner) {};
	CameraState* getEntry(GameObject* owner) final;
	CameraState* handleEvent(Event event) final;
	void update(PhysicsComponent* physics, float dt) final;
	CameraState* copy(GameObject* owner) const final { return new CameraState(owner); };
	void destroy() final;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_CAMERA_STATES__