#include "GameObject.h"
#include "Camera.h"

GameObject::GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics, InputComponent* input) {
	mGeometry = geometry;
	if (mGeometry) mGeometry->transfer();
	mDisplay = display;
	mPhysicsComponent = physics;
	mInputComponent = input;
}

GameObject::~GameObject() {
	if (mGeometry) delete mGeometry;
	if (mDisplay) delete mDisplay;
	if (mPhysicsComponent) delete mPhysicsComponent;
	if (mInputComponent) delete mInputComponent;
}

void GameObject::update(float dt) {
	if (mPhysicsComponent) mPhysicsComponent->update(dt);
}

#include "glm\gtc\type_ptr.hpp"
void GameObject::render(Camera* c) {
	//if (mDisplay) mDisplay->use(c->getViewProjectionMatrix(), mPhysicsComponent->getModelMatrix());
	if (mDisplay) {
		mDisplay->use();
		glm::mat4 mvp = c->getViewProjectionMatrix() * mPhysicsComponent->getModelMatrix();
		mDisplay->sendUniform("mvp", glm::value_ptr(mvp));
	}
	if (mGeometry) mGeometry->render();
}