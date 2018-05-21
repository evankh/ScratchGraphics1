#include "Geometry.h"
#include "Program.h"
#include "Camera.h"
#include "GL/glew.h"
#include <cassert>
#include <fstream>

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

Geometry::Geometry(const char* filename) {
	// Load a .OBJ file into the internal Geometry format
	std::ifstream in(filename);
	if (in.good()) {

	}
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
	glDeleteBuffers(2, (unsigned int*)&mHandles);
	glDeleteVertexArrays(1, &mHandles.vaoHandle);
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
	// For now, objects won't be managing their own data, so that we can give the same geometry to multiple objects
	// Actually, should they ever be? I'm not certain. It seems like we might often want to reuse geometry data , i.e. for particles
	/*if (mVertexData) {
		delete[] mVertexData;
		mVertexData = NULL;
	}
	if (mTriData) {
		delete[] mTriData;
		mTriData = NULL;
	}*/
}

#include "glm/gtc/type_ptr.hpp"

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