#ifndef __EKH_SCRATCH_GRAPHICS_1_RECEIVER__
#define __EKH_SCRATCH_GRAPHICS_1_RECEIVER__

#include "Event.h"

class Receiver {
	int mIndex = 0;
public:
	virtual void handle(const Event event) = 0;
	void setIndex(int i) { mIndex = i; };
	int getIndex() const { return mIndex; };
};

struct ReceiverNode {
	Receiver* receiver;
	ReceiverNode* next;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_RECEIVER__