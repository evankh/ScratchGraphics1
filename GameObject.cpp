#include "GameObject.h"
#include "Camera.h"
#include "Geometry.h"
#include "Program.h"
#include "Source.h"
#include "State.h"
#include "Texture.h"

GameObject::GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics) {
	mGeometryComponent = geometry;
	if (mGeometryComponent) mGeometryComponent->transfer();
	mDisplayComponent = display;
	mPhysicsComponent = physics;
}

void GameObject::setState(State* state) {
	mState = state;
	mState->enter(mPhysicsComponent);
}

GameObject::~GameObject() {
	//if (mPhysicsComponent) delete mPhysicsComponent;	// GameObject is no longer responsible for deleting its own PC... in ~Game. In ~Level, it is.
	if (mState) {
		mState->destroy();
		delete mState;
	}
	if (mAudioComponent) delete mAudioComponent;

	if (mCameraComponent) delete mCameraComponent;
}

void GameObject::update(float dt) {
	mHasCollision = false;
	mHasMouseOver = false;
	int remaining = mEventQueue.size();
	while (remaining) {
		Event e = mEventQueue.pop();
		e.mTimer -= dt;
		if (e.mTimer <= 0.0f)
			process(e);
		else
			mEventQueue.push(e);
		remaining--;
	}
	if (mState) mState->update(mPhysicsComponent, dt);
	if (mCameraComponent) mCameraComponent->update(mPhysicsComponent);
	if (mAudioComponent) mAudioComponent->update();
}

void GameObject::process(const Event e) {
	Sound* s = nullptr;
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
	if (e.mType == EventType::MOUSEOVER)
		mHasMouseOver = true;
}

void GameObject::registerSound(std::string name, Sound* sound) {
	if (!mAudioComponent) mAudioComponent = new Source(mPhysicsComponent, false);
	mSounds.add(name, sound);
}

#include "glm\gtc\type_ptr.hpp"
// Eventually sorting Objects for rendering based on their materials would save time on switching and let me take the stupid Camera out of here
void GameObject::render(GameObject* camera) {
	if (mDisplayComponent) {
		mDisplayComponent->use();
		if (mPhysicsComponent) mDisplayComponent->sendUniform("uM", glm::value_ptr(mPhysicsComponent->getWorldTransform()));
		mDisplayComponent->sendUniform("uVP", glm::value_ptr(camera->getCameraComponent()->getViewProjectionMatrix()));	// If things are sorted by Program for rendering, then all the references to Camera here can be removed!
		mDisplayComponent->sendUniform("uCamera", 3, 1, glm::value_ptr(camera->getPhysicsComponent()->getGlobalPosition()));
		if (mTexture) mTexture->activate(0);	// This will make a lot more sense when DisplayComponent becomes an actual Component
		if (mHasColor) mDisplayComponent->sendUniform("uColor", 3, 1, glm::value_ptr(mColor));
	}
	if (mGeometryComponent)
		if (mDisplayComponent && mDisplayComponent->isTesselated())
			mGeometryComponent->render_patches();
		else
			mGeometryComponent->render();
}

void GameObject::render(Program* p) {
	if (mPhysicsComponent) p->sendUniform("uM", glm::value_ptr(mPhysicsComponent->getWorldTransform()));
	if (mGeometryComponent) mGeometryComponent->render();
}

void GameObject::debugDraw() {
	if (mPhysicsComponent->getBounds()) mPhysicsComponent->getBounds()->debugDraw();
	else if (mGeometryComponent) mGeometryComponent->getBoundingBox()->debugDraw();
}

void GameObject::handle(const Event e) {
	mEventQueue.push(e);
}

GameObject* GameObject::makeCopy() const {
	GameObject* result = new GameObject(mGeometryComponent, mDisplayComponent, mPhysicsComponent);
	if (mGeometryComponent) mGeometryComponent->transfer();
	copyTo(result);
	return result;
}

void GameObject::copyTo(GameObject* target) const {
	target->mGeometryComponent = mGeometryComponent;
	target->mDisplayComponent = mDisplayComponent;
	target->mPhysicsComponent = mPhysicsComponent;
	//if (mPhysicsComponent) target->mPhysicsComponent = mPhysicsComponent->makeCopy();
	target->mTexture = mTexture;
	if (mHasColor) {
		target->mHasColor = true;
		target->mColor = mColor;
	}
	if (mAudioComponent) target->mAudioComponent = mAudioComponent->copy(target->mPhysicsComponent);
	target->mSounds = mSounds;
	if (mState) target->setState(mState->getEntry(target));
	if (mCameraComponent) target->mCameraComponent = mCameraComponent->copy();

}

void GameObject::setPhysicsComponent(PhysicsComponent* other) {
	//if (mPhysicsComponent) delete mPhysicsComponent;	// GameObject is not responsible for managing it's own PC!
	mPhysicsComponent = other;
}