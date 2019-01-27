#include "PhysicsComponent.h"
#include "Bounds.h"
#include <glm\gtc\matrix_transform.hpp>

PhysicsComponent::PhysicsComponent() {
	mScale = glm::vec3(1.0f);
	mPosition = glm::vec3(0.0f);
	mVelocity = glm::vec3(0.0f);
	mAcceleration = glm::vec3(0.0f);
	mRotation = glm::mat4(1.0f);
	mAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	mAngularVelocity = 0.0f;
	mBounds = new Collisionless();
}

PhysicsComponent::PhysicsComponent(glm::vec3 pos, glm::vec3 towards) :PhysicsComponent() {
	mPosition = pos;
	mRotation = glm::inverse(glm::lookAt(pos, towards, glm::vec3(0.0f, 0.0f, 1.0f)));
}

PhysicsComponent::PhysicsComponent(glm::vec3 axis, float aVel) :PhysicsComponent() {
	mAxis = axis;
	mAngularVelocity = aVel;
}

PhysicsComponent::PhysicsComponent(Bounds* bounds, glm::vec3 vel, glm::vec3 acc, glm::vec3 axis, float aVel) :PhysicsComponent() {
	if (bounds) {
		delete mBounds;
		mBounds = bounds;
	}
	mVelocity = vel;
	mAcceleration = acc;
	mAxis = axis;
	mAngularVelocity = aVel;
}

PhysicsComponent::~PhysicsComponent() {
	delete mBounds;
}

void PhysicsComponent::update(float dt) {
	mVelocity += mAcceleration * dt;
	mPosition += mVelocity * dt;
	rotate(mAxis, mAngularVelocity * dt);
	mBounds->transform(getModelMatrix());	// I suspect that's not right but w/e
}

void PhysicsComponent::translate(glm::vec3 dxyz) {
	mPosition += dxyz;
}

void PhysicsComponent::rotate(glm::vec3 axis, float degrees) {
	mRotation = glm::rotate(mRotation, glm::radians(degrees), axis);
}

void PhysicsComponent::scale(glm::vec3 scale) {
	mScale *= scale;
}

glm::mat4 PhysicsComponent::getModelMatrix() {
	glm::mat4 mm = glm::translate(glm::mat4(1.0), mPosition) * (mRotation * glm::scale(glm::mat4(1.0), mScale));
	return mm;
}

glm::mat4 PhysicsComponent::getInverseModelMatrix() {
	return glm::inverse(getModelMatrix());
}

float* PhysicsComponent::getOrientationVectors() const {
	glm::vec4 forward = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 up = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	forward = mRotation * forward;
	up = mRotation * up;
	float* r = new float[6];
	for (int i = 0; i < 3; i++)
		r[i] = forward[i];
	for (int i = 0; i < 3; i++)
		r[i + 3] = up[i];
	return r;
}