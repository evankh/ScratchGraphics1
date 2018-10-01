#ifndef __EKH_SCRATCH_GRAPHICS_1_GEOMETRY__
#define __EKH_SCRATCH_GRAPHICS_1_GEOMETRY__

#include <vector>
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
private:
	unsigned int mNumVerts = 0;
	float* mVertexData;
	unsigned int mNumTris = 0;
	unsigned int* mTriData;
	std::vector<ATTRIB_INDEX> mProperties;
	unsigned int mVertexSize = 0;
	GeometryHandles mHandles;
public:
	Geometry();
	Geometry(unsigned int numverts, float* vertexData, unsigned int numtris, unsigned int* triData, std::vector<ATTRIB_INDEX> properties);
	Geometry(const char* filename);
	//Geometry(std::vector<Vertex> vertexData, unsigned int numtris, unsigned int* triData, std::vector<ATTRIB_INDEX> properties);
	~Geometry();
	void transfer();
	void cleanup();
	void render();
	//void setDisplay(Program* display);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GEOMETRY__