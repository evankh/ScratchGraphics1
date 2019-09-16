#include "Components/Physics.h"
#include "Components/Collision.h"
#include <glm/gtc/matrix_transform.hpp>

PhysicsComponent::PhysicsComponent() {
	mScale = glm::vec3(1.0f);
	mPosition = glm::vec3(0.0f);
	mVelocity = glm::vec3(0.0f);
	mAcceleration = glm::vec3(0.0f);
	mAngles = mAngularVelocities = mAngularAccelerations = glm::vec3(0.0f);
	mBounds = new Collisionless();
}

float angle(glm::vec2 val) {
	float angle = acos(val.x / glm::length(val));
	if (val.y >= 0.0)
		return angle;
	return glm::two_pi<float>() - angle;
}

PhysicsComponent::PhysicsComponent(glm::vec3 pos, glm::vec3 towards) :PhysicsComponent() {
	mPosition = pos;
	glm::vec3 diff = towards - pos;
	mAngles.x = glm::degrees(angle(glm::vec2(diff.x, diff.y)));
	mAngles.y = glm::degrees(glm::atan(diff.z / glm::length(glm::vec2(diff.x,diff.y)))) + 90.0f;
	mAngles.z = -90.0f;
}

PhysicsComponent::PhysicsComponent(glm::vec3 angles, glm::vec3 aVels, glm::vec3 aAccs) :PhysicsComponent() {
	mAngles = angles;
	mAngularVelocities = aVels;
	mAngularAccelerations = aAccs;
}

PhysicsComponent::PhysicsComponent(CollisionComponent* bounds, glm::vec3 vel, glm::vec3 acc, glm::vec3 angles, glm::vec3 aVels) :PhysicsComponent() {
	delete mBounds;
	mBounds = bounds;
	mVelocity = vel;
	mAcceleration = acc;
	mAngles = angles;
	mAngularVelocities = aVels;
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
	mAngularVelocities += mAngularAccelerations * dt;
	mAngles += mAngularVelocities * dt;
	mAngles.x = fmodf(mAngles.x, 360.0f);
	mAngles.y = fmodf(mAngles.y, 360.0f);
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

void PhysicsComponent::copyFrom(const PhysicsComponent* other) {
	mScale = other->mScale;
	mPosition = other->mPosition;
	mVelocity = other->mVelocity;
	mAcceleration = other->mAcceleration;
	mRotation = other->mRotation;
	mAngles = other->mAngles;
	mAngularVelocities = other->mAngularVelocities;
	mAngularAccelerations = other->mAngularAccelerations;
	if (mBounds) delete mBounds;
	if (other->mBounds) mBounds = other->mBounds->copy();
	else mBounds = nullptr;
}

glm::mat4 PhysicsComponent::getWorldTransform() const {
	glm::mat4 mm = glm::translate(glm::mat4(1.0), mPosition) *
		glm::rotate(
			glm::rotate(
				glm::rotate(
					glm::scale(glm::mat4(1.0), mScale),
					glm::radians(mAngles.x), glm::vec3(0.0, 0.0, 1.0)),
				glm::radians(mAngles.y), glm::vec3(0.0, -1.0, 0.0)),
			glm::radians(mAngles.z), glm::vec3(0.0, 0.0, 1.0));
	if (mParent)
		return mParent->getWorldTransform() * mm;
	return mm;
}

glm::vec3 PhysicsComponent::getGlobalPosition() const {
	glm::vec4 glob = getWorldTransform() * glm::vec4(0.0, 0.0, 0.0, 1.0);
	return glm::vec3(glob.x, glob.y, glob.z);
}

glm::mat4 PhysicsComponent::getInverseWorldTransform() const {
	return glm::inverse(getWorldTransform());
}

float* PhysicsComponent::getOrientationVectors() const {
	float* r = new float[6];
	r[0] = cosf(mAngles.x) * cosf(mAngles.z);
	r[1] = sinf(mAngles.y) * cosf(mAngles.z);
	r[2] = sinf(mAngles.z);
	r[3] = -cosf(mAngles.x) * sinf(mAngles.z);
	r[4] = -sinf(mAngles.y) * sinf(mAngles.z);
	r[5] = cosf(mAngles.z);
	return r;
}

PhysicsComponent* PhysicsComponent::makeCopy() const {
	PhysicsComponent* result = new PhysicsComponent();
	result->mScale = mScale;
	result->mPosition = mPosition;
	result->mVelocity = mVelocity;
	result->mAcceleration = mAcceleration;
	result->mRotation = mRotation;
	result->mAngles = mAngles;
	result->mAngularVelocities = mAngularVelocities;
	result->mAngularAccelerations = mAngularAccelerations;
	delete result->mBounds;
	if (mBounds) result->mBounds = mBounds->copy();
	else result->mBounds = nullptr;
	return result;
}