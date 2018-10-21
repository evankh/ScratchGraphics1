#ifndef __EKH_SCRATCH_GRAPHICS_1_EVENT__
#define __EKH_SCRATCH_GRAPHICS_1_EVENT__

enum EventType {
	EKH_EVENT_NONE,
	EKH_EVENT_KEY_PRESSED,
	EKH_EVENT_KEY_RELEASED,
	EKH_EVENT_KEY_HELD,
	EKH_EVENT_BUTTON_PRESSED,
	EKH_EVENT_BUTTON_RELEASED,
	EKH_EVENT_BUTTON_HELD,
	EKH_EVENT_PLAY_SOUND_REQUEST
};

enum MouseButton;

struct EventData {};

struct KeyboardData :public EventData {
	char key;
	int mouse_x, mouse_y;
};

struct MouseData :public EventData {
	MouseButton button;
	int edge;
	int mouse_x, mouse_y;
};

struct SoundData :public EventData {
	const char* name;
	float gain;
};

struct Event {
	EventType mType;
	union Data {
		KeyboardData keyboard;
		MouseData mouse;
		SoundData sound;
	} mData;
	// Why am I doing such weird things with inheritance?
	// Is it all so I can have one function here?
	// I don't like that, especially since I'm not sure how much data from the passed in reference
	// is actually being copied into the Event object
	Event(EventType type, EventData* data);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_EVENT__