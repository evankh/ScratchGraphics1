#include "Event.h"

Event::Event(EventType type, EventData* data) {
	mType = type;
	switch (mType) {
	case EKH_EVENT_KEY_PRESSED:
		mData.keyboard = *(KeyboardData*)data;
		break;
	case EKH_EVENT_BUTTON_PRESSED:
		mData.mouse = *(MouseData*)data;
		break;
	}
}