#ifndef __EKH_SCRATCH_GRAPHICS_1_EVENT__
#define __EKH_SCRATCH_GRAPHICS_1_EVENT__

enum EventType {
	EKH_EVENT_NONE,
	EKH_EVENT_KEY_PRESSED,
	EKH_EVENT_KEY_RELEASED,
	EKH_EVENT_KEY_HELD,
	EKH_EVENT_BUTTON_PRESSED,
	EKH_EVENT_BUTTON_RELEASED,
	EKH_EVENT_BUTTON_HELD
};

struct EventData {};

struct KeyboardData :public EventData {
	char key;
	int mouse_x, mouse_y;
};

struct MouseData :public EventData {
	int mouse_x, mouse_y;
};

struct Event {
	EventType mType;
	union {
		KeyboardData keyboard;
		MouseData mouse;
	} mData;
	Event(EventType type, EventData* data);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_EVENT__