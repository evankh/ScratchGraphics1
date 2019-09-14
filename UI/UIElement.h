#ifndef __EKH_SCRATCH_GRAPHICS_1_UI_ELEMENT__
#define __EKH_SCRATCH_GRAPHICS_1_UI_ELEMENT__

#include <string>
#include <vector>
class GeometryComponent;
class Texture;
class Program;

struct Bounds2D {
	int top, bottom, left, right;
	int layer;
};

class Element {
	static const GeometryComponent* sUnitQuad;
	static const Program* sScreenDraw;
protected:
	Bounds2D mBounds;
public:
	Element(Texture* texture);
	~Element() {};
	void draw();	// Should it be virtual? not sure
	virtual void layout(int left, int right, int top, int bottom) = 0;
};

class RootElement :public Element {
private:
	std::vector<Element*> mChildElements;
public:
	RootElement(std::string filename);
	virtual void layout(int left, int right, int top, int bottom);
};

class FrameElement :public Element {
private:
	std::vector<Element*> mChildElements;
	int padding;

public:
	FrameElement();
	virtual void layout(int left, int right, int top, int bottom);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_UI_ELEMENT__