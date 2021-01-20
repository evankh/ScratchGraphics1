#include "Components/Geometry.h"
#include "Components/Collision.h"
#include "Util/ServiceLocator.h"
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
unsigned int* GeometryComponent::sQuadTris = new unsigned int[6] { 0,1,2, 2,1,3 };
GeometryComponent GeometryComponent::sScreenSpaceQuad = GeometryComponent(4, screenQuadData, 2, sQuadTris, { A_POSITION,A_TEXCOORD0 });
GeometryComponent GeometryComponent::sUnitQuad = GeometryComponent(4, unitQuadData, 2, sQuadTris, { A_TEXCOORD0 });

GeometryComponent::GeometryComponent(unsigned int numverts, float* vertexData, unsigned int numtris, unsigned int* triData, std::vector<ATTRIB_INDEX> properties) {
	mNumVerts = numverts;
	mVertexData = vertexData;
	mNumTris = numtris;
	mTriData = triData;
	mProperties = properties;
	mHandles.good = false;
	for (auto attrib : mProperties)
		mVertexSize += ATTRIB_SIZES[(int)attrib];
	mBoundingBox = NULL;
}

GeometryComponent::GeometryComponent(std::string filename) {
	// Load a .OBJ file into the internal Geometry format
	FileService file(filename);
	if (file.good()) {
		struct V3 { float x, y, z; };
		struct V2 { float x, y; };
		struct V { int pos, tex, norm; };
		struct Face { V vertices[4]; int numVerts; };	// Setting this array to <n> should make it support up to <n>-gons
		// First counting pass
		int numpos = 0, numnorm = 0, numtex = 0, numface = 0;
		while (file.good()) {
			if (file.extract("v `S`L")) numpos++;
			else if (file.extract("vn `S`L")) numnorm++;
			else if (file.extract("vt `S`L")) numtex++;
			else if (file.extract("f `S`L")) numface++;
			else if (file.extract("`?S`L"));
		}
		// Second reading pass
		file.restart();
		int positer = 0, normiter = 0, texiter = 0, faceiter = 0;
		char* err;
		mBoundingBox = new AABB();
		V3* positions = new V3[numpos];
		V3* normals = new V3[numnorm];
		V2* texcoords = new V2[numtex];
		Face* faces = new Face[numface];
		while (file.good()) {
			if (file.extract("v `F `F `F`L", &positions[positer])) {
				// The BB update could/should offer some sort of symmetry around X/Y (box) or all (sphere) axes
				mBoundingBox->update((float*)&positions[positer]);
				positer++;
			}
			else if (file.extract("vn `F `F `F`L", &normals[normiter])) normiter++;
			else if (file.extract("vt `F `F`L", &texcoords[texiter])) texiter++;
			else if (file.extract("#`S`L"));
			else if (file.extract("o `S`L"));	// It's nice of you to offer me a name for the object, but I've got it covered
			else if (file.extract("s `S`L"));	// No idea what that does, but it's part of the standard so I shouldn't throw an error
			else if (file.extract("mtllib `S`L"));
			else if (file.extract("usemtl `S`L"));
			else if (file.extract("f")) {
				// Assumes (possibly dangerously?) that all vertices have already been loaded
				int i = 0;
				while (file.extract(" `?I/`?I/`?I", &faces[faceiter].vertices[i])) i++;
				faces[faceiter++].numVerts = i;
				mNumVerts += 3*i-2;
				mNumTris += i - 2;
				if (file.extract("`?S`L", &err)) {
					if (err) {
						ServiceLocator::getLoggingService().error("Something's fucky at the end of a face line", err);
						delete err;
					}
				}
			}
			else if (file.extract("`?S`L", &err)) {
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
	} else {
		throw std::exception(filename.data());
	}
}

GeometryComponent::~GeometryComponent() {
	cleanup();
	if (mBoundingBox) delete mBoundingBox;
}

void GeometryComponent::transfer() const {
	if (!mHandles.good) {
		glGenBuffers(2, (unsigned int*)&mHandles);
		mHandles.good = true;
		glBindBuffer(GL_ARRAY_BUFFER, mHandles.vboHandle);
		glBufferData(GL_ARRAY_BUFFER, mNumVerts * mVertexSize * sizeof(float), mVertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandles.indexHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumTris * 3 * sizeof(unsigned int), mTriData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		/*glNamedBufferData(mHandles.vboHandle, mNumVerts*mVertexSize*sizeof(float), mVertexData, GL_STATIC_DRAW);
		glNamedBufferData(mHandles.indexHandle, mNumTris * 3 * sizeof(unsigned int), mTriData, GL_STATIC_DRAW);*/

		glGenVertexArrays(1, &mHandles.vaoHandle);
		glBindVertexArray(mHandles.vaoHandle);
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

void GeometryComponent::cleanup() {
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

void GeometryComponent::render() const {
	if (!mHandles.good)
		transfer();
	glBindVertexArray(mHandles.vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandles.indexHandle);
	glDrawElements(GL_TRIANGLES, 3 * mNumTris, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GeometryComponent::render_patches() const {
	if (!mHandles.good)
		transfer();
	glBindVertexArray(mHandles.vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mHandles.indexHandle);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElements(GL_PATCHES, 3 * mNumTris, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#ifndef PI
#define PI 3.141592f
#endif//PI

GeometryComponent* GeometryComponent::getNewQuad() {
	// There must be some smarter way to make it know that texcoords are going to be the same as positions
	float* positions = new float[32] {
		0.0,0.0,0.0, 0.0,0.0, 0.0,0.0,1.0,
		1.0,0.0,0.0, 1.0,0.0, 0.0,0.0,1.0,
		0.0,1.0,0.0, 0.0,1.0, 0.0,0.0,1.0,
		1.0,1.0,0.0, 1.0,1.0, 0.0,0.0,1.0
	};
	unsigned int* tris = new unsigned int[6] { 0,1,2, 2,1,3 };
	return new GeometryComponent(4, positions, 2, tris, { A_POSITION, A_TEXCOORD0, A_NORMAL });
}

GeometryComponent* GeometryComponent::getNewIcosahedron() {
	float* positions = new float[3 * 12 * 2];

	float longitude = 0.0f;
	float latitude = PI / 2.0f;
	positions[0] = sinf(longitude) * cosf(latitude);
	positions[1] = cosf(longitude) * cosf(latitude);
	positions[2] = sinf(latitude);

	latitude = atanf(0.5);
	for (int i = 1; i <= 5; i++) {
		longitude = (float)i * PI * 2.0f / 5.0f;
		positions[3 * i + 0] = sinf(longitude) * cosf(latitude);
		positions[3 * i + 1] = cosf(longitude) * cosf(latitude);
		positions[3 * i + 2] = sinf(latitude);
	}

	latitude = -atanf(0.5);
	for (int i = 6; i <= 10; i++) {
		longitude = (float)i * PI * 2.0f / 5.0f + PI / 5.0f;
		positions[3 * i + 0] = sinf(longitude) * cosf(latitude);
		positions[3 * i + 1] = cosf(longitude) * cosf(latitude);
		positions[3 * i + 2] = sinf(latitude);
	}

	latitude = -PI / 2.0f;
	positions[33] = sinf(longitude) * cosf(latitude);
	positions[34] = cosf(longitude) * cosf(latitude);
	positions[35] = sinf(latitude);

	for (int i = 3*12*2-6; i >= 0; i -= 6) {
		for (int j = 0; j < 6; j++)
			positions[i + j] = positions[i / 2 + j % 3];
	}

	// This could be done a whole lot easier with 2 trifans and a tristrip, but it's not set up to handle that. It probably should be.
	unsigned int* tris = new unsigned int[3 * 20];
	for (int i = 0; i < 5; i++) {
		tris[3*i] = 0;
		tris[3 * i + 1] = i + 1;
		tris[3 * i + 2] = i + 2;
	}
	tris[14] = 1;
	for (int i = 0; i < 5; i++) {
		tris[6 * i + 15] = i + 1;
		tris[6 * i + 16] = i + 6;
		tris[6 * i + 17] = i + 2;

		tris[6 * i + 18] = i + 1;
		tris[6 * i + 19] = i + 5;
		tris[6 * i + 20] = i + 6;
	}
	tris[41] = 1;
	tris[19] = 10;
	for (int i = 0; i < 5; i++) {
		tris[3 * i + 45] = i + 6;
		tris[3 * i + 46] = 11;
		tris[3 * i + 47] = i + 7;
	}
	tris[59] = 6;

	return new GeometryComponent(12, positions, 20, tris, { A_POSITION, A_NORMAL });
}

void GeometryComponent::drawUnitQuad() {
	sUnitQuad.transfer();
	sUnitQuad.render();
}

void GeometryComponent::drawCenteredQuad() {
	sScreenSpaceQuad.transfer();
	sScreenSpaceQuad.render();
}

void GeometryComponent::drawUnitSphere() {
	drawSphere({ 0.0f, 0.0f, 0.0f }, 1.0f);
}

void GeometryComponent::drawSphere(glm::vec3 center, float radius) {
	GeometryHandles handles;
	int numSegments = 16, numStacks = 8;
	int fansize = numSegments + 2;
	int stripsize = 2 * numSegments + 2;
	int off1 = 0 * (stripsize)+fansize;
	int off2 = 1 * (stripsize)+fansize;
	int off3 = (numStacks - 2)*(stripsize)+fansize;
	if (!handles.good) {
		// Create & transfer
		int numVerts = 2 + numSegments * (numStacks - 1);
		int numIndices = 2 * numStacks*numSegments + 2 * numStacks - 2 * numSegments;
		float* vertexData = new float[3 * numVerts];
		unsigned int* indexData = new unsigned int[numIndices];
		int vertiter = 0, indexiter = 0;
		vertexData[vertiter++] = 0.0f;
		vertexData[vertiter++] = 0.0f;
		vertexData[vertiter++] = 1.0f;
		for (int stack = 1; stack < numStacks; stack++) {
			for (int seg = 0; seg < numSegments; seg++) {
				float theta = (float)seg / numSegments * 2 * PI;
				float phi = PI / 2 - (float)stack / numStacks * PI;
				vertexData[vertiter++] = cosf(theta)*cosf(phi);
				vertexData[vertiter++] = sinf(theta)*cosf(phi);
				vertexData[vertiter++] = sinf(phi);
			}
		}
		vertexData[vertiter++] = 0.0f;
		vertexData[vertiter++] = 0.0f;
		vertexData[vertiter++] = -1.0f;
		for (int i = 0; i < vertiter;) {
			vertexData[i++] = vertexData[i] * radius + center.x;
			vertexData[i++] = vertexData[i] * radius + center.y;
			vertexData[i++] = vertexData[i] * radius + center.z;
		}
		for (int i = 0; i <= numSegments; i++)
			indexData[indexiter++] = i;
		indexData[indexiter++] = 1;
		for (int stack = 0; stack < numStacks - 2; stack++) {
			for (int seg = 1; seg <= numSegments; seg++) {
				indexData[indexiter++] = stack*numSegments + seg;
				indexData[indexiter++] = stack*numSegments + seg + numSegments;
			}
			indexData[indexiter++] = stack*numSegments + 1;
			indexData[indexiter++] = stack*numSegments + numSegments + 1;
		}
		indexData[indexiter++] = (numStacks - 1)*numSegments + 1;
		indexData[indexiter++] = (numStacks - 2)*numSegments + 1;
		for (int i = numSegments; i >= 1; i--)
			indexData[indexiter++] = (numStacks - 2)*numSegments + i;
		// Now transfer
		glGenBuffers(2, (unsigned int*)&handles);
		//glNamedBufferData(handles.vboHandle, numVerts * 3 * sizeof(float), vertexData, GL_STATIC_DRAW);
		//glNamedBufferData(handles.indexHandle, numIndices * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, handles.vboHandle);
		glBufferData(GL_ARRAY_BUFFER, numVerts * 3 * sizeof(float), vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles.indexHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(unsigned int), indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &handles.vaoHandle);
		handles.good = true;
		glBindVertexArray(handles.vaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, handles.vboHandle);
		glEnableVertexAttribArray(A_POSITION);
		glVertexAttribPointer(A_POSITION, ATTRIB_SIZES[A_POSITION], GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		delete[] vertexData;
		delete[] indexData;
	}
	// Proceed to draw
	// Create & transfer a uniform matrix containing the size & location information...

	// Oh sure, let's just throw a bunch of random rendering into the geometry file! Encapsulation, what is that?
	glBindVertexArray(handles.vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles.indexHandle);
	glDrawElements(GL_TRIANGLE_FAN, fansize, GL_UNSIGNED_INT, 0);
	for (int i = 0; i < numStacks - 2; i++)
		glDrawElements(GL_TRIANGLE_STRIP, stripsize, GL_UNSIGNED_INT, (void*)((i*(stripsize) + fansize)*sizeof(unsigned int)));
	glDrawElements(GL_TRIANGLE_FAN, fansize, GL_UNSIGNED_INT, (void*)(off3*sizeof(unsigned int)));
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(2, (unsigned int*)&handles);
	glDeleteVertexArrays(1, &handles.vaoHandle);
}

void GeometryComponent::drawUnitBox() {
	static float* vertexData = new float[24]{ 0.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f, 1.0f,0.0f,1.0f, 0.0f,1.0f,1.0f, 1.0f,1.0f,1.0f };
	static unsigned int* triData = new unsigned int[36]{ 0,2,1, 1,2,3, 0,1,4, 4,1,5, 0,4,2, 2,4,6, 4,5,6, 6,5,7, 1,3,5, 5,3,7, 3,2,7, 7,2,6 };
	static GeometryComponent box(8, vertexData, 12, triData, { A_POSITION });
	box.transfer();
	box.render();
}

void GeometryComponent::drawBox(glm::vec3 min, glm::vec3 max) {
	float* vertexData = new float[24]{ min.x,min.y,min.z, max.x,min.y,min.z, min.x,max.y,min.z, max.x,max.y,min.z,
		min.x,min.y,max.z, max.x,min.y,max.z, min.x,max.y,max.z, max.x,max.y,max.z };
	unsigned int* triData = new unsigned int[36]{ 0,2,1, 1,2,3, 0,1,4, 4,1,5, 0,4,2, 2,4,6, 4,5,6, 6,5,7, 1,3,5, 5,3,7, 3,2,7, 7,2,6 };
	GeometryComponent box(8, vertexData, 12, triData, { A_POSITION });
	box.transfer();
	box.render();
	box.cleanup();
}

void GeometryComponent::drawUnitCylinder() {
	static GeometryHandles handles;
	int resolution = 16;
	if (!handles.good) {
		float* vertexData = new float[2 * 3 * resolution];
		unsigned int* indexData = new unsigned int[4 * resolution + 2];
		int vertiter = 0, indexiter = 0;
		for (float z = 1.0f; z > -2.0f; z -= 2.0f) {
			for (int i = 0; i < resolution; i++) {
				vertexData[vertiter++] = cosf((float)i / resolution * 2 * PI);
				vertexData[vertiter++] = sinf((float)i / resolution * 2 * PI);
				vertexData[vertiter++] = z;
			}
		}
		for (int i = 0; i < resolution; i++)
			indexData[indexiter++] = i;
		for (int i = 0; i < resolution; i++) {
			indexData[indexiter++] = i;
			indexData[indexiter++] = resolution + i;
		}
		indexData[indexiter++] = 0;
		indexData[indexiter++] = resolution;
		for (int i = 0; i < resolution; i++)
			indexData[indexiter++] = 2 * resolution - i - 1;

		glGenBuffers(2, (unsigned int*)&handles);
		//glNamedBufferData(handles.vboHandle, numVerts * 3 * sizeof(float), vertexData, GL_STATIC_DRAW);
		//glNamedBufferData(handles.indexHandle, numIndices * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, handles.vboHandle);
		glBufferData(GL_ARRAY_BUFFER, 2 * 3 * resolution * sizeof(float), vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles.indexHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (4 * resolution + 2) * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &handles.vaoHandle);
		handles.good = true;
		glBindVertexArray(handles.vaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, handles.vboHandle);
		glEnableVertexAttribArray(A_POSITION);
		glVertexAttribPointer(A_POSITION, ATTRIB_SIZES[A_POSITION], GL_FLOAT, GL_FALSE, ATTRIB_SIZES[A_POSITION] * sizeof(float), 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		delete[] vertexData;
	}
	glBindVertexArray(handles.vaoHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handles.indexHandle);
	glDrawElements(GL_TRIANGLE_FAN, resolution, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_POINTS, 0, 2 * resolution);
	glDrawElements(GL_TRIANGLE_STRIP, 2 * resolution + 2, GL_UNSIGNED_INT, (void*)(resolution*sizeof(unsigned int)));
	glDrawElements(GL_TRIANGLE_FAN, resolution, GL_UNSIGNED_INT, (unsigned int*)((3 * resolution + 2)*sizeof(unsigned int)));
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GeometryComponent::drawAxes() {
	static GeometryHandles handles;
	if (!handles.good) {
		float* vertexData = new float[36]{ 0,0,0, 1,0,0, 1,0,0, 1,0,0, 0,0,0, 0,1,0, 0,1,0, 0,1,0, 0,0,0, 0,0,1, 0,0,1, 0,0,1 };
		glGenBuffers(1, &handles.vboHandle);
		glBindBuffer(GL_ARRAY_BUFFER, handles.vboHandle);
		glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), vertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_VERTEX_ARRAY, 0);

		glGenVertexArrays(1, &handles.vaoHandle);
		handles.good = true;
		glBindVertexArray(handles.vaoHandle);
		glBindBuffer(GL_VERTEX_ARRAY, handles.vboHandle);
		glEnableVertexAttribArray(A_POSITION);
		glEnableVertexAttribArray(A_COLOR0);
		glVertexAttribPointer(A_POSITION, ATTRIB_SIZES[A_POSITION], GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glVertexAttribPointer(A_COLOR0, ATTRIB_SIZES[A_COLOR0], GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(ATTRIB_SIZES[A_POSITION]*sizeof(float)));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		delete[] vertexData;
	}
	glBindVertexArray(handles.vaoHandle);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);
}

GeometryComponent* subdivide(const GeometryComponent* geom) {
	unsigned int numNewVerts = 2 * geom->mNumVerts;
	unsigned int numNewTris = 4 * geom->mNumTris;
	float* newVerts = new float[geom->mVertexSize * numNewVerts];
	unsigned int* newTris = new unsigned int[3 * numNewTris];
	for (int i = 0; i < geom->mNumTris; i += 3) {
		auto v1 = geom->mTriData[i];
		auto v2 = geom->mTriData[i + 1];
		auto v3 = geom->mTriData[i + 2];
		
	}
	return nullptr;
}
