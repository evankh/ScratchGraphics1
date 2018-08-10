#ifndef __EKH_SCRATCH_GRPAHICS_1_MOUSE_HANDLER__
#define __EKH_SCRATCH_GRAPHICS_1_MOUSE_HANDLER__

#include "Handler.h"

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
	bool sButtonStatus[EKH_MOUSE_NUM_BUTTONS];
	int sMousePosition[2];
	void handleButton(MouseButton button, int edge, int mouse_x, int mouse_y);
	void handleMove(int mouse_x, int mouse_y);
	virtual int getIndexFrom(Event event) { return event.mData.mouse.button; };
	virtual int getNumReceivers() { return EKH_MOUSE_NUM_BUTTONS; };
public:
	static MouseHandler& getInstance();
	bool getButtonStatus(MouseButton button) { return sButtonStatus[button]; };
	int getMouseX() { return sMousePosition[0]; };
	int getMouseY() { return sMousePosition[1]; };
	void registerReceiver(bool interested[EKH_MOUSE_NUM_BUTTONS], Receiver* receiver);
	void registerReceiver(MouseButton button, Receiver* receiver);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_MOUSE_HANDLER__