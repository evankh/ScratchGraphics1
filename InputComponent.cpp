#include "InputComponent.h"
#include "KeyboardHandler.h"
#include "ServiceLocator.h"

KeyboardInputComponent::KeyboardInputComponent(int numInterested, const char* interested) {
	for (int i = 0; i < numInterested; i++)
		KeyboardHandler::registerReceiver(interested[i], this);
}

KeyboardInputComponent::~KeyboardInputComponent() {
	KeyboardHandler::unregisterReceiver(this);
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