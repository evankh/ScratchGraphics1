#include "Handler.h"

void Handler::dispatchAll() {
	// All these events will be passed along instantly, no need to worry about the timers
	while (!sEvents.isEmpty()) {
		Event event = sEvents.pop();
		ReceiverNode* current = mRegisteredReceivers[getIndexFrom(event)];
		while (current) {
			current->receiver->handle(event);
			current = current->next;
		}
	}
}

void Handler::unregisterReceiver(Receiver* receiver) {
	for (int i = 0; i < getNumReceivers(); i++) {
		if (mRegisteredReceivers[i]) {
			if (mRegisteredReceivers[i]->receiver == receiver) {
				auto temp = mRegisteredReceivers[i];
				mRegisteredReceivers[i] = temp->next;
				delete temp;
			}
			else {
				ReceiverNode* current = mRegisteredReceivers[i];
				while (current && current->next) {
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