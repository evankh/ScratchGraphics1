#include "Mesh.h"

#ifndef PI
#define PI 3.141592f
#endif//PI

Mesh* makeIcosphere(float radius, unsigned int subdiv) {
	Mesh* sphere = new Mesh;
	// Build an Icosahedron with no subdivision
	sphere->addVertex(glm::vec3(0.0f, 0.0f, radius), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f));
	float latitude = atanf(0.5f), longitude;
	for (unsigned int i = 0; i < 5;i++){
		longitude = (float)i * PI * 2.0f / 5.0f;
		glm::vec3 pos = glm::vec3(cosf(longitude) * cosf(latitude), sinf(longitude) * cosf(latitude), sinf(latitude));
		sphere->addVertex(radius * pos, pos, glm::vec2(latitude, longitude));
	}
	latitude = atanf(-0.5f);
	for (unsigned int i = 0; i < 5; i++) {
		longitude = (float)i * PI * 2.0f / 5.0f + PI / 5.0f;
		glm::vec3 pos = glm::vec3(cosf(longitude) * cosf(latitude), sinf(longitude) * cosf(latitude), sinf(latitude));
		sphere->addVertex(radius * pos, pos, glm::vec2(latitude, longitude));
	}
	sphere->addVertex(glm::vec3(0.0f, 0.0f, -radius), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, -1.0f));

	for (unsigned int i = 1; i <= 5; i++) {	// Edges & Faces for the top cap
		sphere->mEdges.push_back(Mesh::Edge{ 0, i });	// upper spokes
		sphere->mEdges.push_back(Mesh::Edge{ i, (i%5)+1 });	// upper ring
		sphere->mTris.push_back(Mesh::Tri{ 0, i, (i%5)+1,   2*i-2, 2*i-1, (2*i)%10 });
	}
	for (unsigned int i = 1; i <= 5; i++) {	// Edges & Faces for the central ring
		sphere->mEdges.push_back(Mesh::Edge{ i, i+5 });	// downward diagonal
		sphere->mEdges.push_back(Mesh::Edge{ i+5, (i%5)+1 });	// upward diagonal
		sphere->mTris.push_back(Mesh::Tri{ i, i+5, (i%5)+1,   2*i+8, 2*i+9, 2*i-1 });
		sphere->mTris.push_back(Mesh::Tri{ (i%5)+1, i+5, i%5+6,   2*i+9, 2*i+19, 2*(i%5)+10 });
	}
	for (unsigned int i = 1; i <= 5; i++) {	// Edges & Faces for the bottom cap
		sphere->mEdges.push_back(Mesh::Edge{ 11, 5+i });	// lower spokes
		sphere->mEdges.push_back(Mesh::Edge{ 5+i, 6+(i%5) });	// lower ring
		sphere->mTris.push_back(Mesh::Tri{ 5+i, 11, 6+(i%5),   2*i+18, 2*(i%5)+20, 2*1+19 });
	}

	// Subdivide it N times
	for (unsigned int i = 0; i < subdiv; i++) {
		std::vector<Mesh::Tri> new_faces;
		std::vector<Mesh::Edge> new_edges;
		for (unsigned int face = 0; face < sphere->mTris.size(); face++) {
			unsigned int v1, v2, v3;
			v1 = sphere->mTris[face].verts[0];
			v2 = sphere->mTris[face].verts[1];
			v3 = sphere->mTris[face].verts[2];
			Mesh::Vertex n1, n2, n3;
			n1.pos = sphere->mVertices[v1].pos + sphere->mVertices[v2].pos;
			n2.pos = sphere->mVertices[v2].pos + sphere->mVertices[v3].pos;
			n3.pos = sphere->mVertices[v3].pos + sphere->mVertices[v1].pos;
			n1.norm = n1.pos / glm::abs(n1.pos);
			n2.norm = n2.pos / glm::abs(n2.pos);
			n3.norm = n3.pos / glm::abs(n3.pos);
			n1.pos = radius * n1.norm;
			n2.pos = radius * n2.norm;
			n3.pos = radius * n3.norm;
			n1.tex = (sphere->mVertices[v1].tex + sphere->mVertices[v2].tex) * 0.5f;
			n2.tex = (sphere->mVertices[v2].tex + sphere->mVertices[v3].tex) * 0.5f;
			n3.tex = (sphere->mVertices[v3].tex + sphere->mVertices[v1].tex) * 0.5f;
			unsigned int n = sphere->mVertices.size();
			sphere->addVertex(n1.pos, n1.norm, n1.tex);
			sphere->addVertex(n2.pos, n2.norm, n2.tex);
			sphere->addVertex(n3.pos, n3.norm, n3.tex);
			unsigned int e = sphere->mEdges.size();
			// To-do: recognize and remove duplicate edges
			// We could actually go through the edges first to figure out all the new vertices?
			// But then figuring out the right indices for the faces would be difficult
			new_edges.push_back(Mesh::Edge{ v1,  n+0 });
			new_edges.push_back(Mesh::Edge{ n+0, v2  });
			new_edges.push_back(Mesh::Edge{ v2,  n+1 });
			new_edges.push_back(Mesh::Edge{ n+1, v3  });
			new_edges.push_back(Mesh::Edge{ v3,  n+2 });
			new_edges.push_back(Mesh::Edge{ n+2, v1  });
			new_edges.push_back(Mesh::Edge{ n+0, n+1 });
			new_edges.push_back(Mesh::Edge{ n+1, n+2 });
			new_edges.push_back(Mesh::Edge{ n+2, n+0 });
			new_faces.push_back(Mesh::Tri{ v1, n+0, n+2,   e+0, e+8, e+5 });
			new_faces.push_back(Mesh::Tri{ v2, n+1, n+0,   e+2, e+6, e+1 });
			new_faces.push_back(Mesh::Tri{ v3, n+2, n+1,   e+4, e+7, e+3 });
			new_faces.push_back(Mesh::Tri{ n,  n+1, n+2,   e+6, e+7, e+8 });
		}
		sphere->mEdges = new_edges;
		sphere->mTris = new_faces;
	}
	return sphere;
}

void Mesh::addVertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex) {
	mVertexLookup.addNode(pos.x, pos.y, pos.z, mVertices.size());
	Vertex n;
	n.pos = pos;
	n.norm = norm;
	n.col = glm::vec3(0.0f);
	n.tex = tex;
	mVertices.push_back(n);
}