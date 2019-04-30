#ifndef __EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__
#define __EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__

#include "glm/matrix.hpp"
#include "glm/vec3.hpp"
class Bounds;

class PhysicsComponent {
private:
	//glm::mat4 mModelMatrix;
	glm::vec3 mScale;
	glm::vec3 mPosition;
	glm::vec3 mVelocity;
	glm::vec3 mAcceleration;
	glm::mat4 mRotation;
	glm::vec3 mAxis;
	float mAngularVelocity;
	Bounds* mBounds = NULL;
public:
	PhysicsComponent();
	PhysicsComponent(glm::vec3 pos, glm::vec3 towards);	// For a Camera
	PhysicsComponent(glm::vec3 axis, float aVel);	// For our debug rotating objects
	PhysicsComponent(Bounds* bounds, glm::vec3 vel, glm::vec3 acc, glm::vec3 axis, float aVel);	// For reading them in from Level files (MM is taken care of through transform functions)
	~PhysicsComponent();
	void update(float dt);
	void translate(glm::vec3 dxyz);
	void setPosition(glm::vec3 pos) { mPosition = pos; };
	void setVelocity(glm::vec3 vel) { mVelocity = vel; };
	void setAcceleration(glm::vec3 acc) { mAcceleration = acc; };
	void setRotation(glm::vec3 axis, float mom) { mAxis = axis; mAngularVelocity = mom; };
	void set(const PhysicsComponent* other);
	void rotate(glm::vec3 axis, float degrees);
	void scale(glm::vec3 scale);
	glm::mat4 getModelMatrix();
	glm::mat4 getInverseModelMatrix();
	glm::vec3 getPosition() const { return mPosition; };
	glm::vec3 getVelocity() const { return mVelocity; };
	Bounds* getBounds() const { return mBounds; };
	float* getOrientationVectors() const;
	PhysicsComponent* copy() const;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__