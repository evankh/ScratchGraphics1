#ifndef __EKH_SCRATCH_GRAPHICS_1_GEOMETRY_COMPONENT__
#define __EKH_SCRATCH_GRAPHICS_1_GEOMETRY_COMPONENT__

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
	bool good = false;
};

class GeometryComponent {
	static unsigned int* sQuadTris;
	static GeometryComponent sScreenSpaceQuad;
	static GeometryComponent sUnitQuad;
	friend GeometryComponent* subdivide(const GeometryComponent*);
private:
	unsigned int mNumVerts = 0;
	float* mVertexData = nullptr;
	unsigned int mNumTris = 0;
	unsigned int mNumQuads = 0;
	unsigned int mNumPatches = 0;
	unsigned int* mAllIndexData = nullptr;
	unsigned int* mTriData = nullptr;
	unsigned int* mQuadData = nullptr;
	unsigned int* mPatchData = nullptr;
	std::vector<ATTRIB_INDEX> mProperties;
	unsigned int mVertexSize = 0;
	mutable GeometryHandles mHandles;	// That's not cheating, is it? It makes sense that external code doesn't care whether the Geometry's been transferred or not, right?
	AABB* mBoundingBox = nullptr;
public:
	GeometryComponent(unsigned int numverts, float* vertexData, unsigned int numtris, unsigned int* triData, std::vector<ATTRIB_INDEX> properties);
	GeometryComponent(std::string filename);
	~GeometryComponent();
	void transfer() const;
	void cleanup();
	void render() const;
	AABB* getBoundingBox() const { return mBoundingBox; };
	static const GeometryComponent* getScreenQuad() { return &sScreenSpaceQuad; };
	static const GeometryComponent* getUnitQuad() { return &sUnitQuad; };
	static GeometryComponent* getNewQuad();
	static GeometryComponent* getNewIcosahedron(unsigned int subdiv = 0);	// These really need to be non-member functions
	static void drawUnitQuad();
	static void drawCenteredQuad();
	static void drawUnitSphere();
	static void drawSphere(glm::vec3 center, float radius);	// This thing should go; only used in drawing collision boxes
	static void drawUnitCylinder();
	static void drawUnitBox();
	static void drawBox(glm::vec3 min, glm::vec3 max);
	static void drawAxes();
};

GeometryComponent* subdivide(const GeometryComponent* geom);

#endif//__EKH_SCRATCH_GRAPHICS_1_GEOMETRY_COMPONENT__