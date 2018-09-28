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
	if (mPhysicsComponent) delete mPhysicsComponent;
	if (mInputComponent) delete mInputComponent;
}

void GameObject::update(float dt) {
	if (mInputComponent) mInputComponent->update(/*mState*/);
	if (mPhysicsComponent) mPhysicsComponent->update(dt);
}

#include "glm\gtc\type_ptr.hpp"
// Eventually sorting Objects for rendering based on their materials would save time on switching and let me take the stupid Camera out of here
void GameObject::render(Camera* c) {
	//if (mDisplay) mDisplay->use(c->getViewProjectionMatrix(), mPhysicsComponent->getModelMatrix());
	if (mDisplay) {
		mDisplay->use();
		//glm::mat4 mvp = c->getViewProjectionMatrix() * mPhysicsComponent->getModelMatrix();
		//mDisplay->sendUniform("mvp", glm::value_ptr(mvp));
		mDisplay->sendUniform("uM", glm::value_ptr(mPhysicsComponent->getModelMatrix()));
		mDisplay->sendUniform("uVP", glm::value_ptr(c->getViewProjectionMatrix()));
	}
	if (mGeometry) mGeometry->render();
}