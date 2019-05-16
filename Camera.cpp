#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "GL/glew.h"

PerspCamera::PerspCamera(unsigned int width, unsigned int height, float fov) {
	mWidth = width;
	mHeight = height;
	mFOV = glm::radians(fov);
	mZMin = 0.01f;
	mZMax = 100.0f;
	glDepthRange(0.01f, 100.0f);
	mProjectionMatrix = glm::perspective(mFOV, getAspectRatio(), mZMin, mZMax);
}

Camera* PerspCamera::copy() const {
	PerspCamera* ret = new PerspCamera(mWidth, mHeight, glm::degrees(mFOV));
	ret->mViewProjectionMatrix = mViewProjectionMatrix;
	return ret;
}

OrthoCamera::OrthoCamera(unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;
	mProjectionMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
	mViewProjectionMatrix = mProjectionMatrix;
}

Camera* OrthoCamera::copy() const {
	return new OrthoCamera(mWidth, mHeight);
}

void PerspCamera::resize(unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;
	mProjectionMatrix = glm::perspective(mFOV, getAspectRatio(), mZMin, mZMax);
}

void OrthoCamera::resize(unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;
	mProjectionMatrix = glm::ortho((float)width, 0.0f, (float)height, 0.0f);
	mViewProjectionMatrix = mProjectionMatrix;
}

glm::mat4& Camera::getViewProjectionMatrix() {
	return mViewProjectionMatrix;
}

void PerspCamera::update(PhysicsComponent* physics) {
	mViewProjectionMatrix = mProjectionMatrix * physics->getInverseWorldTransform();
}