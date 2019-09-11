#ifndef __EKH_SCRATCH_GRAPHICS_1_CAMERA__
#define __EKH_SCRATCH_GRAPHICS_1_CAMERA__

#include "GameObject.h"

class Camera {
protected:
	unsigned int mWidth, mHeight;
	glm::mat4 mProjectionMatrix, mViewProjectionMatrix;
public:
	unsigned int getWidth() const { return mWidth; };
	unsigned int getHeight() const { return mHeight; };
	float getAspectRatio() const { return float(mWidth) / mHeight; };
	virtual void resize(unsigned int width, unsigned int height) = 0;
	glm::mat4& getProjectionMatrix();
	glm::mat4& getViewProjectionMatrix();
	virtual Camera* copy() const = 0;
	virtual void update(PhysicsComponent* physics) = 0;
};

class PerspCamera : public Camera {
private:
	float mZMin, mZMax, mFOV;
public:
	PerspCamera(unsigned int width, unsigned int height, float fov);
	void resize(unsigned int width, unsigned int height) final;
	Camera* copy() const final;
	void update(PhysicsComponent* physics) final;
};

class OrthoCamera : public Camera {
public:
	OrthoCamera(unsigned int width, unsigned int height);
	void resize(unsigned int width, unsigned int height) final;
	Camera* copy() const final;
	void update(PhysicsComponent* physics) final { mViewProjectionMatrix = mProjectionMatrix; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_CAMERA__