#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__
#define __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__

class Geometry;
class Program;
class InputComponent;
#include "PhysicsComponent.h"
class Camera;
class Texture;

class GameObject {
private:
	Geometry* mGeometry;
	Program* mDisplay;
	PhysicsComponent* mPhysicsComponent;
	InputComponent* mInputComponent;
	Texture* mTexture;
public:
	GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics, InputComponent* input, Texture* texture = NULL);
	~GameObject();
	void update(float dt);
	void render(Camera* c);
	void translate(glm::vec3 dxyz) { mPhysicsComponent->translate(dxyz); };
	void rotate(glm::vec3 axis, float degrees) { mPhysicsComponent->rotate(axis, degrees); };
	void scale(glm::vec3 scale) { mPhysicsComponent->scale(scale); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__