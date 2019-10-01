#include "Input/MouseHandler.h"

MouseHandler& MouseHandler::getInstance() {
	static MouseHandler* sInstance = new MouseHandler;
	if (!sInstance->mRegisteredReceivers) {
		sInstance->mRegisteredReceivers = new ReceiverNode*[EKH_MOUSE_NUM_BUTTONS] { nullptr };
		sInstance->mMouseoverReceivers.push_back(nullptr);
	}
	return *sInstance;
}

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

void MouseHandler::registerMouseoverReceiver(Receiver* receiver) {
	if (receiver) {
		receiver->setIndex(mMouseoverReceivers.size());	// May not necessarily agree well with removing them
		mMouseoverReceivers.push_back(receiver);
	}
}

void MouseHandler::unregisterReceiver(Receiver* receiver) {
	Handler::unregisterReceiver(receiver);
	// Remove from mMouseoverReceivers too
	unsigned int i = 0;
	for (; i < mMouseoverReceivers.size(); i++)
		if (mMouseoverReceivers[i] == receiver) break;
	if (i == mMouseoverReceivers.size()) return;	// Wasn't in the list
	for (; i < mMouseoverReceivers.size() - 1; i++) {
		mMouseoverReceivers[i] = mMouseoverReceivers[i + 1];
		mMouseoverReceivers[i]->setIndex(i);
	}
	mMouseoverReceivers.pop_back();
}

void MouseHandler::handleButton(MouseButton button, int edge, int mouse_x, int mouse_y) {
	mButtonStatus[button] = (edge == 0);
	if (edge == 0) {
		mDragStartPosition[button][0] = mouse_x;
		mDragStartPosition[button][1] = mWindowHeight - mouse_y;
	}
	MouseData data;
	data.button = button;
	data.edge = edge;
	data.mouse_x = mDragStartPosition[button][0];
	data.mouse_y = mDragStartPosition[button][1];
	for (int i = 0; i < 3; i++) data.world_pos[i] = 0.0f;
	if (0 <= data.mouse_x && data.mouse_x < mWindowWidth && 0 <= data.mouse_y && data.mouse_y < mWindowHeight) {
		float worldPos[4];
		mFramebuffer->validate();
		mFramebuffer->getPixel(data.mouse_x, data.mouse_y, 1, worldPos);
		for (int i = 0; i < 3; i++) data.world_pos[i] = worldPos[i];
	}
	sEvents.push(Event(mButtonStatus[button] ? EventType::BUTTON_PRESSED : EventType::BUTTON_RELEASED, data));
	sEvents.push(Event(CommandData{ mButtonBindings[button], 0.0f }));
}

void MouseHandler::handleMove(int mouse_x, int mouse_y) {
	mMousePosition[0] = mouse_x;
	mMousePosition[1] = mWindowHeight - mouse_y;
}

const int* MouseHandler::getDragStartPosition(MouseButton button) const {
	if (mButtonStatus[button])
		return mDragStartPosition[button];
	return mMousePosition;
}

void MouseHandler::step() {
	for (int i = 0; i < EKH_MOUSE_NUM_BUTTONS; i++)
		if (mButtonStatus[i]) sEvents.push(Event(EventType::BUTTON_HELD, MouseData{ MouseButton(i),0,mMousePosition[0],mMousePosition[1] }));
}

void MouseHandler::resize(unsigned int width, unsigned int height) {
	mWindowWidth = width;
	mWindowHeight = height;
	if (mFramebuffer) mFramebuffer->resize(width, height);
}

//#include "Util/ServiceLocator.h"

void MouseHandler::dispatchAll() {
	Handler::dispatchAll();
	if (mFramebuffer && (0 <= mMousePosition[0] && mMousePosition[0] < mWindowWidth) && (0 <= mMousePosition[1] && mMousePosition[1] < mWindowHeight)) {
		mFramebuffer->validate();
		// Find the moused-over object
		// Dispatch an event to that object
		MouseoverData data;
		data.mouse_x = mMousePosition[0];
		data.mouse_y = mMousePosition[1];
		unsigned int index = 0u;
		float worldPos[4] = { 0.1f, 0.1f, 0.1f, 0.1f };
		float localPos[4] = { 0.8f, 0.8f, 0.8f, 0.8f };
		mFramebuffer->getPixel(mMousePosition[0], mMousePosition[1], 0, &index);
		mFramebuffer->getPixel(mMousePosition[0], mMousePosition[1], 1, worldPos);
		mFramebuffer->getPixel(mMousePosition[0], mMousePosition[1], 2, localPos);
		for (int i = 0; i < 3; i++) {
			data.world_pos[i] = worldPos[i];
			data.local_pos[i] = localPos[i];
		}
		if (index) mMouseoverReceivers[index]->handle(Event(data));
	}
}

void MouseHandler::init(unsigned int width, unsigned int height) {
	mFramebuffer = new Framebuffer(width, height);
	mFramebuffer->attach(ATTACHMENT_INTEGER);	// Index
	mFramebuffer->attach(ATTACHMENT_COLOR);	// World position
	mFramebuffer->attach(ATTACHMENT_COLOR);	// Local position
	mFramebuffer->attach(ATTACHMENT_DEPTH);	// Probably useful
	mFramebuffer->validate();
}

void MouseHandler::cleanup() {
	if (mFramebuffer) delete mFramebuffer;
}

void MouseHandler::setAsDrawingTarget() const {
	mFramebuffer->setAsDrawingTarget();
}

void MouseHandler::setAsTextureSource() const {
	if (mFramebuffer) mFramebuffer->setAttachmentAsTextureSource(1, 0);
}

glm::vec2 MouseHandler::getAspectCorrectedNDC() const {
	glm::vec2 result;
	result.x = (float)mMousePosition[0] / (float)mWindowHeight;
	result.y = (float)mMousePosition[1] / (float)mWindowHeight;
	return result * 2.0f - 1.0f;
	//return result;
}