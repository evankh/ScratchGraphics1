#include "Core/GameObject.h"
#include "Components/Geometry.h"
#include "Components/Graphics.h"
#include "Core/State.h"
#include "Graphics/Camera.h"
#include "Sound/Source.h"

GameObject::GameObject(GeometryComponent* geometry, GraphicsComponent* graphics, PhysicsComponent* physics) {
	mGeometryComponent = geometry;
	if (mGeometryComponent) mGeometryComponent->transfer();
	if (graphics) mGraphicsComponents.push_back(graphics);
	mPhysicsComponent = physics;
}

void GameObject::setState(State* state) {
	mState = state;
	mState->enter(mPhysicsComponent);
}

GameObject::~GameObject() {
	if (mState) {
		mState->destroy();
		delete mState;
	}
	if (mAudioComponent) delete mAudioComponent;
	for (GraphicsComponent* i : mGraphicsComponents) delete i;
	mGraphicsComponents.clear();
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

// Eventually sorting Objects for rendering based on their materials would save time on switching and let me take the stupid Camera out of here
void GameObject::render(GameObject* camera) {
	/*if (mGraphicsComponent) {
		mGraphicsComponent->activate(this, camera);
		if (mGeometryComponent)
			mGraphicsComponent->render(mGeometryComponent);
	}*/
	if (mGeometryComponent) {
		for (GraphicsComponent* i : mGraphicsComponents) {
			i->activate(this, camera);
			i->render(mGeometryComponent);
		}
	}
}

void GameObject::debugDraw() {
	if (mPhysicsComponent->getBounds()) mPhysicsComponent->getBounds()->debugDraw();
	else if (mGeometryComponent) mGeometryComponent->getBoundingBox()->debugDraw();
}

void GameObject::handle(const Event e) {
	mEventQueue.push(e);
}

GameObject* GameObject::makeCopy() const {
	GameObject* result = new GameObject(mGeometryComponent, nullptr, mPhysicsComponent);
	for (GraphicsComponent* i : mGraphicsComponents) result->addGraphicsComponent(i);
	if (mGeometryComponent) mGeometryComponent->transfer();
	copyTo(result);
	return result;
}

void GameObject::copyTo(GameObject* target) const {
	target->mGeometryComponent = mGeometryComponent;
	if (!mGraphicsComponents.empty()) {
		for (GraphicsComponent* i : mGraphicsComponents)
			target->mGraphicsComponents.push_back(i->makeCopy());
	}
	target->mPhysicsComponent = mPhysicsComponent;
	if (mAudioComponent) target->mAudioComponent = mAudioComponent->copy(target->mPhysicsComponent);
	target->mSounds = mSounds;
	if (mState) target->setState(mState->getEntry(target));
	if (mCameraComponent) target->mCameraComponent = mCameraComponent->copy();

}

void GameObject::setPhysicsComponent(PhysicsComponent* other) {
	mPhysicsComponent = other;
}