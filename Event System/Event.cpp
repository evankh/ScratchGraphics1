#include "Event.h"

Event::Event(EventType type, EventData* data) {
	mType = type;
	switch (mType) {
	case EKH_EVENT_KEY_PRESSED:
	case EKH_EVENT_KEY_RELEASED:
		mData.keyboard = *(KeyboardData*)data;
		break;
	case EKH_EVENT_BUTTON_PRESSED:
	case EKH_EVENT_BUTTON_RELEASED:
		mData.mouse = *(MouseData*)data;
		break;
	case EKH_EVENT_PLAY_SOUND_REQUEST:
		mData.sound = *(SoundData*)data;
		break;
	}
}