#ifndef __EKH_SCRATCH_GRPAHICS_1_INPUT_COMPONENT__
#define __EKH_SCRATCH_GRAPHICS_1_INPUT_COMPONENT__

#include "Event System\EventQueue.h"
#include "Event System\Receiver.h"

class InputComponent {
public:
	virtual ~InputComponent() {};
	virtual void update(/*StateObject& state*/) = 0;
};

class KeyboardInputComponent :public InputComponent, public Receiver {
private:
	EventQueue mKeyboardEvents;
public:
	KeyboardInputComponent(int numInterested, const char* interested);
	~KeyboardInputComponent();
	virtual void handle(const Event event);
	virtual void update(/*StateObject& state*/);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_INPUT_COMPONENT__