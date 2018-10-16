#include "Geometry.h"
#include "ServiceLocator.h"
#include "GL/glew.h"
#include <cassert>
#include <fstream>
#include <string>

const unsigned int ATTRIB_SIZES[16] = {
	3,
	1,
	3,
	3,
	3,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	4,
	4,
	4,
	4
};

float* screenQuadData = new float[20] { -1.0f,-1.0f,0.0f, 0.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,0.0f, -1.0f,1.0f,0.0f, 0.0f,1.0f, 1.0f,1.0f,0.0f, 1.0f,1.0f };
float* unitQuadData = new float[8] { 0.0f,0.0f, 1.0f,0.0f, 0.0f,1.0f, 1.0f,1.0f };	// Position will be the same as texcoord, and I'm counting on whatever shader knowing this
unsigned int* Geometry::sQuadTris = new unsigned int[6] { 0,1,2, 2,1,3 };
Geometry Geometry::sScreenSpaceQuad = Geometry(4, screenQuadData, 2, sQuadTris, { A_POSITION,A_TEXCOORD0 });
Geometry Geometry::sUnitQuad = Geometry(4, unitQuadData, 2, sQuadTris, { A_TEXCOORD0 });

Geometry::Geometry() {
	mNumVerts = 0;
	mVertexData = NULL;
	mNumTris = 0;
	mTriData = NULL;
	mProperties = {};
	mHandles.good = false;
}

Geometry::Geometry(unsigned int numverts, float* vertexData, unsigned int numtris, unsigned int* triData, std::vector<ATTRIB_INDEX> properties) {
	mNumVerts = numverts;
	mVertexData = vertexData;
	mNumTris = numtris;
	mTriData = triData;
	mProperties = properties;
	mHandles.good = false;
	for (auto attrib : mProperties)
		mVertexSize += ATTRIB_SIZES[(int)attrib];
}

Geometry::Geometry(const char* filename) :Geometry() {
	// Load a .OBJ file into the internal Geometry format
	FileService& file = ServiceLocator::getFileService(filename);
	if (file.good()) {
		struct V3 { float x, y, z; };
		struct V2 { float x, y; };
		struct V { int pos, tex, norm; };
		struct Face { V vertices[4]; int numVerts; };	// Setting this array to <n> should make it support up to <n>-gons
		// First counting pass
		int numpos = 0, numnorm = 0, numtex = 0, numface = 0;
		while (file.good()) {
			if (file.extract("v \\S\\L", NULL)) numpos++;
			else if (file.extract("vn \\S\\L", NULL)) numnorm++;
			else if (file.extract("vt \\S\\L", NULL)) numtex++;
			else if (file.extract("f \\S\\L", NULL)) numface++;
			else if (file.extract("\\?S\\L", NULL));
		}
		// Second reading pass
		file.restart();
		int positer = 0, normiter = 0, texiter = 0, faceiter = 0;
		char* err;
		V3* positions = new V3[numpos];
		V3* normals = new V3[numnorm];
		V2* texcoords = new V2[numtex];
		Face* faces = new Face[numface];
		while (file.good()) {
			if (file.extract("v \\F \\F \\F\\L", &positions[positer])) positer++;
			else if (file.extract("vn \\F \\F \\F\\L", &normals[normiter])) normiter++;
			else if (file.extract("vt \\F \\F\\L", &texcoords[texiter])) texiter++;
			else if (file.extract("#\\S\\L", NULL));
			else if (file.extract("o \\S\\L", NULL));	// It's nice of you to offer me a name for the object, but I've got it covered
			else if (file.extract("s \\S\\L", NULL));	// No idea what that does, but it's part of the standard so I shouldn't throw an error
			else if (file.extract("mtllib \\S\\L", NULL));
			else if (file.extract("usemtl \\S\\L", NULL));
			else if (file.extract("f", NULL)) {
				// Assumes (possibly dangerously?) that all vertices have already been loaded
				int i = 0;
				while (file.extract(" \\?I/\\?I/\\?I", &faces[faceiter].vertices[i])) i++;
				faces[faceiter++].numVerts = i;
				mNumVerts += 3*i-2;
				mNumTris += i - 2;
				if (file.extract("\\?S\\L", &err)) {
					if (err) {
						ServiceLocator::getLoggingService().error("Something's fucky at the end of a face line", err);
						delete err;
					}
				}
			}
			else if (file.extract("\\?S\\L", &err)) {
				if (err) {
					ServiceLocator::getLoggingService().error("Unknown line in object file", err);
					delete err;
				}
			}
		}
		// Converting to internal format
		mProperties = { A_POSITION,A_TEXCOORD0,A_NORMAL };	// Won't always have texcoords
		for (auto prop : mProperties)
			mVertexSize += ATTRIB_SIZES[prop];
		mVertexData = new float[mNumVerts * mVertexSize];
		mTriData = new unsigned int[mNumTris * 3];
		int vertiter = 0, triiter = 0;
		for (int face = 0; face < numface; face++) {
			for (int j = 1; j < faces[face].numVerts - 1; j++) {
				mTriData[triiter++] = vertiter / mVertexSize;
				// Copy over vertex 0
				memcpy(&mVertexData[vertiter], &positions[faces[face].vertices[0].pos - 1], 3 * sizeof(float));
				vertiter += 3;
				memcpy(&mVertexData[vertiter], &texcoords[faces[face].vertices[0].tex - 1], 2 * sizeof(float));
				vertiter += 2;
				memcpy(&mVertexData[vertiter], &normals[faces[face].vertices[0].norm - 1], 3 * sizeof(float));
				vertiter += 3;
				for (int i = 0; i < 2; i++) {
					mTriData[triiter++] = vertiter / mVertexSize;
					// Copy over vertex j + i
					// Good opportunity for three memcpy's, or some pointer magic to assign a V2/3 to a float[]
					memcpy(&mVertexData[vertiter], &positions[faces[face].vertices[j + i].pos - 1], 3 * sizeof(float));
					vertiter += 3;
					memcpy(&mVertexData[vertiter], &texcoords[faces[face].vertices[j + i].tex - 1], 2 * sizeof(float));
					vertiter += 2;
					memcpy(&mVertexData[vertiter], &normals[faces[face].vertices[j + i].norm - 1], 3 * sizeof(float));
					vertiter += 3;
				}
			}
		}
		// Cleanup
		delete positions;
		delete normals;
		delete texcoords;
		delete faces;
		file.close();
	} else {
		ServiceLocator::getLoggingService().badFileError(filename);
	}
}

