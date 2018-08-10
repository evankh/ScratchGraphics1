#include "Handler.h"

void Handler::dispatchAll() {
	while (!sEvents.isEmpty()) {
		Event event = sEvents.pop();
		ReceiverNode* current = sRegisteredReceivers[getIndexFrom(event)];
		while (current) {
			current->receiver->handle(event);
			current = current->next;
		}
	}
}

void Handler::unregisterReceiver(Receiver* receiver) {
	for (int i = 0; i < getNumReceivers(); i++) {
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