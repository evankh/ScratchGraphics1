#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__
#define __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__

#include "Event System/Receiver.h"
#include "Components/Collision.h"
class GeometryComponent;
class GraphicsComponent;
#include "Components/Physics.h"
class Camera;
class State;
#include "Util/SoundLibrary.h"
class Source;

class GameObject :public Receiver {
private:
	std::vector<GraphicsComponent*> mGraphicsComponents;
	GeometryComponent* mGeometryComponent;
	PhysicsComponent* mPhysicsComponent;

	Camera* mCameraComponent = nullptr;	// This will be moved somewhere else (probably GraphicsComponent?) later but for now I just want it to work

	Source* mAudioComponent = nullptr;
	SoundLibrary mSounds;
	State* mState = nullptr;
	EventQueue mEventQueue;

	bool mHasCollision = false;
	bool mHasMouseOver = false;
public:
	GameObject(GeometryComponent* geometry = nullptr, GraphicsComponent* graphics = nullptr, PhysicsComponent* physics = nullptr);
	~GameObject();

	// GameObject-y functions
	void update(float dt);
	void render(GameObject* camera);
	void debugDraw();
	virtual void handle(const Event e);
	void process(const Event e);
	void setState(State* state);	// Probably should be private or something, could completely change the type of the object if given the wrong type of State
	GameObject* makeCopy() const;
	void copyTo(GameObject* target) const;
	
	// Sound functions
	void registerSound(std::string name, Sound* sound);

	// Physics functions
	PhysicsComponent* getPhysicsComponent() const { return mPhysicsComponent; };
	void setPhysicsComponent(PhysicsComponent* other);
	CollisionComponent* getBounds() const { if (mPhysicsComponent) return mPhysicsComponent->getBounds(); return nullptr; };
	//glm::vec3 getPosition() const { if (mPhysicsComponent) return mPhysicsComponent->getGlobalPosition(); return { 0.0f,0.0f,0.0f }; };
	void translate(glm::vec3 dxyz) { if (mPhysicsComponent) mPhysicsComponent->translate(dxyz); };
	void rotate(glm::vec3 axis, float degrees) { if (mPhysicsComponent) mPhysicsComponent->rotate(axis, degrees); };
	void scale(glm::vec3 scale) { if (mPhysicsComponent) mPhysicsComponent->scale(scale); };

	// Debug functions
	bool hasCollision() const { return mHasCollision; };
	bool hasMouseOver() const { return mHasMouseOver; };

	// Camera functions
	void setCameraComponent(Camera* cam) { mCameraComponent = cam; };
	Camera* getCameraComponent() const { return mCameraComponent; };

	GeometryComponent* getGeometryComponent() const { return mGeometryComponent; };
	void addGraphicsComponent(GraphicsComponent* graphics) { mGraphicsComponents.push_back(graphics); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__