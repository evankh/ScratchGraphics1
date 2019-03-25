#ifndef __EKH_SCRATCH_GRAPHICS_1_HANDLER__
#define __EKH_SCRATCH_GRAPHICS_1_HANDLER__

#include "Event System\Event.h"
#include "Event System\EventQueue.h"
#include "Event System\Receiver.h"

class Handler {
protected:
	EventQueue sEvents;
	ReceiverNode** mRegisteredReceivers;
	virtual int getIndexFrom(Event event) = 0;
	virtual int getNumReceivers() = 0;
public:
	virtual void step() = 0;
	virtual void dispatchAll();
	virtual void unregisterReceiver(Receiver* receiver);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_HANDLER__