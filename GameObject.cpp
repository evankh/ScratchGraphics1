#include "GameObject.h"
#include "Camera.h"
#include "Geometry.h"
#include "Program.h"
#include "Source.h"
#include "State.h"
#include "Texture.h"

GameObject::GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics, Texture* texture, glm::vec3* color) {
	mGeometryComponent = geometry;
	if (mGeometryComponent) mGeometryComponent->transfer();
	mDisplayComponent = display;
	mPhysicsComponent = physics;
	mState = NULL;
	mTexture = texture;
	mColor = color;
	mAudioComponent = NULL;
}

void GameObject::setState(State* state) {
	mState = state;
	mState->enter(mPhysicsComponent);
}

GameObject::~GameObject() {
	if (mPhysicsComponent) delete mPhysicsComponent;
	if (mState) {
		mState->destroy();
		delete mState;
	}
	if (mColor) delete mColor;	// Just because it's temporary doesn't mean it's allowed to leak
	if (mAudioComponent) delete mAudioComponent;
}

void GameObject::update(float dt) {
	mHasCollision = false;
	while (!mEventQueue.isEmpty()) {
		Event e = mEventQueue.pop();
		Sound* s = NULL;
		if (e.mType == EventType::PLAY_SOUND_REQUEST && (s = mSounds.get(e.mData.sound.name))) {
			mAudioComponent->setVolume(e.mData.sound.gain);
			mAudioComponent->playSound(s);
		}
		if (mState) {
			State* next = mState->handleEvent(e);
			if (next) {
				mState->exit();
				delete mState;
				mState = next;
				mState->enter(mPhysicsComponent);
			}
		}
		if (e.mType == EventType::COLLISION)
			mHasCollision = true;
	}
	if (mState) mState->update(mPhysicsComponent, dt);
	if (mAudioComponent) mAudioComponent->update();
}

void GameObject::registerSound(std::string name, Sound* sound) {
	if (!mAudioComponent) mAudioComponent = new Source(mPhysicsComponent, false);
	mSounds.add(name, sound);
}

#include "glm\gtc\type_ptr.hpp"
// Eventually sorting Objects for rendering based on their materials would save time on switching and let me take the stupid Camera out of here
void GameObject::render(Camera* c) {
	if (mDisplayComponent) {
		mDisplayComponent->use();
		mDisplayComponent->sendUniform("uM", glm::value_ptr(mPhysicsComponent->getModelMatrix()));
		mDisplayComponent->sendUniform("uVP", glm::value_ptr(c->getViewProjectionMatrix()));
		mDisplayComponent->sendUniform("uCamera", 3, 1,  glm::value_ptr(c->getPosition()));
		if (mTexture) mTexture->activate();
		if (mColor) mDisplayComponent->sendUniform("uColor", 3, 1, &mColor->r);	// Temporary, will figure out a more intelligent way of assigning uniforms later
	}
	if (mGeometryComponent) mGeometryComponent->render();
}

void GameObject::debugDraw() {
	if (mPhysicsComponent->getBounds()) mPhysicsComponent->getBounds()->debugDraw();
	else if (mGeometryComponent) mGeometryComponent->getBoundingBox()->debugDraw();
}

void GameObject::handle(const Event e) {
	mEventQueue.push(e);
}
