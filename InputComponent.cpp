#include "InputComponent.h"
#include "KeyboardHandler.h"

KeyboardInputComponent::KeyboardInputComponent(int numInterested, const char* interested) {
	for (int i = 0; i < numInterested; i++)
		KeyboardHandler::registerReceiver(interested[i], this);
}

KeyboardInputComponent::~KeyboardInputComponent() {
	KeyboardHandler::unregisterReceiver(this);
}

void KeyboardInputComponent::handle(Event event) {
	mKeyboardEvents.push(event);
}
