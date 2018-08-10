#ifndef __EKH_SCRATCH_GRAPHICS_1_HANDLER__
#define __EKH_SCRATCH_GRAPHICS_1_HANDLER__

#include "Event System\Event.h"
#include "Event System\EventQueue.h"
#include "Event System\Receiver.h"

class Handler {
protected:
	EventQueue sEvents;
	ReceiverNode** sRegisteredReceivers;
	virtual int getIndexFrom(Event event) = 0;
	virtual int getNumReceivers() = 0;
public:
	void dispatchAll();
	void unregisterReceiver(Receiver* receiver);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_HANDLER__