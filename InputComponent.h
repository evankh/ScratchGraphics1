#ifndef __EKH_SCRATCH_GRPAHICS_1_INPUT_COMPONENT__
#define __EKH_SCRATCH_GRAPHICS_1_INPUT_COMPONENT__

#include "Event System\EventQueue.h"
#include "Event System\Receiver.h"
class GameObject;

class InputComponent {
public:
	virtual ~InputComponent() {};
	virtual void update(GameObject* owner/*StateObject& state*/) = 0;
};

class KeyboardInputComponent :public InputComponent, public Receiver {
private:
	EventQueue mKeyboardEvents;
public:
	KeyboardInputComponent(int numInterested, const char* interested);
	~KeyboardInputComponent();
	virtual void handle(const Event event);
	virtual void update(GameObject* owner/*StateObject& state*/);
};

class MouseInputComponent :public InputComponent, public Receiver {
private:
	EventQueue mMouseEvents;
public:
	MouseInputComponent(bool interested[5]);
	~MouseInputComponent();
	virtual void handle(const Event event);
	virtual void update(GameObject* owner/*StateObject& state*/);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_INPUT_COMPONENT__