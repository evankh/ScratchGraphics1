#ifndef __EKH_SCRATCH_GRAPHICS_1_CAMERA__
#define __EKH_SCRATCH_GRAPHICS_1_CAMERA__

#include "GameObject.h"

class Camera {
protected:
	unsigned int mWidth, mHeight;
	glm::mat4 mProjectionMatrix;
	glm::mat4 mViewProjectionMatrix;
public:
	unsigned int getWidth() const { return mWidth; };
	unsigned int getHeight() const { return mHeight; };
	float getAspectRatio() const { return float(mWidth) / mHeight; };
	virtual void resize(unsigned int width, unsigned int height) = 0;
	glm::mat4& getViewProjectionMatrix();
	virtual glm::vec3 getPosition() const = 0;
};

class PerspCamera : public Camera {
private:
	PhysicsComponent* mPhysicsComponent;
	float mZMin, mZMax, mFOV;
public:
	PerspCamera(PhysicsComponent* physics, unsigned int width, unsigned int height, float fov);
	~PerspCamera();
	virtual void resize(unsigned int width, unsigned int height);
	glm::vec3 getPosition() const { return mPhysicsComponent->getPosition(); };
	PhysicsComponent* getPhysics() const { return mPhysicsComponent; };
};

class OrthoCamera : public Camera {
public:
	OrthoCamera(unsigned int width, unsigned int height);
	virtual void resize(unsigned int width, unsigned int height);
	glm::vec3 getPosition() const { return glm::vec3(INFINITY); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_CAMERA__