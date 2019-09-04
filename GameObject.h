#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__
#define __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__

#include "Event System/Receiver.h"
#include "Bounds.h"
class Geometry;
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
	Texture* mTexture = nullptr;
	glm::vec3 mColor;
	bool mHasColor = false;

	Camera* mCameraComponent = nullptr;	// This will be moved somewhere else (probably GraphicsComponent?) later but for now I just want it to work

	PhysicsComponent* mPhysicsComponent;
	Source* mAudioComponent = nullptr;
	SoundLibrary mSounds;
	State* mState = nullptr;
	EventQueue mEventQueue;

	bool mHasCollision = false;
	bool mHasMouseOver = false;
public:
	GameObject(Geometry* geometry = nullptr, Program* display = nullptr, PhysicsComponent* physics = nullptr);
	~GameObject();

	// GameObject-y functions
	void update(float dt);
	void render(GameObject* camera);
	void render(Program* p);
	void debugDraw();
	virtual void handle(const Event e);
	void process(const Event e);
	void setState(State* state);	// Probably should be private or something, could completely change the type of the object if given the wrong type of State
	GameObject* copy() const;
	void copy(GameObject* target) const;

	// Graphics functions
	void setTexture(Texture* tex) { mTexture = tex; };
	void setColor(glm::vec3 col) { mHasColor = true; mColor = col; };
	
	// Sound functions
	void registerSound(std::string name, Sound* sound);

	// Physics functions
	PhysicsComponent* getPhysicsComponent() const { return mPhysicsComponent; };
	Bounds* getBounds() const { if (mPhysicsComponent) return mPhysicsComponent->getBounds(); return nullptr; };
	glm::vec3 getPosition() const { if (mPhysicsComponent) return mPhysicsComponent->getPosition(); return { 0.0f,0.0f,0.0f }; };
	void translate(glm::vec3 dxyz) { if (mPhysicsComponent) mPhysicsComponent->translate(dxyz); };
	void rotate(glm::vec3 axis, float degrees) { if (mPhysicsComponent) mPhysicsComponent->rotate(axis, degrees); };
	void scale(glm::vec3 scale) { if (mPhysicsComponent) mPhysicsComponent->scale(scale); };

	// Debug functions
	bool hasCollision() const { return mHasCollision; };
	bool hasMouseOver() const { return mHasMouseOver; };

	// Camera functions
	void setCameraComponent(Camera* cam) { mCameraComponent = cam; };
	Camera* getCameraComponent() const { return mCameraComponent; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__