Geometry::~Geometry() {
	cleanup();
}

void Geometry::transfer() const {
	if (!mHandles.good) {
		glGenBuffers(2, (unsigned int*)&mHandles);
		mHandles.good = true;
		glBindBuffer(GL_ARRAY_BUFFER, mHandles.vboHandle);
		glBufferData(GL_ARRAY_BUFFER, mNumVerts * mVertexSize * sizeof(float), mVertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandles.indexHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumTris * 3 * sizeof(unsigned int), mTriData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &mHandles.vaoHandle);
		glBindVertexArray(mHandles.vaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, mHandles.vboHandle);
		unsigned int offset = 0;
		for (auto property : mProperties) {
			glEnableVertexAttribArray(property);	// I think this is the problem line that requires explicit location
			// This tells OpenGL how to get the data for a particular attribute out of the interleaved array we sent it earlier -
			// And I can tell it to pull out whatever data I want, however I want. If I made a Vertex struct and gave OpenGL the
			// array of Vertices, then stride would be sizeof(Vertex) and offset would be (&Vertex[0].attribute - &Vertex[0]). If
			// the layout of the Vertex struct matched the order of the inputs in the shader, there would be no need for explicit
			// locations.
			glVertexAttribPointer(property, ATTRIB_SIZES[property], GL_FLOAT, GL_FALSE, mVertexSize * sizeof(float), (char*)(offset * sizeof(float)));
			offset += ATTRIB_SIZES[property];
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Geometry::cleanup() {
	if (mVertexData) {
		delete[] mVertexData;
		mVertexData = NULL;
	}
	if (mTriData && mTriData != sQuadTris) {
		delete[] mTriData;
		mTriData = NULL;
	}
	if (mHandles.good) {
		glDeleteBuffers(2, (unsigned int*)&mHandles);
		glDeleteVertexArrays(1, &mHandles.vaoHandle);
	}
}

void Geometry::render() const {
	if (!mHandles.good)
		transfer();
	glBindVertexArray(mHandles.vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandles.indexHandle);
	glDrawElements(GL_TRIANGLES, 3 * mNumTris, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}