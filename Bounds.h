#ifndef __EKH_SCRATCH_GRAPHICS_1_BOUNDS__
#define __EKH_SCRATCH_GRAPHICS_1_BOUNDS__

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

enum class BoundsType {
	COLLISIONLESS,
	SPHERE,
	AA_CYLINDER,
	AA_BOX,
	CYLINDER,
	BOX,
	PLANE,
	DISTANCE_ESTIMATOR,
	MESH
};

// I really don't like the way this whole thing is set up, I feel like it might be better to have one big honkin' collides(Bounds*, Bounds*)
// function with a double-switch and all the algorithms in one place, and forget about all the dynamic dispatch overhead

// Or the other, weirder way would be to have a big honking' table of function pointers, and use the BoundsType as an index, but that's just ridiculous

class Bounds {
	friend bool collides(Bounds*, Bounds*);
protected:
	BoundsType mType;
	Bounds(BoundsType type) :mType(type) {};
public:
	virtual void debugDraw() = 0;
	virtual void rotate(glm::vec3 axis, float degrees) = 0;	// Each primitive needs to decide for itself how it needs to handle transforms
	virtual void translate(glm::vec3 dxyz) = 0;
	virtual void scale(glm::vec3 scale) = 0;
	void scale(float scale) { this->scale(glm::vec3(scale)); };
	virtual Bounds* copy() const = 0;
};

class BoundingSphere;
class AABoundingCylinder;
class AABB;
class ArbitraryBoundingCylinder;
class ArbitraryBoundingBox;
class CollisionPlane;

class Collisionless :public Bounds {
	virtual void debugDraw() {};
	virtual void rotate(glm::vec3, float) {};
	virtual void translate(glm::vec3) {};
	virtual void scale(glm::vec3) {};
	virtual Collisionless* copy() const { return new Collisionless(); };
public:
	Collisionless() :Bounds(BoundsType::COLLISIONLESS) {};
};

class BoundingSphere :public Bounds {
	friend bool collides(BoundingSphere*, BoundingSphere*);
	friend bool collides(BoundingSphere*, AABoundingCylinder*);
	friend bool collides(BoundingSphere*, AABB*);
	friend bool collides(BoundingSphere*, ArbitraryBoundingCylinder*);
	friend bool collides(BoundingSphere*, ArbitraryBoundingBox*);
	friend bool collides(BoundingSphere*, CollisionPlane*);

	// I guess the main function could just swap the order of them instead, but then I need to keep track of what functions I have & haven't defined
	friend bool collides(AABoundingCylinder*, BoundingSphere*);
	friend bool collides(AABB*, BoundingSphere*);
	friend bool collides(ArbitraryBoundingCylinder*, BoundingSphere*);
	friend bool collides(ArbitraryBoundingBox*, BoundingSphere*);
	friend bool collides(CollisionPlane*, BoundingSphere*);
private:
	glm::vec3 mCenter;
	float mRadius;
public:
	BoundingSphere(glm::vec3 center, float radius) :Bounds(BoundsType::SPHERE) { mCenter = center; mRadius = radius; };
	virtual void debugDraw();
	virtual void rotate(glm::vec3 axis, float degrees) {};
	virtual void translate(glm::vec3 dxyz) { mCenter += dxyz; };
	virtual void scale(glm::vec3 scale) { mRadius *= scale.x; };	// It's a sphere, not an ellipsoid, so can't scale anisometrically
	virtual BoundingSphere* copy() const { return new BoundingSphere(mCenter, mRadius); };
};
/*
class AABoundingCylinder :public Bounds {
	friend bool collides(AABoundingCylinder*, BoundingSphere*);
	friend bool collides(AABoundingCylinder*, AABoundingCylinder*);
	friend bool collides(AABoundingCylinder*, AABB*);
	friend bool collides(AABoundingCylinder*, ArbitraryBoundingCylinder*);
	friend bool collides(AABoundingCylinder*, ArbitraryBoundingBox*);
	friend bool collides(AABoundingCylinder*, CollisionPlane*);

	friend bool collides(BoundingSphere*, AABoundingCylinder*);
	friend bool collides(AABB*, AABoundingCylinder*);
	friend bool collides(ArbitraryBoundingCylinder*, AABoundingCylinder*);
	friend bool collides(ArbitraryBoundingBox*, AABoundingCylinder*);
	friend bool collides(CollisionPlane*, AABoundingCylinder*);
private:
	glm::vec3 mCenter;
	float mRadius, mHeight;
public:
	AABoundingCylinder() :Bounds(BoundsType::AA_CYLINDER) {};
	virtual void debugDraw();
	virtual void rotate(glm::vec3 axis, float degrees) {}
	virtual void translate(glm::vec3 dxyz);
	virtual void scale(float scale);
};
*/
class AABB :public Bounds {
	friend bool collides(AABB*, BoundingSphere*);
	friend bool collides(AABB*, AABoundingCylinder*);
	friend bool collides(AABB*, AABB*);
	friend bool collides(AABB*, ArbitraryBoundingCylinder*);
	friend bool collides(AABB*, ArbitraryBoundingBox*);
	friend bool collides(AABB*, CollisionPlane*);

