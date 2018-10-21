#include "GameObject.h"
#include "Camera.h"
#include "Geometry.h"
#include "Program.h"
#include "Source.h"
#include "State.h"
#include "Texture.h"

<<<<<<< HEAD
GameObject::GameObject(Geometry* geometry, Program* display, PhysicsComponent* physics, InputComponent input, Texture* texture, glm::vec3* color) {
=======
GameObject::GameObject(const Geometry* geometry, Program* display, PhysicsComponent* physics, InputComponent* input, Texture* texture, glm::vec3* color) {
>>>>>>> remotes/ScratchGraphics1/master
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
	// This sort of thing will get moved to State
	while (!mEventQueue.isEmpty()) {
		Event e = mEventQueue.pop();
		if (e.mType == EKH_EVENT_KEY_PRESSED && e.mData.keyboard.key == 'W') {
			translate({ 0.0,0.1,0.0 });
		}
		Sound* s = NULL;
		if (e.mType == EKH_EVENT_PLAY_SOUND_REQUEST && (s = mSounds.get(e.mData.sound.name))) {
			mSource->setVolume(e.mData.sound.gain);
			mSource->playSound(s);
		}
	}
	// vvv State transitions happen here vvv
	if (mInputComponent) mInputComponent(mState, mEventQueue);	// mState will change itself and its update function depending on what events it receives
	// vvv Wait, what does this even do since PhysicsComponent is still there? vvv
	if (mState) mState->update(dt);
	if (mPhysicsComponent) mPhysicsComponent->update(dt);
	if (mSource) mSource->update();
}

void GameObject::registerSound(std::string name, Sound* sound) {
	if (!mSource) mSource = new Source(mPhysicsComponent, false);
	mSounds.add(name, sound);
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