#include "KeyboardHandler.h"

KeyboardHandler& KeyboardHandler::getInstance() {
	static KeyboardHandler* sInstance = new KeyboardHandler;
	if (!sInstance->sRegisteredReceivers) sInstance->sRegisteredReceivers = new ReceiverNode*[256] {NULL};
	return *sInstance;
}

void KeyboardHandler::registerReceiver(const char* interested, Receiver* receiver) {
	for (int i = 0; interested[i] != '\0'; i++)	// Careful, will be easier than usual to register to a particular key twice
		sRegisteredReceivers[interested[i]] = new ReceiverNode{ receiver, sRegisteredReceivers[interested[i]] };
}

void KeyboardHandler::registerReceiver(char interested, Receiver* receiver) {
	// Relies entirely on outside code to not register the same receiver twice (which would probably break removal); should't be a problem, but something to keep in mind
	if (receiver)	// Dunno why you would try registering NULL as a receiver, but might as well check it
		sRegisteredReceivers[interested] = new ReceiverNode{ receiver, sRegisteredReceivers[interested] };
}

void KeyboardHandler::handlePress(char key, int mouse_x, int mouse_y) {
	sKeyStatus[key] = true;
	KeyboardData data;
	data.key = key;
	data.mouse_x = mouse_y;
	data.mouse_y = mouse_y;
	sEvents.push(Event(EventType::KEY_PRESSED, data));
}

void KeyboardHandler::handleRelease(char key, int mouse_x, int mouse_y) {
	sKeyStatus[key] = false;
	KeyboardData data;
	data.key = key;
	data.mouse_x = mouse_y;
	data.mouse_y = mouse_y;
	sEvents.push(Event(EventType::KEY_RELEASED, data));
}