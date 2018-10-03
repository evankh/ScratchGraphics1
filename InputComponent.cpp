#include "InputComponent.h"
#include "GameObject.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "ServiceLocator.h"

KeyboardInputComponent::KeyboardInputComponent(int numInterested, const char* interested) {
	for (int i = 0; i < numInterested; i++)
		KeyboardHandler::getInstance().registerReceiver(interested[i], this);
}

KeyboardInputComponent::~KeyboardInputComponent() {
	KeyboardHandler::getInstance().unregisterReceiver(this);
}

void KeyboardInputComponent::handle(Event event) {
	mKeyboardEvents.push(event);
	ServiceLocator::getLoggingService().log(std::string(1, event.mData.keyboard.key) + " queued");
}

void KeyboardInputComponent::update(GameObject* owner) {
	while (!mKeyboardEvents.isEmpty()) {
		Event event = mKeyboardEvents.pop();
		switch (event.mData.keyboard.key) {
		case 'a':
			owner->playSound("ascending_3");
			break;
		case 'd':
			owner->playSound("descending_3");
			break;
		case 'w':
			owner->playSound("ascending");
			break;
		case 's':
			owner->playSound("descending");
			break;
		case 'i':
		case 'j':
		case 'k':
		case 'l':
			owner->playSound("doorbell");
			break;
		}
	}
}

MouseInputComponent::MouseInputComponent(bool interested[5]) {
	MouseHandler::getInstance().registerReceiver(interested, this);
}

MouseInputComponent::~MouseInputComponent() {
	MouseHandler::getInstance().unregisterReceiver(this);
}

void MouseInputComponent::handle(Event event) {
	mMouseEvents.push(event);
	ServiceLocator::getLoggingService().log("event received in MouseInputComponent");
}

void MouseInputComponent::update(GameObject* owner) {
	while (!mMouseEvents.isEmpty()) {
		Event event = mMouseEvents.pop();
		ServiceLocator::getLoggingService().log("event consumed by MouseInputComponent");
	}
}