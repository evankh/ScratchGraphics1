#include "Bounds.h"
#include "Geometry.h"

void BoundingSphere::debugDraw() {
	Geometry::drawUnitSphere();//mCenter, mRadius);
}

void AABoundingCylinder::debugDraw() {
	Geometry::drawUnitCylinder();//mCenter, mRadius, mHeight);
}

void AABB::debugDraw() {
	Geometry::drawUnitBox();//mMin, mMax);
}

void ArbitraryBoundingCylinder::debugDraw() {
	Geometry::drawUnitCylinder();//mCylinder.mCenter, mCylinder.mRadius, mCylinder.mHeight, mTransform);
}

void ArbitraryBoundingBox::debugDraw() {
	Geometry::drawUnitBox();//mBox.mMin, mBox.mMax, mTransform);
}

bool collides(Bounds* a, Bounds* b) {
	switch (b->mType) {
	case BoundsType::COLLISIONLESS:
		return false;
	case BoundsType::SPHERE: {
		BoundingSphere* b_ = static_cast<BoundingSphere*>(b);
		return a->collides(b_);
	}
	case BoundsType::AA_CYLINDER: {
		AABoundingCylinder* b_ = static_cast<AABoundingCylinder*>(b);
		return a->collides(b_);
	}
	case BoundsType::AA_BOX: {
		AABB* b_ = static_cast<AABB*>(b);
		return a->collides(b_);
	}
	case BoundsType::CYLINDER: {
		ArbitraryBoundingCylinder* b_ = static_cast<ArbitraryBoundingCylinder*>(b);
		return a->collides(b_);
	}
	case BoundsType::BOX: {
		ArbitraryBoundingBox* b_ = static_cast<ArbitraryBoundingBox*>(b);
		return a->collides(b_);
	}
	}
	return false;
}

bool BoundingSphere::collides(BoundingSphere* b) {
	return (mCenter - b->mCenter).length() <= mRadius + b->mRadius;
}

bool BoundingSphere::collides(AABoundingCylinder* b) {
	return true;
}

bool BoundingSphere::collides(AABB* b) {
	return mCenter.x < b->mMax.x &&mCenter.x > b->mMin.x;
}

bool BoundingSphere::collides(ArbitraryBoundingCylinder* b) {
	return false;
}

bool BoundingSphere::collides(ArbitraryBoundingBox* b) {
	return false;
}

bool AABoundingCylinder::collides(AABoundingCylinder* b) {
	return false;
}

bool AABoundingCylinder::collides(AABB* b) {
	return false;
}

bool AABoundingCylinder::collides(ArbitraryBoundingCylinder* b) {
	return false;
}

bool AABoundingCylinder::collides(ArbitraryBoundingBox* b) {
	return false;
}

bool AABB::collides(AABB* b) {
	return mMin.x < b->mMax.x && mMax.x > b->mMin.x;	// And so on...
}

bool AABB::collides(ArbitraryBoundingCylinder* b) {
	return false;
}

bool AABB::collides(ArbitraryBoundingBox* b) {
	return false;
}

bool ArbitraryBoundingCylinder::collides(ArbitraryBoundingCylinder* b) {
	return mCylinder.collides(b);
}

bool ArbitraryBoundingCylinder::collides(ArbitraryBoundingBox* b) {
	return mCylinder.collides(b);
}

bool ArbitraryBoundingBox::collides(ArbitraryBoundingBox* b) {
	return mBox.collides(b);
}