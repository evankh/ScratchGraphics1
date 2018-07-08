#ifndef __EKH_SCRATCH_GRPAHICS_1_INPUT_COMPONENT__
#define __EKH_SCRATCH_GRAPHICS_1_INPUT_COMPONENT__

#include "Event System\EventQueue.h"
#include "Event System\Receiver.h"

class InputComponent {

};

class KeyboardInputComponent :public InputComponent, public Receiver {
private:
	EventQueue mKeyboardEvents;
public:
	virtual void handle(Event event);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_INPUT_COMPONENT__