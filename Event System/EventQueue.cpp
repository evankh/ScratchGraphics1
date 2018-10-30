#include "EventQueue.h"

void EventQueue::push(Event event) {
	mEventQueue.push(event);
}

Event EventQueue::pop() {
	if (mEventQueue.size() > 0) {
		Event next = mEventQueue.front();
		mEventQueue.pop();
		return next;
	}
	return Event();
}
