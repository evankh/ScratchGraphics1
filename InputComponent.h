#ifndef __EKH_SCRATCH_GRPAHICS_1_INPUT_COMPONENT__
#define __EKH_SCRATCH_GRAPHICS_1_INPUT_COMPONENT__

#include "EventQueue.h"

class InputComponent {

};

class KeyboardInputComponent :public InputComponent {
private:
	static EventQueue sKeyboardEvents;
public:
	static void queueEvent(char key, int x, int y);

};

#endif//__EKH_SCRATCH_GRAPHICS_1_INPUT_COMPONENT__