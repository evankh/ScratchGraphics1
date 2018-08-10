#include "KeyboardHandler.h"

bool KeyboardHandler::sKeyStatus[256];
EventQueue KeyboardHandler::sEvents = EventQueue();
ReceiverNode* KeyboardHandler::sRegisteredReceivers[256];

void KeyboardHandler::dispatchAll() {
	while (!sEvents.isEmpty()) {
		Event event = sEvents.pop();
		ReceiverNode* current = sRegisteredReceivers[event.mData.keyboard.key];
		while (current) {
			current->receiver->handle(event);
			current = current->next;
		}
	}
}

void KeyboardHandler::registerReceiver(char interested, Receiver* receiver) {
	// Relies entirely on outside code to not register the same receiver twice (which would probably break removal); should't be a problem, but something to keep in mind
	if (receiver)	// Dunno why you would try registering NULL as a receiver, but might as well check it
		sRegisteredReceivers[interested] = new ReceiverNode{ receiver, sRegisteredReceivers[interested] };
}

void KeyboardHandler::unregisterReceiver(Receiver* receiver) {
	for (int i = 0; i < 256; i++) {
		if (sRegisteredReceivers[i]) {
			if (sRegisteredReceivers[i]->receiver == receiver) {
				auto temp = sRegisteredReceivers[i];
				sRegisteredReceivers[i] = temp->next;
				delete temp;
			}
			else {
				ReceiverNode* current = sRegisteredReceivers[i];
				while (current->next) {
					if (current->next->receiver == receiver) {
						auto temp = current->next;
						current->next = temp->next;
						delete temp;
					}
					current = current->next;
				}
			}
		}
	}
}

void KeyboardHandler::handlePress(char key, int mouse_x, int mouse_y) {
	sKeyStatus[key] = true;
	KeyboardData data;
	data.key = key;
	data.mouse_x = mouse_y;
	data.mouse_y = mouse_y;
	sEvents.push(Event(EKH_EVENT_KEY_PRESSED, &data));
}

void KeyboardHandler::handleRelease(char key, int mouse_x, int mouse_y) {
	sKeyStatus[key] = false;
	KeyboardData data;
	data.key = key;
	data.mouse_x = mouse_y;
	data.mouse_y = mouse_y;
	sEvents.push(Event(EKH_EVENT_KEY_RELEASED, &data));
}