#include "GameObject.h"
#include "Camera.h"
#include "Geometry.h"
#include "InputComponent.h"
#include "Program.h"
#include "Source.h"
#include "Texture.h"

GameObject::GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics, InputComponent* input, Texture* texture, glm::vec3* color) {
	mGeometry = geometry;
	if (mGeometry) mGeometry->transfer();
	mDisplay = display;
	mPhysicsComponent = physics;
	mInputComponent = input;
	mTexture = texture;
	mColor = color;
	mSource = NULL;
}

GameObject::~GameObject() {
	if (mPhysicsComponent) delete mPhysicsComponent;
	if (mColor) delete mColor;	// Just because it's temporary doesn't mean it's allowed to leak
	if (mSource) delete mSource;
}

void GameObject::update(float dt) {
	if (mInputComponent) mInputComponent->update(this/*mState*/);
	if (mPhysicsComponent) mPhysicsComponent->update(dt);
	if (mSource) mSource->update();
}

void GameObject::registerSound(std::string name, Sound* sound) {
	if (!mSource) mSource = new Source(mPhysicsComponent, false);
	mSounds.add(name, sound);
}

void GameObject::playSound(std::string name) {
	if (auto sound = mSounds.get(name))
		mSource->playSound(sound);
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
		mDisplay->sendUniform("uCamera", 3,  glm::value_ptr(c->getPosition()));
		if (mTexture) mTexture->activate();
		if (mColor) mDisplay->sendUniform("uColor", 3, &mColor->r);	// Temporary, will figure out a more intelligent way of assigning uniforms later
	}
	if (mGeometry) mGeometry->render();
}