#ifndef __EKH_SCRATCH_GRPAHICS_1_MOUSE_HANDLER__
#define __EKH_SCRATCH_GRAPHICS_1_MOUSE_HANDLER__

#include "Event System\Event.h"
#include "Event System\EventQueue.h"
#include "Event System\Receiver.h"

enum MouseButton {
	EKH_MOUSE_BUTTON_LEFT = 0,
	EKH_MOUSE_BUTTON_MIDDLE,
	EKH_MOUSE_BUTTON_RIGHT,
	EKH_MOUSE_SCROLL_UP,
	EKH_MOUSE_SCROLL_DOWN,

	EKH_MOUSE_NUM_BUTTONS
};

class MouseHandler {
	friend void game_mouse_wrapper(int, int, int, int);
	friend void game_movement_wrapper(int, int);
private:
	static bool sButtonStatus[EKH_MOUSE_NUM_BUTTONS];
	static int sMousePosition[2];
	static void handleButton(MouseButton button, int edge, int mouse_x, int mouse_y);
	static void handleMove(int mouse_x, int mouse_y);
	static EventQueue sEvents;
	static ReceiverNode* sRegisteredReceivers[EKH_MOUSE_NUM_BUTTONS];
public:
	static bool getButtonStatus(MouseButton button) { return sButtonStatus[button]; };
	static int getMouseX() { return sMousePosition[0]; };
	static int getMouseY() { return sMousePosition[1]; };
	static void dispatchAll();
	static void registerReceiver(bool interested[EKH_MOUSE_NUM_BUTTONS], Receiver* receiver);
	static void registerReceiver(MouseButton button, Receiver* receiver);
	static void unregisterReceiver(Receiver* receiver);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_MOUSE_HANDLER__