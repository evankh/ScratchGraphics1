#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__
#define __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__

#include "Event System/EventQueue.h"
#include "Event System/Receiver.h"
#include "Geometry.h"
#include "Bounds.h"
class Program;
#include "PhysicsComponent.h"
class Camera;
class State;
class Texture;
#include "SoundLibrary.h"
class Source;

class GameObject :public Receiver {
private:
	// GraphicsComponent* mGraphicsComponent;
	Geometry* mGeometryComponent;
	Program* mDisplayComponent;
	Texture* mTexture;

	Camera* mCameraComponent = NULL;	// This will be moved somewhere else (probably GraphicsComponent?) later but for now I just want it to work

	PhysicsComponent* mPhysicsComponent;
	Source* mAudioComponent;
	SoundLibrary mSounds;
	State* mState;
	EventQueue mEventQueue;
	bool mHasCollision = false;
public:
	GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics, Texture* texture = NULL);
	~GameObject();
	void update(float dt);
	void render(Camera* c);
	void debugDraw();
	virtual void handle(const Event e);
	void setState(State* state);	// Probably should be private or something, could completely change the type of the object if given the wrong type of State
	PhysicsComponent* getPhysicsComponent() const { return mPhysicsComponent; };
	Bounds* getBounds() const { return mPhysicsComponent->getBounds(); };
	void registerSound(std::string name, Sound* sound);
	void translate(glm::vec3 dxyz) { mPhysicsComponent->translate(dxyz); };
	void rotate(glm::vec3 axis, float degrees) { mPhysicsComponent->rotate(axis, degrees); };
	void scale(glm::vec3 scale) { mPhysicsComponent->scale(scale); };
	bool hasCollision() { return mHasCollision; };
	GameObject* copy() const;

	void setCameraComponent(Camera* cam) { mCameraComponent = cam; };
	Camera* getCameraComponent() const { return mCameraComponent; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__