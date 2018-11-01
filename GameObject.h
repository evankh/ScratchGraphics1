#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__
#define __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__

#include "Event System/EventQueue.h"
#include "Event System/Receiver.h"
class Geometry;
class Program;
#include "PhysicsComponent.h"
class Camera;
class State;
class Texture;
#include "SoundLibrary.h"
class Source;

//typedef State*(*InputComponent)(State*, EventQueue&);

class GameObject :public Receiver {
private:
	const Geometry* mGeometryComponent;
	Program* mDisplayComponent;
	PhysicsComponent* mPhysicsComponent;
	Texture* mTexture;	// Doesn't make a ton of sense here
	glm::vec3* mColor;	// Temporary
	Source* mAudioComponent;
	SoundLibrary mSounds;
	//InputComponent mInputComponent;
	State* mState;
	EventQueue mEventQueue;
	bool mHasCollision = false;
public:
	GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics, Texture* texture = NULL, glm::vec3* color = NULL);
	~GameObject();
	void update(float dt);
	void render(Camera* c);
	virtual void handle(const Event e);
	void setState(State* state);	// Probably should be private or something, could completely change the type of the object if given the wrong type of State
	PhysicsComponent* getPhysicsComponent() const { return mPhysicsComponent; };
	Bounds* getBounds()const { return mPhysicsComponent->getBounds(); };
	void registerSound(std::string name, Sound* sound);
	void translate(glm::vec3 dxyz) { mPhysicsComponent->translate(dxyz); };
	void rotate(glm::vec3 axis, float degrees) { mPhysicsComponent->rotate(axis, degrees); };
	void scale(glm::vec3 scale) { mPhysicsComponent->scale(scale); };
	bool hasCollision() { return mHasCollision; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__