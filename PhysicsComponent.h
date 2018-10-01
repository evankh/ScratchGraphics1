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
	glm::vec3 mAxis;
	float mAngularVelocity;
	Bounds* mBounds = NULL;
public:
	PhysicsComponent();
	PhysicsComponent(glm::mat4 &mm);
	PhysicsComponent(glm::vec3 axis, float aVel);	// For our debug rotating objects
	PhysicsComponent(glm::vec3 vel, glm::vec3 acc, glm::vec3 axis, float aVel);	// For reading them in from Level files (MM is taken care of through transform functions)
	~PhysicsComponent();
	void update(float dt);
	void translate(glm::vec3 dxyz);
	void rotate(glm::vec3 axis, float degrees);
	void scale(glm::vec3 scale);
	glm::mat4& getModelMatrix();
	glm::mat4& getInverseModelMatrix();
	glm::vec4 getPosition();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__