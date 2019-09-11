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
	MOUSEOVER,
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

struct MouseoverData {
	int mouse_x, mouse_y;
	float world_pos[3], local_pos[3];
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
		MouseoverData mouseover;
		SoundData sound;
		CollisionData collision;
		CommandData command;
	} mData;
	float mTimer = 0.0f;
	Event() :mType(EventType::NONE) { mData.empty = EmptyData(); };
	Event(EventType type, KeyboardData data, float timer = 0.0f) :mType(type), mTimer(timer) { mData.keyboard = data; };
	Event(EventType type, MouseData data, float timer = 0.0f) :mType(type), mTimer(timer) { mData.mouse = data; };
	Event(MouseoverData data, float timer = 0.0f) :mType(EventType::MOUSEOVER), mTimer(timer) { mData.mouseover = data; };
	Event(SoundData data, float timer = 0.0f) :mType(EventType::PLAY_SOUND_REQUEST), mTimer(timer) { mData.sound = data; };
	Event(CollisionData data, float timer = 0.0f) :mType(EventType::COLLISION), mTimer(timer) { mData.collision = data; };
	Event(CommandData data, float timer = 0.0f) :mType(EventType::COMMAND), mTimer(timer) { mData.command = data; };
};

#include <queue>

struct EventQueue {
	std::queue<Event> mQueue;
public:
	bool isEmpty() const { return mQueue.empty(); };
	int size() const { return mQueue.size(); };
	void push(const Event e) { mQueue.push(e); };
	Event pop() {
		if (!isEmpty()) {
			Event ret = mQueue.front();
			mQueue.pop();
			return ret;
		}
		return Event();
	};
};

#endif//__EKH_SCRATCH_GRAPHICS_1_EVENT__