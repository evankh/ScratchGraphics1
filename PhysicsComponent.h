#ifndef __EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__
#define __EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__

#include "glm/matrix.hpp"
#include "glm/vec3.hpp"
class Bounds;

class PhysicsComponent {
private:
	glm::mat4 mModelMatrix;
	glm::vec3 mVelocity;
	glm::vec3 mAcceleration;
	Bounds* mBounds = NULL;
public:
	PhysicsComponent();
	PhysicsComponent(glm::mat4 &mm);
	~PhysicsComponent();
	void update(float dt);
	void translate(glm::vec3 dxyz);
	void scale(glm::vec3 scale);
	glm::mat4& getModelMatrix();
	glm::mat4& getInverseModelMatrix();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__