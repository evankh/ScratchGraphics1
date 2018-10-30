#ifndef __EKH_SCRATCH_GRAPHICS_1_EVENT__
#define __EKH_SCRATCH_GRAPHICS_1_EVENT__

enum class EventType {
	INVALID = -1,
	NONE,
	KEY_PRESSED,
	KEY_RELEASED,
	KEY_HELD,
	BUTTON_PRESSED,
	BUTTON_RELEASED,
	BUTTON_HELD,
	PLAY_SOUND_REQUEST,
	COLLISION
};

enum MouseButton;

struct EmptyData {};

struct KeyboardData {
	char key;
	int mouse_x, mouse_y;
};

struct MouseData {
	MouseButton button;
	int edge;
	int mouse_x, mouse_y;
};

struct SoundData {
	const char* name;
	float gain;
};

struct CollisionData {
	float floorheight;
};

struct Event {
	EventType mType;
	union Data {
		EmptyData empty;
		KeyboardData keyboard;
		MouseData mouse;
		SoundData sound;
		CollisionData collision;
	} mData;
	Event() :mType(EventType::NONE) { mData.empty = EmptyData(); };
	Event(EventType type, KeyboardData data) :mType(type) { mData.keyboard = data; };
	Event(EventType type, MouseData data) :mType(type) { mData.mouse = data; };
	Event(SoundData data) :mType(EventType::PLAY_SOUND_REQUEST) { mData.sound = data; };
	Event(CollisionData data) :mType(EventType::COLLISION) { mData.collision = data; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_EVENT__