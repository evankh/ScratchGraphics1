#include "Bounds.h"

bool collides(Collisionless*, BoundingSphere*) { return false; }
bool collides(BoundingSphere*, Collisionless*) { return false; }
bool collides(Collisionless*, AABB*) { return false; }
bool collides(AABB*, Collisionless*) { return false; }

bool collides(BoundingSphere* a, BoundingSphere* b) {
	return (a->mCenter - b->mCenter).length() <= a->mRadius + b->mRadius;
}

bool collides(BoundingSphere* a, AABB* b) {
	return a->mCenter.x < b->mMax.x &&a->mCenter.x > b->mMin.x;	// Gonna hafta look this one up
}

bool collides(AABB* a, BoundingSphere* b) {
	return collides(b, a);
}

bool collides(AABB* a, AABB* b) {
	return a->mMin.x < b->mMax.x && a->mMax.x > b->mMin.x;	// And so on...
}