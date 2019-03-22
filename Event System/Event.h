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
	COLLISION,
	COMMAND
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
	float world_pos[3];
};

struct SoundData {
	const char* name;
	float gain;
};

struct CollisionData {
	class PhysicsComponent* first, *second;
};

enum class Command {
	NONE,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_LEFT,
	MOVE_RIGHT,
	JUMP
};

struct CommandData {
	Command command;
	float factor;	// We'll see what data they actually need
};

struct Event {
	EventType mType;
	union Data {
		EmptyData empty;
		KeyboardData keyboard;
		MouseData mouse;
		SoundData sound;
		CollisionData collision;
		CommandData command;
	} mData;
	Event() :mType(EventType::NONE) { mData.empty = EmptyData(); };
	Event(EventType type, KeyboardData data) :mType(type) { mData.keyboard = data; };
	Event(EventType type, MouseData data) :mType(type) { mData.mouse = data; };
	Event(SoundData data) :mType(EventType::PLAY_SOUND_REQUEST) { mData.sound = data; };
	Event(CollisionData data) :mType(EventType::COLLISION) { mData.collision = data; };
	Event(CommandData data) :mType(EventType::COMMAND) { mData.command = data; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_EVENT__