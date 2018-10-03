#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"

PerspCamera::PerspCamera(PhysicsComponent* physics, unsigned int width, unsigned int height, float fov) {
	mPhysicsComponent = physics;
	mWidth = width;
	mHeight = height;
	mFOV = glm::radians(fov);
	mZMin = 0.01f;
	mZMax = 100.0f;
	mProjectionMatrix = glm::perspective(mFOV, getAspectRatio(), mZMin, mZMax);
	//mProjectionMatrix = glm::translate(mProjectionMatrix, glm::vec3(3.0, 0.0, 0.0));
	glm::mat4 imm = mPhysicsComponent->getInverseModelMatrix();
	mViewProjectionMatrix = mProjectionMatrix * imm;
}

OrthoCamera::OrthoCamera(unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;
	mProjectionMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
	mViewProjectionMatrix = mProjectionMatrix*glm::inverse(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 1.0f)));	// I think?
}

PerspCamera::~PerspCamera() {
	if (mPhysicsComponent) delete mPhysicsComponent;
}

void PerspCamera::resize(unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;
	mProjectionMatrix = glm::perspective(mFOV, getAspectRatio(), mZMin, mZMax);
	mProjectionMatrix = glm::translate(mProjectionMatrix, glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 imm = mPhysicsComponent->getInverseModelMatrix();
	mViewProjectionMatrix = mProjectionMatrix * imm;
}

void OrthoCamera::resize(unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;
	mProjectionMatrix = glm::ortho((float)width, 0.0f, (float)height, 0.0f);
	mViewProjectionMatrix = mProjectionMatrix;
}

glm::mat4 & Camera::getViewProjectionMatrix() {
	return mViewProjectionMatrix;
}