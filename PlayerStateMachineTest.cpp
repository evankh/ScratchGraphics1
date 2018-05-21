#include "StateMachine.h"

enum EventType {
	keyboard_event,
	mouse_click_event,
	collision_event
};

enum PlayerState {
	standing_still,
	moving_left,
	moving_right,
	jumping,
	double_jumping
};

struct Event{
	EventType type;
	int data;
};

void playerStateTransitions(Event* event) {
	PlayerState current = standing_still;
	int facing = 0;
	switch (current) {
	case standing_still:
		switch (event->type) {
		case keyboard_event:
			switch (event->data) {
			case 'A':
				current = moving_left;
				//velocity.x = -1;
				facing = -1;
				break;
			case 'D':
				current = moving_right;
				//velocity.x = 1;
				facing = 1;
				break;
			default: break;
			}
			break;
		case mouse_click_event:
			current = jumping;
			//velocity.y = 1;
			//gravity = -1;
			break;
		}
		break;
	case moving_left:
		switch (event->type) {
		case collision_event:
			current = standing_still;
			//velocity.x = 0;
			break;
		case mouse_click_event:
			current = jumping;
			//velocity.y = 1;
			//gravity = -1;
			break;
		}
		break;
	case moving_right:
		// same as moving_left
		break;
	case jumping:
		switch (event->type) {
		case collision_event:
			current = standing_still;
			//velocity.y = 0
			//gravity = 0;
			break;
		case keyboard_event:
			switch (event->data) {
			case 'A':
				facing = -1;
				break;
			case 'D':
				facing = 1;
				break;
			default: break;
			}
			break;
		case mouse_click_event:
			current = double_jumping;
			//velocity.y = 1;
			break;
		}
		break;
	case double_jumping:
		switch (event->type) {
		case collision_event:
			current = standing_still;
			//velocity.y = 0;
			//gravity = 0;
			break;
		}
		break;
	}
}