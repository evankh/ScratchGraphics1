#ifndef __EKH_SCRATCH_GRAPHICS_1_BOUNDS__
#define __EKH_SCRATCH_GRAPHICS_1_BOUNDS__

#include <glm\vec3.hpp>

class Bounds {
};

class Collisionless :public Bounds {
};

class AABB;
class BoundingSphere :public Bounds {
	friend bool collides(BoundingSphere*, BoundingSphere*);
	friend bool collides(BoundingSphere*, AABB*);
	glm::vec3 mCenter;
	float mRadius;
};

class AABB :public Bounds {
	friend bool collides(BoundingSphere*, AABB*);
	friend bool collides(AABB*, AABB*);
	glm::vec3 mMin, mMax;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_BOUNDS__