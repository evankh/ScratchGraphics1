#ifndef __EKH_SCRATCH_GRAPHICS_1_GEOMETRY__
#define __EKH_SCRATCH_GRAPHICS_1_GEOMETRY__

class AABB;

#include <vector>
#include <glm/matrix.hpp>

#include <string>

enum ATTRIB_INDEX
{
	A_POSITION,		// vertex location, point in space
	A_BLWT,			// irrelevant for now (bl. wt)
	A_NORMAL,		// normal vector
	A_COLOR0,		// colour
	A_COLOR1,		// alt colour
	A_FOG,			// irrelevant for now (fog)
	A_PTSZ,			// point size
	A_BLIND,		// irrelevant for now (bl. wt)
	A_TEXCOORD0,	// texcoord
	A_TEXCOORD1,	// "
	A_TEXCOORD2,	// "
	A_TEXCOORD3,	// "
	A_TEXCOORD4,	// "
	A_TEXCOORD5,	// "
	A_TEXCOORD6,	// "
	A_TEXCOORD7,	// "

	ATTRIB_COUNT	// 16
};
extern const unsigned int ATTRIB_SIZES[16];
class Vertex;

struct GeometryHandles {
	unsigned int vboHandle, indexHandle, vaoHandle;
	bool good;
};

class Geometry {
	static unsigned int* sQuadTris;
	static Geometry sScreenSpaceQuad;
	static Geometry sUnitQuad;
private:
	unsigned int mNumVerts = 0;
	float* mVertexData;
	unsigned int mNumTris = 0;
	unsigned int* mTriData;
	std::vector<ATTRIB_INDEX> mProperties;
	unsigned int mVertexSize = 0;
	mutable GeometryHandles mHandles;	// That's not cheating, is it? It makes sense that external code doesn't care whether the Geometry's been transferred or not, right?
	AABB* mBoundingBox;
public:
	Geometry() [[deprecated("probably")]];
	Geometry(unsigned int numverts, float* vertexData, unsigned int numtris, unsigned int* triData, std::vector<ATTRIB_INDEX> properties) [[deprecated("probably")]];
	Geometry(std::string filename);
	~Geometry();
	void transfer() const;
	void cleanup();
	void render() const;
	AABB* getBoundingBox() const { return mBoundingBox; };
	static const Geometry* getScreenQuad() { return &sScreenSpaceQuad; };
	static const Geometry* getUnitQuad() { return &sUnitQuad; };
	static Geometry* getNewQuad();
	static void drawUnitSphere();
	static void drawUnitCylinder();
	static void drawBox(glm::vec3 min, glm::vec3 max);
	static void drawUnitBox();
	static void drawAxes();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GEOMETRY__