#include "Bounds.h"
#include "Geometry.h"

void BoundingSphere::debugDraw() {
	Geometry::drawSphere(mCenter, mRadius);
}
/*
void AABoundingCylinder::debugDraw() {
	Geometry::drawUnitCylinder();//mCenter, mRadius, mHeight);
}
*/
void AABB::debugDraw() {
	//Geometry::drawUnitBox();
	Geometry::drawBox(mMin, mMax);
}
/*
void ArbitraryBoundingCylinder::debugDraw() {
	Geometry::drawUnitCylinder();//mCylinder.mCenter, mCylinder.mRadius, mCylinder.mHeight, mTransform);
}

void ArbitraryBoundingBox::debugDraw() {
	Geometry::drawUnitBox();//mBox.mMin, mBox.mMax, mTransform);
}
*/
void CollisionPlane::debugDraw() {
	//Geometry::drawUnitQuad();
	return;
}

void AABB::translate(glm::vec3 dxyz) {
	mOrigin += dxyz;
	mMin += dxyz;
	mMax += dxyz;
}
void AABB::scale(glm::vec3 scale) {
	// Not sure, maybe scale both mMin & mMax towards the center?
	mMax = mOrigin + scale * (mMax - mOrigin);
	mMin = mOrigin + scale * (mMin - mOrigin);
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
	return glm::length(a->mCenter - b->mCenter) <= a->mRadius + b->mRadius;
}
/*
bool collides(BoundingSphere* a, AABoundingCylinder* b) {
	return true;
}
*/
bool collides(BoundingSphere* a, AABB* b) {
	glm::vec3 diff = b->mMax - b->mMin;
	glm::vec3 center = a->mCenter;
	center.x = std::fmaxf(std::fminf(center.x, b->mMin.x), center.x - diff.x);
	center.y = std::fmaxf(std::fminf(center.y, b->mMin.y), center.y - diff.y);
	center.z = std::fmaxf(std::fminf(center.z, b->mMin.z), center.z - diff.z);
	return collides(&BoundingSphere(center, a->mRadius), &BoundingSphere(b->mMin, 0.0));
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
	return a->mMin.x < b->mMax.x && a->mMax.x > b->mMin.x
		&& a->mMin.y < b->mMax.y && a->mMax.y > b->mMin.y
		&& a->mMin.z < b->mMax.z && a->mMax.z > b->mMin.z;
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
	collides = collides || glm::dot(a->mMin - b->mPosition, b->mNormal) <= b->mThreshold;
	collides = collides || glm::dot(a->mMax - b->mPosition, b->mNormal) <= b->mThreshold;
	collides = collides || glm::dot(glm::vec3{ a->mMin.x,a->mMin.y,a->mMax.z } - b->mPosition, b->mNormal) <= b->mThreshold;
	collides = collides || glm::dot(glm::vec3{ a->mMin.x,a->mMax.y,a->mMin.z } - b->mPosition, b->mNormal) <= b->mThreshold;
	collides = collides || glm::dot(glm::vec3{ a->mMin.x,a->mMax.y,a->mMax.z } - b->mPosition, b->mNormal) <= b->mThreshold;
	collides = collides || glm::dot(glm::vec3{ a->mMax.x,a->mMin.y,a->mMin.z } - b->mPosition, b->mNormal) <= b->mThreshold;
	collides = collides || glm::dot(glm::vec3{ a->mMax.x,a->mMin.y,a->mMax.z } - b->mPosition, b->mNormal) <= b->mThreshold;
	collides = collides || glm::dot(glm::vec3{ a->mMax.x,a->mMax.y,a->mMin.z } - b->mPosition, b->mNormal) <= b->mThreshold;
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