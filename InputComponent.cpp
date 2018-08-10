#include "InputComponent.h"
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
	ServiceLocator::getLoggingService().error("event received", &event.mData.keyboard.key);
}

void KeyboardInputComponent::update() {
	while (!mKeyboardEvents.isEmpty()) {
		Event event = mKeyboardEvents.pop();
		ServiceLocator::getLoggingService().error("update handling event", &event.mData.keyboard.key);
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

void MouseInputComponent::update() {
	while (!mMouseEvents.isEmpty()) {
		Event event = mMouseEvents.pop();
		ServiceLocator::getLoggingService().log("event consumed by MouseInputComponent");
	}
}