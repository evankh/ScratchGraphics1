#include "Geometry.h"
//#include "Program.h"
//#include "Camera.h"
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
		struct Face { V vertices[4]; };
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
				while (file.extract(" \\?I/\\?I/\\?I", &faces[faceiter].vertices[i]))
					i++;
				faceiter++;
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
		mNumVerts = numface * 6;	// Only valid for all quads  - when we're doing absolutely zero dupe checking - needs more robustness desperately
		mVertexData = new float[mNumVerts * mVertexSize];
		mNumTris = numface * 2;	// Only valid for all quads
		mTriData = new unsigned int[mNumTris * 3];
		int vertiter = 0, triiter = 0;
		for (int face = 0; face < numface; face++) {
			int triangulate[6] = { 0,1,2,0,2,3 };
			for (int j = 0; j < 6; j++) {
				int vert = triangulate[j];
				mTriData[triiter++] = vertiter / mVertexSize;
				// Good opportunity for three memcpy's, or some pointer magic to assign a V2/3 to a float[]
				mVertexData[vertiter++] = positions[faces[face].vertices[vert].pos - 1].x;
				mVertexData[vertiter++] = positions[faces[face].vertices[vert].pos - 1].y;
				mVertexData[vertiter++] = positions[faces[face].vertices[vert].pos - 1].z;
				mVertexData[vertiter++] = texcoords[faces[face].vertices[vert].tex - 1].x;
				mVertexData[vertiter++] = texcoords[faces[face].vertices[vert].tex - 1].y;
				mVertexData[vertiter++] = normals[faces[face].vertices[vert].norm - 1].x;
				mVertexData[vertiter++] = normals[faces[face].vertices[vert].norm - 1].y;
				mVertexData[vertiter++] = normals[faces[face].vertices[vert].norm - 1].z;
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
	// Congratulations, I have a Stupid Version (TM)! It can get more fleshed-out in the future, but for now it appears to work
}

/*Geometry::Geometry(std::vector<Vertex> vertexData, unsigned int numtris, unsigned int* triData, std::vector<ATTRIB_INDEX> properties) {
	mNumVerts = vertexData.size();
	mVertexData = (float*)vertexData.data();
	mNumTris = numtris;
	mTriData = triData;
	mProperties = properties;
	mHandles = NULL;
	mDisplay = NULL;
}*/

Geometry::~Geometry() {
	cleanup();
}

void Geometry::transfer() {
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
			glEnableVertexAttribArray(property);
			glVertexAttribPointer(property, ATTRIB_SIZES[property], GL_FLOAT, GL_FALSE, mVertexSize * sizeof(float), (char*)(offset * sizeof(float)));
			offset += ATTRIB_SIZES[property];
		}
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void Geometry::cleanup() {
	// Objects won't be managing their own CPU-side data, so that we can give the same geometry to multiple objects
	// Actually, should they ever be? I'm not certain. It seems like we might often want to reuse geometry data , i.e. for particles
	/*if (mVertexData) {
		delete[] mVertexData;
		mVertexData = NULL;
	}
	if (mTriData) {
		delete[] mTriData;
		mTriData = NULL;
	}*/
	if (mHandles.good) {
		glDeleteBuffers(2, (unsigned int*)&mHandles);
		glDeleteVertexArrays(1, &mHandles.vaoHandle);
	}
}

//#include "glm/gtc/type_ptr.hpp"

void Geometry::render() {
	/*if (mDisplay) {
		glUseProgram(mDisplay->mHandle);
		//c->transfer();
		glm::mat4 mvp = c->mViewProjectionMatrix * mModelMatrix;
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));
	}*/
	glBindVertexArray(mHandles.vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandles.indexHandle);
	glDrawElements(GL_TRIANGLES, 3 * mNumTris, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}