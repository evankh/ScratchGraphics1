#ifndef __EKH_SCRATCH_GRAPHICS_1_KEYFRAME_MANAGER__
#define __EKH_SCRATCH_GRAPHICS_1_KEYFRAME_MANAGER__

#include <vector>

template<class T>
T constant(const T& a, const T& b, float f) {
	return a;
}

template<class T>
T lerp(const T& a, const T& b, float f) {
	return a + f * (b - a);
}

template<class T, T(*interp)(const T&, const T&, float) = constant>
class KeyframeManager {
private:
	struct Keyframe {
		float time, duration;
		T data;
	};
	std::vector<Keyframe> mKeyframes;
	int mCurrentKeyframe;
	Keyframe& current() { return mKeyframes[mCurrentKeyframe]; };
	Keyframe& next() { return mKeyframes[(mCurrentKeyframe + 1) % mKeyframes.size()]; };
public:
	void add(const T& datum, float duration) {
		Keyframe kf;
		kf.time = 0.0f;
		kf.duration = duration;
		kf.data = datum;
		mKeyframes.push_back(kf);
	}
	void update(float dt) {
		current().time += dt;
		while (current().time >= current().duration) {
			next().time = current().time - current().duration;
			mCurrentKeyframe = (mCurrentKeyframe + 1) % mKeyframes.size();
		}
	}
	T getCurrent() {
		return interp(current().data, next().data, current().time / current().duration);
	}
};

#endif//__EKH_SCRATCH_GRAPHICS_1_KEYFRAME_MANAGER__