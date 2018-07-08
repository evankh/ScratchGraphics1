#include "KeyboardHandler.h"

bool KeyboardHandler::sKeyStatus[256];
EventQueue KeyboardHandler::sKeyboardEvents = EventQueue();
std::map<char, std::vector<Receiver*>> KeyboardHandler::mRegisteredReceivers = {};

void KeyboardHandler::handlePress(char key, int mouse_x, int mouse_y) {
	sKeyStatus[key] = true;
	KeyboardData data;
	data.key = key;
	data.mouse_x = mouse_y;
	data.mouse_y = mouse_y;
	sKeyboardEvents.push(Event(EKH_EVENT_KEY_PRESSED, &data));
}

void KeyboardHandler::handleRelease(char key, int mouse_x, int mouse_y) {
	sKeyStatus[key] = false;
	KeyboardData data;
	data.key = key;
	data.mouse_x = mouse_y;
	data.mouse_y = mouse_y;
	sKeyboardEvents.push(Event(EKH_EVENT_KEY_RELEASED, &data));
}

void KeyboardHandler::dispatchAll() {
	while (!sKeyboardEvents.isEmpty()) {
		Event event = sKeyboardEvents.pop();
		for (Receiver* receiver : mRegisteredReceivers[event.mData.keyboard.key])
			receiver->handle(event);
	}
}

void KeyboardHandler::registerReceiver(char interested, Receiver* receiver) {
	mRegisteredReceivers[interested].push_back(receiver);
}
