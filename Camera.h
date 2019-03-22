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
	virtual PhysicsComponent* getPhysics() const = 0;
	virtual Camera* copy() const = 0;
	virtual void update(float dt) = 0;
};

class PerspCamera : public Camera {
private:
	PhysicsComponent* mPhysicsComponent;
	float mZMin, mZMax, mFOV;
public:
	PerspCamera(PhysicsComponent* physics, unsigned int width, unsigned int height, float fov);
	~PerspCamera();
	void resize(unsigned int width, unsigned int height) final;
	glm::vec3 getPosition() const final { return mPhysicsComponent->getPosition(); };
	PhysicsComponent* getPhysics() const final { return mPhysicsComponent; };
	Camera* copy() const final;
	void update(float dt) final;
};

class OrthoCamera : public Camera {
public:
	OrthoCamera(unsigned int width, unsigned int height);
	void resize(unsigned int width, unsigned int height) final;
	glm::vec3 getPosition() const final { return glm::vec3(INFINITY); };
	PhysicsComponent* getPhysics() const final { return NULL; };
	Camera* copy() const final;
	void update(float dt) final {};
};

#endif//__EKH_SCRATCH_GRAPHICS_1_CAMERA__