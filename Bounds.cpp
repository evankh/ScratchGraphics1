#include "Bounds.h"
#include "Geometry.h"

void BoundingSphere::debugDraw() {
	Geometry::drawUnitSphere();//mCenter, mRadius);
}

void AABoundingCylinder::debugDraw() {
	Geometry::drawUnitCylinder();//mCenter, mRadius, mHeight);
}

void AABB::debugDraw() {
	Geometry::drawBox(mMin, mMax);
}

void ArbitraryBoundingCylinder::debugDraw() {
	Geometry::drawUnitCylinder();//mCylinder.mCenter, mCylinder.mRadius, mCylinder.mHeight, mTransform);
}

void ArbitraryBoundingBox::debugDraw() {
	Geometry::drawUnitBox();//mBox.mMin, mBox.mMax, mTransform);
}

void CollisionPlane::debugDraw() {
	// Geometry's static draw calls are actually super unhelpful
	return;
}

void AABB::translate(glm::vec3 dxyz) {
	mMin += dxyz;
	mMax += dxyz;
}
void AABB::scale(float scale) {
	// Not sure, maybe scale both mMin & mMax towards the center?
}

void CollisionPlane::rotate(glm::vec3 axis, float degrees) {
	// Dunno exactly what this one needs to do
}

void AABB::update(float vert[3]) {
	if (vert[0] < mMin.x)
		mMin.x = vert[0];
	if (vert[0] > mMax.x)
		mMax.x = vert[0];
	if (vert[1] < mMin.y)
		mMin.y = vert[1];
	if (vert[1] > mMax.y)
		mMax.y = vert[1];
	if (vert[2] < mMin.z)
		mMin.z = vert[2];
	if (vert[2] > mMax.z)
		mMax.z = vert[2];
}

// There must be some more efficient way to write this... maybe with macros?
bool collides(Bounds* a, Bounds* b) {
	switch (a->mType) {
	case BoundsType::COLLISIONLESS:
		return false;
	case BoundsType::SPHERE: {
		BoundingSphere* a_ = static_cast<BoundingSphere*>(a);
		switch (b->mType) {
		case BoundsType::COLLISIONLESS:
			return false;
		case BoundsType::SPHERE: {
			BoundingSphere* b_ = static_cast<BoundingSphere*>(b);
			return collides(a_, b_);
		}
		case BoundsType::AA_BOX: {
			AABB* b_ = static_cast<AABB*>(b);
			return collides(a_, b_);
		}
		case BoundsType::PLANE: {
			CollisionPlane* b_ = static_cast<CollisionPlane*>(b);
			return collides(a_, b_);
		}
		}
	}
	case BoundsType::AA_BOX: {
		AABB* a_ = static_cast<AABB*>(a);
		switch (b->mType) {
		case BoundsType::COLLISIONLESS:
			return false;
		case BoundsType::SPHERE: {
			BoundingSphere* b_ = static_cast<BoundingSphere*>(b);
			return collides(a_, b_);
		}
		case BoundsType::AA_BOX: {
			AABB* b_ = static_cast<AABB*>(b);
			return collides(a_, b_);
		}
		case BoundsType::PLANE: {
			CollisionPlane* b_ = static_cast<CollisionPlane*>(b);
			return collides(a_, b_);
		}
		}
	}
	case BoundsType::PLANE: {
		CollisionPlane* a_ = static_cast<CollisionPlane*>(a);
		switch (b->mType) {
		case BoundsType::COLLISIONLESS:
			return false;
		case BoundsType::SPHERE: {
			BoundingSphere* b_ = static_cast<BoundingSphere*>(b);
			return collides(a_, b_);
		}
		case BoundsType::AA_BOX: {
			AABB* b_ = static_cast<AABB*>(b);
			return collides(a_, b_);
		}
		case BoundsType::PLANE: {
			CollisionPlane* b_ = static_cast<CollisionPlane*>(b);
			return collides(a_, b_);
		}
		}
	}
	}
	return false;
}

bool collides(BoundingSphere* a, BoundingSphere* b) {
	return (a->mCenter - b->mCenter).length() <= a->mRadius + b->mRadius;
}
/*
bool collides(BoundingSphere* a, AABoundingCylinder* b) {
	return true;
}
*/
bool collides(BoundingSphere* a, AABB* b) {
	return a->mCenter.x < b->mMax.x && a->mCenter.x > b->mMin.x;
}
/*
bool collides(BoundingSphere* a, ArbitraryBoundingCylinder* b) {
	return false;
}

bool collides(BoundingSphere* a, ArbitraryBoundingBox* b) {
	return false;
}
*/
bool collides(BoundingSphere* a, CollisionPlane* b) {
	return glm::dot(a->mCenter - b->mPosition, b->mNormal) <= a->mRadius + b->mThreshold;
}
/*
bool collides(AABoundingCylinder* a, AABoundingCylinder* b) {
	return false;
}

bool collides(AABoundingCylinder* a, AABB* b) {
	return false;
}

bool collides(AABoundingCylinder* a, ArbitraryBoundingCylinder* b) {
	return false;
}

bool collides(AABoundingCylinder* a, ArbitraryBoundingBox* b) {
	return false;
}
*/

bool collides(AABB* a, BoundingSphere* b) {
	return collides(b, a);
}

bool collides(AABB* a, AABB* b) {
	return a->mMin.x < b->mMax.x && a->mMax.x > b->mMin.x;	// And so on...
}
/*
bool collides(AABB* a, ArbitraryBoundingCylinder* b) {
	return false;
}

bool collides(AABB* a, ArbitraryBoundingBox* b) {
	return false;
}
*/
bool collides(AABB* a, CollisionPlane* b) {
	bool collides = false;
	collides = collides && glm::dot(a->mMin - b->mPosition, b->mNormal) <= b->mThreshold;
	collides = collides && glm::dot(a->mMax - b->mPosition, b->mNormal) <= b->mThreshold;
	// And now to do the same thing for the other 6 vertices...
	return collides;
}

bool collides(CollisionPlane* a, BoundingSphere* b) {
	return collides(b, a);
}

bool collides(CollisionPlane* a, AABB* b) {
	return collides(b, a);
}

bool collides(CollisionPlane* a, CollisionPlane* b) {	// Planes are infinite and will nearly always collide, only way they won't is if they're exactly parallel or antiparallel (and not in the same position)
	return a->mPosition == b->mPosition || (a->mNormal != b->mNormal && a->mNormal != -b->mNormal);	// Though even this fails if mPosition is translated in the plane
}