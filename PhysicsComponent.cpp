#include "PhysicsComponent.h"
#include "Bounds.h"
#include <glm\gtc\matrix_transform.hpp>

PhysicsComponent::PhysicsComponent() {
	mScale = glm::vec3(1.0f);
	mPosition = glm::vec3(0.0f);
	mVelocity = glm::vec3(0.0f);
	mAcceleration = glm::vec3(0.0f);
	//mRotation = glm::mat4(1.0f);
	//mAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	mAngles = mAngularVelocities = mAngularAccelerations = glm::vec3(0.0f);
	mBounds = new Collisionless();
}

PhysicsComponent::PhysicsComponent(glm::vec3 pos, glm::vec3 towards) :PhysicsComponent() {
	mPosition = pos;
	// That's not the rotation at all, it's the inverse model matrix
	//mRotation = glm::inverse(glm::lookAt(glm::vec3(0.0), towards - pos, glm::vec3(0.0f, 0.0f, 1.0f)));
	mAngles.x = glm::degrees(glm::atan((towards.y - pos.y) / (towards.x - pos.x)));
	mAngles.y = glm::degrees(glm::asin((towards.z - pos.z) / glm::length(towards - pos)));
	mAngles.z = 90.0f;
}

PhysicsComponent::PhysicsComponent(glm::vec3 axis, float aVel) :PhysicsComponent() {
	/*mAxis = axis;
	mAngularVelocity = aVel;*/

}

PhysicsComponent::PhysicsComponent(Bounds* bounds, glm::vec3 vel, glm::vec3 acc, glm::vec3 axis, float aVel) :PhysicsComponent() {
	/*if (bounds) {
		mBounds = bounds->copy();
	}*/
	mBounds = bounds;
	mVelocity = vel;
	mAcceleration = acc;
	/*mAxis = axis;
	mAngularVelocity = aVel;*/
	// Euler angles?
}

PhysicsComponent::~PhysicsComponent() {
	if (mBounds) delete mBounds;
}

float clamp(float f, float min, float max) {
	return fminf(fmaxf(f, min), max);
}

void PhysicsComponent::update(float dt) {
	mVelocity += mAcceleration * dt;
	mPosition += mVelocity * dt;
	/*mAngularVelocity += mAngularAcceleration * dt;
	mAngle += mAngularVelocity * dt;*/
	//mAngle = fmodf(mAngle, glm::pi<float>());
	mAngularVelocities += mAngularAccelerations * dt;
	mAngles += mAngularVelocities * dt;
	//mAngle = fmodf(mAngle, 360.0f);
	mAngles.x = fmodf(mAngles.x, 360.0f);
	mAngles.y = clamp(mAngles.y, -90.0f, 90.0f);
	mAngles.z = fmodf(mAngles.z, 360.0f);
	if (mBounds) mBounds->translate(mVelocity * dt);
	//if (mBounds) mBounds->rotate(mAxis, mAngularVelocity * dt);
}

void PhysicsComponent::translate(glm::vec3 dxyz) {
	mPosition += dxyz;
	if (mBounds) mBounds->translate(dxyz);
}

void PhysicsComponent::rotate(glm::vec3 axis, float degrees) {
	mRotation = glm::rotate(mRotation, glm::radians(degrees), axis);
	if (mBounds) mBounds->rotate(axis, degrees);
}

void PhysicsComponent::rotateAzimuth(float degrees) {
	mAngles.x += degrees;
}

void PhysicsComponent::rotateAltitude(float degrees) {
	mAngles.y += degrees;
	mAngles.y = clamp(mAngles.y, -90.0f, 90.0f);
}

void PhysicsComponent::rotateOrientation(float degrees) {
	mAngles.z += degrees;
}

void PhysicsComponent::scale(glm::vec3 scale) {
	mScale *= scale;
	if (mBounds) mBounds->scale(scale);
}

void PhysicsComponent::set(const PhysicsComponent* other) {
	mScale = other->mScale;
	mPosition = other->mPosition;
	mVelocity = other->mVelocity;
	mAcceleration = other->mAcceleration;
	mRotation = other->mRotation;
	/*mAxis = other->mAxis;
	mAngularVelocity = other->mAngularVelocity;
	mAngle = other->mAngle;*/
	mAngles = other->mAngles;
	mAngularVelocities = other->mAngularVelocities;
	mAngularAccelerations = other->mAngularAccelerations;
	if (mBounds) delete mBounds;
	if (other->mBounds) mBounds = other->mBounds->copy();
	else mBounds = nullptr;
}

glm::mat4 PhysicsComponent::getWorldTransform() {
	// TODO : dirty flag, caching
	//glm::mat4 mm = glm::translate(glm::mat4(1.0), mPosition) * (mRotation * glm::scale(glm::mat4(1.0), mScale));
	glm::mat4 mm = glm::translate(glm::mat4(1.0), mPosition) *
		glm::rotate(
			glm::rotate(
				glm::rotate(
					glm::scale(glm::mat4(1.0), mScale),
					glm::radians(mAngles.x), glm::vec3(0.0, 0.0, 1.0)),
				glm::radians(mAngles.y), glm::vec3(0.0, 1.0, 0.0)),
			glm::radians(mAngles.z), glm::vec3(0.0, 0.0, 1.0));
	if (mParent)
		return mParent->getWorldTransform() * mm;
	return mm;
}

glm::mat4 PhysicsComponent::getInverseWorldTransform() {
	return glm::inverse(getWorldTransform());
}

float* PhysicsComponent::getOrientationVectors() const {
	/*glm::vec4 forward = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	glm::vec4 up = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	forward = mRotation * forward;
	up = mRotation * up;
	float* r = new float[6];
	for (int i = 0; i < 3; i++)
		r[i] = forward[i];
	for (int i = 0; i < 3; i++)
		r[i + 3] = up[i];*/
	float* r = new float[6];
	r[0] = cosf(mAngles.x) * cosf(mAngles.z);
	r[1] = sinf(mAngles.y) * cosf(mAngles.z);
	r[2] = sinf(mAngles.z);
	r[3] = -cosf(mAngles.x) * sinf(mAngles.z);
	r[4] = -sinf(mAngles.y) * sinf(mAngles.z);
	r[5] = cosf(mAngles.z);
	return r;
}

PhysicsComponent* PhysicsComponent::copy() const {
	PhysicsComponent* result = new PhysicsComponent();
	result->mScale = mScale;
	result->mPosition = mPosition;
	result->mVelocity = mVelocity;
	result->mAcceleration = mAcceleration;
	result->mRotation = mRotation;
	/*result->mAxis = mAxis;
	result->mAngularVelocity = mAngularVelocity;
	result->mAngle = mAngle;*/
	result->mAngles = mAngles;
	result->mAngularVelocities = mAngularVelocities;
	result->mAngularAccelerations = mAngularAccelerations;
	delete result->mBounds;
	if (mBounds) result->mBounds = mBounds->copy();
	else result->mBounds = nullptr;
	return result;
}