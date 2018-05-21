#include "PhysicsComponent.h"
#include "Bounds.h"
#include <glm\gtc\matrix_transform.hpp>

PhysicsComponent::PhysicsComponent() {
	mModelMatrix = glm::mat4(1.0f);
	mVelocity = glm::vec3(0.0f);
	mAcceleration = glm::vec3(0.0f);
	mBounds = new Collisionless();
}

PhysicsComponent::PhysicsComponent(glm::mat4 &mm) {
	mModelMatrix = mm;
	mVelocity = glm::vec3(0.0f);
	mAcceleration = glm::vec3(0.0f);
	mBounds = new Collisionless();
}

PhysicsComponent::~PhysicsComponent() {
	delete mBounds;
}

void PhysicsComponent::update(float dt) {
	mVelocity += mAcceleration * dt;
	translate(mVelocity * dt);
}

void PhysicsComponent::translate(glm::vec3 dxyz) {
	mModelMatrix = glm::translate(mModelMatrix, dxyz);
}

void PhysicsComponent::scale(glm::vec3 scale) {
	mModelMatrix = glm::scale(mModelMatrix, scale);
}

glm::mat4& PhysicsComponent::getModelMatrix() {
	return mModelMatrix;
}

glm::mat4& PhysicsComponent::getInverseModelMatrix() {
	return glm::inverse(mModelMatrix);
}