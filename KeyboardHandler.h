#ifndef __EKH_SCRATCH_GRAPHICS_1_KEYBOARD_HANDLER__
#define __EKH_SCRATCH_GRAPHICS_1_KEYBOARD_HANDLER__

#include "Handler.h"

class KeyboardHandler :public Handler {
	friend void game_keydown_wrapper(unsigned char, int, int);
	friend void game_keyup_wrapper(unsigned char, int, int);
private:
	bool sKeyStatus[256];
	void handlePress(char key, int mouse_x, int mouse_y);
	void handleRelease(char key, int mouse_x, int moue_y);
	virtual int getIndexFrom(Event event) { return event.mData.keyboard.key; };
	virtual int getNumReceivers() { return 256; };
public:
	static KeyboardHandler& getInstance();
	bool getKeyStatus(char key) { return sKeyStatus[key]; };
	void registerReceiver(const char* interested, Receiver* receiver);
	void registerReceiver(char interested, Receiver* receiver);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_KEYBOARD_HANDLER__