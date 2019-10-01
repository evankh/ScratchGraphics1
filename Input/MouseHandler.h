#ifndef __EKH_SCRATCH_GRPAHICS_1_MOUSE_HANDLER__
#define __EKH_SCRATCH_GRAPHICS_1_MOUSE_HANDLER__

#include "Graphics/FrameBuffer.h"
#include "Input/Handler.h"
#include <vector>
#include "glm/glm/vec2.hpp"

enum MouseButton {
	EKH_MOUSE_BUTTON_LEFT = 0,
	EKH_MOUSE_BUTTON_MIDDLE,
	EKH_MOUSE_BUTTON_RIGHT,
	EKH_MOUSE_SCROLL_UP,
	EKH_MOUSE_SCROLL_DOWN,

	EKH_MOUSE_NUM_BUTTONS
};

class MouseHandler :public Handler {
	friend void game_mouse_wrapper(int, int, int, int);
	friend void game_movement_wrapper(int, int);
private:
	bool mButtonStatus[EKH_MOUSE_NUM_BUTTONS];
	int mDragStartPosition[EKH_MOUSE_NUM_BUTTONS][2];	// It may be a better idea to leave tracking this up to whatever is interested in it
	Command mButtonBindings[EKH_MOUSE_NUM_BUTTONS];
	unsigned int mWindowWidth, mWindowHeight;
	int mMousePosition[2];
	std::vector<Receiver*> mMouseoverReceivers;
	Framebuffer* mFramebuffer = nullptr;

	void handleButton(MouseButton button, int edge, int mouse_x, int mouse_y);
	void handleMove(int mouse_x, int mouse_y);
	virtual int getIndexFrom(Event event) { return event.mData.mouse.button; };
	virtual int getNumReceivers() { return EKH_MOUSE_NUM_BUTTONS; };
public:
	static MouseHandler& getInstance();
	void init(unsigned int width, unsigned int height);	// Set up OpenGL textures used for mouseover targeting
	void cleanup();
	void resize(unsigned int width, unsigned int height);
	bool getButtonStatus(MouseButton button) const { return mButtonStatus[button]; };
	int getMouseX() const { return mMousePosition[0]; };
	int getMouseY() const { return mMousePosition[1]; };
	glm::vec2 getAspectCorrectedNDC() const;
	int const* getDragStartPosition(MouseButton button) const;
	void registerReceiver(bool interested[EKH_MOUSE_NUM_BUTTONS], Receiver* receiver);
	void registerReceiver(MouseButton button, Receiver* receiver);
	void registerMouseoverReceiver(Receiver* receiver);
	void unregisterReceiver(Receiver* receiver) final;
	virtual void step();
	void dispatchAll() final;
	void setAsDrawingTarget() const;
	void setAsTextureSource() const;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_MOUSE_HANDLER__