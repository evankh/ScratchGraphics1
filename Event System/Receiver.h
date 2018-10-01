#ifndef __EKH_SCRATCH_GRAPHICS_1_RECEIVER__
#define __EKH_SCRATCH_GRAPHICS_1_RECEIVER__

#include "Event.h"

class Receiver {
public:
	virtual void handle(const Event event) = 0;
};

struct ReceiverNode {
	Receiver* receiver;
	ReceiverNode* next;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_RECEIVER__