#ifndef __EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__
#define __EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__

#include "glm/matrix.hpp"
#include "glm/vec3.hpp"
class CollisionComponent;

class PhysicsComponent {
private:
	//glm::mat4 mModelMatrix;
	glm::vec3 mScale;
	glm::vec3 mPosition;
	glm::vec3 mVelocity;
	glm::vec3 mAcceleration;
	glm::mat4 mRotation;
	// Euler Angles
	glm::vec3 mAngles, mAngularVelocities, mAngularAccelerations;
	// This thing reeeaaally needs quaternions or something
	CollisionComponent* mBounds = nullptr;
	PhysicsComponent* mParent = nullptr;
public:
	PhysicsComponent();
	PhysicsComponent(glm::vec3 pos, glm::vec3 towards);	// For a Camera
	PhysicsComponent(glm::vec3 angles, glm::vec3 aVels, glm::vec3 aAccs);	// For our debug rotating objects
	PhysicsComponent(CollisionComponent* bounds, glm::vec3 vel, glm::vec3 acc, glm::vec3 angles, glm::vec3 aVels);	// For reading them in from Level files (MM is taken care of through transform functions)
	~PhysicsComponent();
	void setParent(PhysicsComponent* parent) { mParent = parent; };
	void update(float dt);
	void translate(glm::vec3 dxyz);
	void rotate(glm::vec3 axis, float degrees);
	void rotateAzimuth(float degrees);
	void rotateAltitude(float degrees);
	void rotateOrientation(float degrees);
	void scale(glm::vec3 scale);
	void setPosition(glm::vec3 pos) { mPosition = pos; };
	void setVelocity(glm::vec3 vel) { mVelocity = vel; };
	void setAcceleration(glm::vec3 acc) { mAcceleration = acc; };
	glm::mat4 getWorldTransform() const;
	glm::mat4 getInverseWorldTransform() const;
	glm::vec3 getLocalPosition() const { return mPosition; };
	glm::vec3 getGlobalPosition() const;
	glm::vec3 getVelocity() const { return mVelocity; };
	CollisionComponent* getBounds() const { return mBounds; };
	float* getOrientationVectors() const;
	PhysicsComponent* makeCopy() const;
	void copyFrom(const PhysicsComponent* other);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_PHYSICS_COMPONENT__