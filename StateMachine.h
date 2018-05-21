#ifndef __EKH_SCRATCH_GRAPHICS_1_STATE_MACHINE__
#define __EKH_SCRATCH_GRAPHICS_1_STATE_MACHINE__

template <class State>
class StateMachine {
private:
	State mInitial;
	State mCurrent;
public:
//	StateMachine(State initial, void(Event*) transitions);
//	void(Event*) handle;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_STATE_MACHINE__
/*
template<class State>
inline StateMachine<State>::StateMachine(State initial, void(Event*) transitions) {
	mInitial = initial;
	mCurrent = initial;
	handle = transitions;
}
*/