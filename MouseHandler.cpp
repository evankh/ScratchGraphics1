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

void MouseHandler::registerMouseoverReceiver(Receiver* receiver) {
	if (receiver) {
		receiver->setIndex(mMouseoverReceivers.size());	// May not necessarily agree well with removing them
		mMouseoverReceivers.push_back(receiver);
	}
}

void MouseHandler::unregisterReceiver(Receiver* receiver) {
	Handler::unregisterReceiver(receiver);
	// Remove from mMouseoverReceivers too
	int i = 0;
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
		mDragStartPosition[button][1] = mouse_y;
	}
	sEvents.push(Event(mButtonStatus[button] ? EventType::BUTTON_PRESSED : EventType::BUTTON_RELEASED, MouseData{ button,edge,mouse_x,mouse_y }));
	sEvents.push(Event(CommandData{ mButtonBindings[button], 0.0f }));
}

void MouseHandler::handleMove(int mouse_x, int mouse_y) {
	mMousePosition[0] = mouse_x;
	mMousePosition[1] = mouse_y;
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
	if (width != mWindowSize[0] || height != mWindowSize[1]) {
		cleanup();
		init(width, height);
	}
}

#include "GL/glew.h"

void MouseHandler::dispatchAll() {
	Handler::dispatchAll();
	if (mReadyToRead) {
		// Sample textures
		unsigned int index = 0;
		float world_pos[4]{ 0.0f,0.0f,0.0f,0.0f };
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebufferHandle);
		// glBindFramebuffer sometimes (but not always) gives a GL_INVALID_OPERATION error even though it's obviously a valid framebuffer (and glIsFramebuffer confirms it).
		// This appears to be something of a known issue? (https://devtalk.nvidia.com/default/topic/1026791/glbindframebuffer-with-a-valid-framebuffer-gives-gl_invalid_operation/)
		int error = glGetError();
		glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(mMousePosition[0], mMousePosition[1], 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &index);
		error = glGetError();
		if (index) {
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glReadnPixels(mMousePosition[0], mMousePosition[1], 1, 1, GL_RGBA, GL_FLOAT, 4 * sizeof(float), world_pos);
			error = glGetError();
			// Notify the Receiver
			MouseoverData eventdata{ mMousePosition[0], mMousePosition[1], world_pos[0], world_pos[1], world_pos[2] };
			mMouseoverReceivers[index]->handle(eventdata);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

#include "ServiceLocator.h"

void MouseHandler::init(unsigned int width, unsigned int height) {
	mWindowSize[0] = width;
	mWindowSize[1] = height;
	glGenFramebuffers(1, &mFramebufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferHandle);
	int error = glGetError();
	glGenTextures(3, mTextureHandles);
	GLenum attachments[]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_DEPTH_ATTACHMENT };
	GLint sizes[]{ GL_R32UI, GL_RGBA32F, GL_DEPTH_COMPONENT };
	GLint formats[]{ GL_RED_INTEGER, GL_RGBA, GL_DEPTH_COMPONENT };
	GLenum types[]{ GL_UNSIGNED_INT, GL_FLOAT, GL_FLOAT };
	for (int i = 0; i < 3; i++) {
		glBindTexture(GL_TEXTURE_2D, mTextureHandles[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, sizes[i], width, height, 0, formats[i], types[i], NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, mTextureHandles[i], 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDrawBuffers(2, attachments);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// Abort
		// Danger Will Robinson
		switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		default:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", std::to_string((int)glCheckFramebufferStatus(GL_FRAMEBUFFER)));
			break;
		}
		throw (std::runtime_error("Mouse framebuffer is incomplete"));
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MouseHandler::cleanup() {
	glDeleteFramebuffers(1, &mFramebufferHandle);
	mFramebufferHandle = 0;
	glDeleteTextures(3, mTextureHandles);
	mTextureHandles[0] = 0;
	mTextureHandles[1] = 0;
	mTextureHandles[2] = 0;
}

void MouseHandler::enableDrawing() const {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebufferHandle);
	int error = glGetError();
	unsigned int clearColor[4]{ 0,0,0,0 };
	glViewport(0, 0, mWindowSize[0], mWindowSize[1]);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClearBufferuiv(GL_COLOR, 0, clearColor);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	mReadyToRead = true;
}

void MouseHandler::draw() const {
	for (int i = 0; i < 3; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mTextureHandles[i]);
	}
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, mTextureHandles[1]);
	//glActiveTexture(GL_TEXTURE0);
}