#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__
#define __EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__

#include "Geometry.h"
#include "Program.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
class Camera;

class GameObject {
private:
	Geometry* mGeometry;
	Program* mDisplay;
	PhysicsComponent* mPhysicsComponent;
	InputComponent* mInputComponent;
public:
	GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics, InputComponent* input);
	~GameObject();
	void update(float dt);
	void render(Camera* c);
	void translate(glm::vec3 dxyz) { mPhysicsComponent->translate(dxyz); };
	void rotate(glm::vec3 axis, float degrees) { mPhysicsComponent->rotate(axis, degrees); };
	void scale(glm::vec3 scale) { mPhysicsComponent->scale(scale); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME_OBJECT__