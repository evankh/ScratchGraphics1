#include "KeyboardHandler.h"

KeyboardHandler& KeyboardHandler::getInstance() {
	static KeyboardHandler* sInstance = new KeyboardHandler;
	if (!sInstance->mRegisteredReceivers) sInstance->mRegisteredReceivers = new ReceiverNode*[256] {NULL};
	return *sInstance;
}

void KeyboardHandler::registerReceiver(const char* interested, Receiver* receiver) {
	for (int i = 0; interested[i] != '\0'; i++)	// Careful, will be easier than usual to register to a particular key twice
		mRegisteredReceivers[interested[i]] = new ReceiverNode{ receiver, mRegisteredReceivers[interested[i]] };
}

void KeyboardHandler::registerReceiver(char interested, Receiver* receiver) {
	// Relies entirely on outside code to not register the same receiver twice (which would probably break removal); should't be a problem, but something to keep in mind
	if (receiver)	// Dunno why you would try registering NULL as a receiver, but might as well check it
		mRegisteredReceivers[interested] = new ReceiverNode{ receiver, mRegisteredReceivers[interested] };
}

void KeyboardHandler::handlePress(char key, int mouse_x, int mouse_y) {
	mKeyStatus[key] = true;
	sEvents.push(Event(EventType::KEY_PRESSED, KeyboardData{ key, mouse_x,mouse_y }));
	sEvents.push(Event(CommandData{ mKeyBindings[key], 1.0f }));	// No reason not to do both, right?
}

void KeyboardHandler::handleRelease(char key, int mouse_x, int mouse_y) {
	mKeyStatus[key] = false;
	sEvents.push(Event(EventType::KEY_RELEASED, KeyboardData{ key,mouse_x,mouse_y }));
}