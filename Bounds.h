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
	BOX
};

class Bounds {
	friend bool collides(Bounds*, Bounds*);
protected:
	BoundsType mType;
	virtual bool collides(Bounds*) { return false; };
	Bounds(BoundsType type) :mType(type) {};
public:
	virtual void debugDraw() = 0;
};

class Collisionless :public Bounds {
	virtual bool collides(Bounds*) { return false; };
	virtual void debugDraw() { return; };
public:
	Collisionless() :Bounds(BoundsType::COLLISIONLESS) {};
};

class BoundingSphere :public Bounds {
	friend class AABoundingCylinder;
	friend class AABB;
	friend class ArbitraryBoundingCylinder;
	friend class ArbitraryBoundingBox;
private:
	glm::vec3 mCenter;
	float mRadius;
public:
	//bool collides(Bounds* b) { return ::collides(this, b); };
	bool collides(BoundingSphere*);
	bool collides(AABoundingCylinder*);
	bool collides(AABB*);
	bool collides(ArbitraryBoundingCylinder*);
	bool collides(ArbitraryBoundingBox*);
	virtual void debugDraw();
	BoundingSphere(glm::vec3 center, float radius) :Bounds(BoundsType::SPHERE) { mCenter = center; mRadius = radius; };
};

class AABoundingCylinder :public Bounds {
	friend class BoundingSphere;
	friend class AABB;
	friend class ArbitraryBoundingCylinder;
	friend class ArbitraryBoundingBox;
private:
	glm::vec3 mCenter;
	float mRadius, mHeight;
public:
	//bool collides(Bounds* b) { return ::collides(this, b); };
	bool collides(AABoundingCylinder*);
	bool collides(AABB*);
	bool collides(ArbitraryBoundingCylinder*);
	bool collides(ArbitraryBoundingBox*);
	virtual void debugDraw();
	AABoundingCylinder() :Bounds(BoundsType::AA_CYLINDER) {};
};

class AABB :public Bounds {
	friend class BoundingSphere;
	friend class AABoundingCylinder;
	friend class ArbitraryBoundingCylinder;
	friend class ArbitraryBoundingBox;
private:
	glm::vec3 mMin, mMax;
public:
	//bool collides(Bounds* b) { return ::collides(this, b); };
	bool collides(AABB*);
	bool collides(ArbitraryBoundingCylinder*);
	bool collides(ArbitraryBoundingBox*);
	virtual void debugDraw();
	AABB() :Bounds(BoundsType::AA_BOX) {};
	void update(float vert[3]);
	void setMinX(float x) { mMin.x = x; };
	void setMinY(float y) { mMin.y = y; };
	void setMinZ(float z) { mMin.z = z; };
	void setMaxX(float x) { mMin.x = x; };
	void setMaxY(float y) { mMin.y = y; };
	void setMaxZ(float z) { mMin.z = z; };
};

class ArbitraryBoundingCylinder :public Bounds {
	friend class BoundingSphere;
	friend class AABoundingCylinder;
	friend class AABB;
	friend class ArbitraryBoundingBox;
private:
	AABoundingCylinder mCylinder;
	glm::mat4 mTransform;
public:
	//bool collides(Bounds* b) { return ::collides(this, b); };
	bool collides(ArbitraryBoundingCylinder*);
	bool collides(ArbitraryBoundingBox*);
	virtual void debugDraw();
	ArbitraryBoundingCylinder() :Bounds(BoundsType::CYLINDER) {};
};

class ArbitraryBoundingBox :public Bounds {
	friend class BoundingSphere;
	friend class AABoundingCylinder;
	friend class AABB;
	friend class ArbitraryBoundingCylinder;
private:
	AABB mBox;
	glm::mat4 mTransform;
public:
	//bool collides(Bounds* b) { return ::collides(this, b); };
	bool collides(ArbitraryBoundingBox*);
	virtual void debugDraw();
	ArbitraryBoundingBox() :Bounds(BoundsType::BOX) {};
};

#endif//__EKH_SCRATCH_GRAPHICS_1_BOUNDS__