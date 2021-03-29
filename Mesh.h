#ifndef __EKH_SCRATCH_GRAPHICS_1_MESH__
#define __EKH_SCRATCH_GRAPHICS_1_MESH__

#include "Util/KDTree.h"
#include <vector>
#include <string>
#include <glm/common.hpp>

class Mesh {
	friend Mesh* makeIcosphere(float, unsigned int);
	friend Mesh* makeUVsphere(float, unsigned int, unsigned int);
	friend Mesh* makeCylinder(float, float, unsigned int);
private:
	Octree mVertexLookup;
	// I could add an array of attributes to mirror the idea of the VAO, or it could be time to standardize on what attributes actually get used
	Mesh() {};
	void addVertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex);
public:
	struct Vertex { glm::vec3 pos, norm, col; glm::vec2 tex; };
	struct Edge { unsigned int a, b; };
	struct Tri { unsigned int verts[3], edges[3]; };
	Mesh(std::string filename);
	~Mesh();
	void transfer();
	void render() const;
private:
	std::vector<Vertex> mVertices;
	std::vector<Edge> mEdges;
	std::vector<Tri> mTris;
};

Mesh* makeIcosphere(float radius, unsigned int subdiv = 0);
Mesh* makeUVsphere(float radius, unsigned int uRes = 32, unsigned int vRes = 16);
Mesh* makeCylinder(float radius, float height, unsigned int res = 16);

#endif//__EKH_SCRATCH_GRAPHICS_1_MESH__