#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__
#define __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__

class Geometry;
class Program;
class InputComponent;
#include "PhysicsComponent.h"
class Camera;
class Texture;
#include "SoundLibrary.h"
class Source;

class GameObject {
private:
	const Geometry* mGeometry;
	Program* mDisplay;
	PhysicsComponent* mPhysicsComponent;
	InputComponent* mInputComponent;
	Texture* mTexture;
	glm::vec3* mColor;	// Temporary
	Source* mSource;
	SoundLibrary mSounds;
public:
	GameObject(const Geometry* geometry, Program* display, PhysicsComponent* physics, InputComponent* input, Texture* texture = NULL, glm::vec3* color = NULL);
	~GameObject();
	void update(float dt);
	void render(Camera* c);
	void registerSound(std::string name, Sound* sound);
	void playSound(std::string name);
	void translate(glm::vec3 dxyz) { mPhysicsComponent->translate(dxyz); };
	void rotate(glm::vec3 axis, float degrees) { mPhysicsComponent->rotate(axis, degrees); };
	void scale(glm::vec3 scale) { mPhysicsComponent->scale(scale); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__