	friend bool collides(BoundingSphere*, AABB*);
	friend bool collides(AABoundingCylinder*, AABB*);
	friend bool collides(ArbitraryBoundingCylinder*, AABB*);
	friend bool collides(ArbitraryBoundingBox*, AABB*);
	friend bool collides(CollisionPlane*, AABB*);
private:
	glm::vec3 mOrigin = { 0.0,0.0,0.0 }, mMin, mMax;
public:
	AABB() :Bounds(BoundsType::AA_BOX) {};
	AABB(glm::vec3 min, glm::vec3 max) :Bounds(BoundsType::AA_BOX), mMin(min), mMax(max) {}
	virtual void debugDraw();
	virtual void rotate(glm::vec3 axis, float degrees) {};
	virtual void translate(glm::vec3 dxyz);
	virtual void scale(glm::vec3 scale);
	virtual AABB* copy() const { return new AABB(mMin, mMax); };
	void update(float vert[3]);
	//virtual glm::vec3 getDimensions() const { return mMax - mMin; };
	void setMinX(float x) { mMin.x = x; };
	void setMinY(float y) { mMin.y = y; };
	void setMinZ(float z) { mMin.z = z; };
	void setMaxX(float x) { mMin.x = x; };
	void setMaxY(float y) { mMin.y = y; };
	void setMaxZ(float z) { mMin.z = z; };
};
/*
class ArbitraryBoundingCylinder :public Bounds {
	friend bool collides(ArbitraryBoundingCylinder*, BoundingSphere*);
	friend bool collides(ArbitraryBoundingCylinder*, AABoundingCylinder*);
	friend bool collides(ArbitraryBoundingCylinder*, AABB*);
	friend bool collides(ArbitraryBoundingCylinder*, ArbitraryBoundingCylinder*);
	friend bool collides(ArbitraryBoundingCylinder*, ArbitraryBoundingBox*);
	friend bool collides(ArbitraryBoundingCylinder*, CollisionPlane*);

	friend bool collides(BoundingSphere*, ArbitraryBoundingCylinder*);
	friend bool collides(AABoundingCylinder*, ArbitraryBoundingCylinder*);
	friend bool collides(AABB*, ArbitraryBoundingCylinder*);
	friend bool collides(ArbitraryBoundingBox*, ArbitraryBoundingCylinder*);
	friend bool collides(CollisionPlane*, ArbitraryBoundingCylinder*);
private:
	AABoundingCylinder mCylinder;
	glm::mat4 mTransform;
public:
	ArbitraryBoundingCylinder() :Bounds(BoundsType::CYLINDER) {};
	virtual void debugDraw();
	virtual void rotate(glm::vec3 axis, float degrees);
	virtual void translate(glm::vec3 dxyz);
	virtual void scale(float scale);
};

class ArbitraryBoundingBox :public Bounds {
	friend bool collides(ArbitraryBoundingBox*, BoundingSphere*);
	friend bool collides(ArbitraryBoundingBox*, AABoundingCylinder*);
	friend bool collides(ArbitraryBoundingBox*, AABB*);
	friend bool collides(ArbitraryBoundingBox*, ArbitraryBoundingCylinder*);
	friend bool collides(ArbitraryBoundingBox*, ArbitraryBoundingBox*);
	friend bool collides(ArbitraryBoundingBox*, CollisionPlane*);

	friend bool collides(BoundingSphere*, ArbitraryBoundingBox*);
	friend bool collides(AABoundingCylinder*, ArbitraryBoundingBox*);
	friend bool collides(AABB*, ArbitraryBoundingBox*);
	friend bool collides(ArbitraryBoundingCylinder*, ArbitraryBoundingBox*);
	friend bool collides(CollisionPlane*, ArbitraryBoundingBox*);
private:
	AABB mBox;
	glm::mat4 mTransform;
public:
	ArbitraryBoundingBox() :Bounds(BoundsType::BOX) {};
	virtual void debugDraw();
	virtual void rotate(glm::vec3 axis, float degrees);
	virtual void translate(glm::vec3 dxyz);
	virtual void scale(float scale);
};
*/
class CollisionPlane :public Bounds {
	friend bool collides(CollisionPlane*, BoundingSphere*);
	friend bool collides(CollisionPlane*, AABoundingCylinder*);
	friend bool collides(CollisionPlane*, AABB*);
	friend bool collides(CollisionPlane*, ArbitraryBoundingCylinder*);
	friend bool collides(CollisionPlane*, ArbitraryBoundingBox*);
	friend bool collides(CollisionPlane*, CollisionPlane*);

	friend bool collides(BoundingSphere*, CollisionPlane*);
	friend bool collides(AABoundingCylinder*, CollisionPlane*);
	friend bool collides(AABB*, CollisionPlane*);
	friend bool collides(ArbitraryBoundingCylinder*, CollisionPlane*);
	friend bool collides(ArbitraryBoundingBox*, CollisionPlane*);
	friend bool collides(CollisionPlane*, CollisionPlane*);
private:
	glm::vec3 mPosition;
	glm::vec3 mNormal;
	float mThreshold = -0.0f;
public:
	CollisionPlane(glm::vec3 position, glm::vec3 normal) :Bounds(BoundsType::PLANE) { mPosition = position; mNormal = normal; };
	virtual void debugDraw();
	virtual void rotate(glm::vec3 axis, float degrees);
	virtual void translate(glm::vec3 dxyz) { mPosition += dxyz; };
	virtual void scale(glm::vec3 scale) {};
	virtual CollisionPlane* copy() const { return new CollisionPlane(mPosition, mNormal); };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_BOUNDS__