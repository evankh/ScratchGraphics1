#include "Mouse.h"
#define NULL 0

MouseEvent* Mouse::sHead = NULL;
MouseEvent* Mouse::sTail = NULL;

Mouse::~Mouse() {
	clear();
}

void Mouse::append(MouseEvent* event) {
	if (sTail) {
		sTail->next = event;
		sTail = sTail->next;
	}
	else {
		sHead = sTail = event;
		sTail->next = NULL;
	}
}

MouseEvent* Mouse::search(SSBB ssbb) {
	MouseEvent start = MouseEvent{ EKH_MOUSE_EVENT_NONE,0,0,0,0,0,0, sHead }, *iter = &start, *result = NULL, *tail = NULL;
	while (iter->next) {
		if (ssbb.inside(iter->next->x, iter->next->y)) {
			if (iter->next == sHead)
				sHead = iter->next->next;
			if (result) {
				tail->next = iter->next;
				tail = tail->next;
			}
			else {
				result = iter->next;
				tail = result;
			}
			iter->next = iter->next->next;
			iter = iter->next;
			tail->next = NULL;
		}
	}
	return result;
}

void Mouse::clear() {
	MouseEvent* iter = sHead;
	while (iter) {
		MouseEvent* temp = iter;
		iter = iter->next;
		delete temp;
	}
	sHead = sTail = NULL;
}

bool SSBB::inside(int pt_x, int pt_y) {
	return x < pt_x && pt_x < x + w && y < pt_y && pt_y < y + h;
}
