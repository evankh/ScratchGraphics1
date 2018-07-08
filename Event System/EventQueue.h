#ifndef __EKH_SCRATCH_GRAPHICS_1_EVENT_QUEUE__
#define __EKH_SCRATCH_GRAPHICS_1_EVENT_QUEUE__

#include "Event.h"
#include <queue>

class EventQueue {
	std::queue<Event> mEventQueue;
public:
	bool isEmpty() const { return mEventQueue.empty(); };
	void push(Event event);
	Event pop();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_EVENT_QUEUE__