#ifndef __EKH_SCRATCH_GRAPHICS_1_ARCBALL_STATES__
#define __EKH_SCRATCH_GRAPHICS_1_ARCBALL_STATES__

#include "Core/State.h"
#include "include/glm/glm/gtc/quaternion.hpp"

class ArcballState :public State {
	static bool sRegistered;
private:
	GameObject* mOwner;
	glm::vec3 mStartingClickVector;
	glm::quat mStartingOrientation;
	bool mClicked = false;
public:
	ArcballState(GameObject* owner) :mOwner(owner) {};
	ArcballState* getEntry(GameObject* owner) final;
	ArcballState* handleEvent(Event event) final;
	void update(PhysicsComponent* physics, float dt) final;
	ArcballState* copy(GameObject* owner) const final { return new ArcballState(owner); };
	void destroy() final;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_ARCBALL_STATES__