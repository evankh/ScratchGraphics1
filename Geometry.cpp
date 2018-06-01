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
	// Good idea to wrap file reads in a Service like with writes?
	std::ifstream in(filename);
	if (in.good()) {
		// First pass: find number of vertices
		bool hasNormals = false, hasTexcoords = false;
		std::string buffer;
		for (std::getline(in, buffer); !in.eof(); std::getline(in, buffer)) {
			if (buffer.substr(0, 2) == "v ")
				mNumVerts++;
			if (buffer.substr(0, 2) == "vn")
				hasNormals = true;
			if (buffer.substr(0, 2) == "vt")
				hasTexcoords = true;
			if (buffer.substr(0, 2) == "f ")
				mNumTris++;
		}
		in.seekg(0, SEEK_SET);
		float* allPositionData = new float[mNumVerts * 3];
		int posIter = 0, normIter = 0, texIter = 0;
		float* allNormalData = NULL;
		if (hasNormals) allNormalData = new float[mNumVerts * 3];
		float* allTexcoordData = NULL;
		if (hasTexcoords) allTexcoordData = new float[mNumVerts * 2];
		for (int lineNumber = 0; !in.eof(); lineNumber++) {
			// Second pass: load data
			/*char type = in.get();
			switch (type) {
			case '#':	// Comment
				break;
			case 'v':	// Vertex
				char subtype = in.get();
				switch (subtype) {
				case ' ':
					std::str
					break;
				case 't':
					break;
				case 'n':
					break;
				default:
					ServiceLocator::getLoggingService().error("Unexpected character on line " + line, subtype);
					break;
				}
				break;
			case 'f':	// Face
				break;
			default:
				ServiceLocator::getLoggingService().error("Unexpected character on line " + line, type);
				break;
			}*/

			std::string token;
			std::getline(in, token, ' ');
			in.ignore(1, ' ');
			if (token == "v") {
				// There will be 3 floats here
				in >> allPositionData[posIter++];
				in >> allPositionData[posIter++];
				in >> allPositionData[posIter++];
			} else if (token == "vn") {
				// There will be 3 floats here
				in >> allNormalData[normIter++];
				in >> allNormalData[normIter++];
				in >> allNormalData[normIter++];
			} else if (token == "vt") {
				// There will be 3 floats here
				in >> allTexcoordData[texIter++];
				in >> allTexcoordData[texIter++];
			} else if (token == "f") {
				// Format: f v/vt/vn v/vt/vn v/vt/vn [v/vt/vn]
				std::string vertex;
				for (int v = 0; v < 4; v++) {
					std::getline(in, vertex, ' ');
					int vert, tex, norm;
					in >> vert;
					in.ignore(1, '/');
					if (hasTexcoords)
						in >> tex;
					in.ignore(1, '/');
					if (hasNormals)
						in >> norm;
					// Fuck it, I need more sleep for this.
				}
			} else {
				ServiceLocator::getLoggingService().error("Unexpected token at line" + lineNumber, token);
			}
		}
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