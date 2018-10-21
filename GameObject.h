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

typedef void(*InputComponent)(State*, EventQueue&);

class GameObject :public Receiver {
private:
	Geometry* mGeometry;
	Program* mDisplay;
	PhysicsComponent* mPhysicsComponent;
	Texture* mTexture;
	glm::vec3* mColor;	// Temporary
	Source* mSource;
	SoundLibrary mSounds;
	InputComponent mInputComponent;
	State* mState;
	EventQueue mEventQueue;
public:
	GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics, InputComponent input, Texture* texture = NULL, glm::vec3* color = NULL);
	~GameObject();
	void update(float dt);
	void render(Camera* c);
	virtual void handle(const Event event);
	void registerSound(std::string name, Sound* sound);
	void translate(glm::vec3 dxyz) { mPhysicsComponent->translate(dxyz); };
	void rotate(glm::vec3 axis, float degrees) { mPhysicsComponent->rotate(axis, degrees); };
	void scale(glm::vec3 scale) { mPhysicsComponent->scale(scale); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__