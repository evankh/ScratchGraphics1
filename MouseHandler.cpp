#include "MouseHandler.h"

MouseHandler& MouseHandler::getInstance() {
	static MouseHandler* sInstance = new MouseHandler;
	if (!sInstance->mRegisteredReceivers) sInstance->mRegisteredReceivers = new ReceiverNode*[EKH_MOUSE_NUM_BUTTONS] {NULL};
	return *sInstance;
}

// It thoroughly irks me to have to rewrite all of this code instead of using inheritance, but since everything has to be static and static doesn't play nice with inheritance... I really did try.
void MouseHandler::registerReceiver(bool interested[EKH_MOUSE_NUM_BUTTONS], Receiver* receiver) {
	// Relies entirely on outside code to not register the same receiver twice (which would probably break removal); should't be a problem, but something to keep in mind
	if (receiver)	// Dunno why you would try registering NULL as a receiver, but might as well check it
		for (int i = 0; i < EKH_MOUSE_NUM_BUTTONS; i++)
			if (interested[i])
				mRegisteredReceivers[i] = new ReceiverNode{ receiver, mRegisteredReceivers[i] };
}

void MouseHandler::registerReceiver(MouseButton button, Receiver* receiver) {
	if (receiver)
		mRegisteredReceivers[button] = new ReceiverNode{ receiver, mRegisteredReceivers[button] };
}

void MouseHandler::handleButton(MouseButton button, int edge, int mouse_x, int mouse_y) {
	mButtonStatus[button] = (edge == 0);
	MouseData data;
	data.button = button;
	data.edge = edge;
	data.mouse_x = mouse_x;
	data.mouse_y = mouse_y;
	sEvents.push(Event(mButtonStatus[button] ? EventType::BUTTON_PRESSED : EventType::BUTTON_RELEASED, data));
	sEvents.push(Event(mButtonBindings[button]));
}

void MouseHandler::handleMove(int mouse_x, int mouse_y) {
	mMousePosition[0] = mouse_x;
	mMousePosition[1] = mouse_y;
}