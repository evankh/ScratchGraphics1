#ifndef __EKH_SCRATCH_GRAPHICS_1_KEYBOARD_HANDLER__
#define __EKH_SCRATCH_GRAPHICS_1_KEYBOARD_HANDLER__

#include "Event System\Event.h"
#include "Event System\EventQueue.h"
#include "Event System\Receiver.h"
#include <map>
#include <list>

class KeyboardHandler {
	friend void game_keydown_wrapper(unsigned char, int, int);
	friend void game_keyup_wrapper(unsigned char, int, int);
private:
	static bool sKeyStatus[256];
	static void handlePress(char key, int mouse_x, int mouse_y);
	static void handleRelease(char key, int mouse_x, int moue_y);
	static EventQueue sEvents;
	static ReceiverNode* sRegisteredReceivers[256];
public:
	static bool getKeyStatus(char key) { return sKeyStatus[key]; };
	static void dispatchAll();
	static void registerReceiver(char interested, Receiver* receiver);
	static void unregisterReceiver(Receiver* receiver);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_KEYBOARD_HANDLER__