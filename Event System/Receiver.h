#ifndef __EKH_SCRATCH_GRAPHICS_1_RECEIVER__
#define __EKH_SCRATCH_GRAPHICS_1_RECEIVER__

class Receiver {
public:
	virtual void handle(const Event event) = 0;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_RECEIVER__