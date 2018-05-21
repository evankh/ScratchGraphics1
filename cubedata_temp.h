#ifndef __EKH_SCRATCH_GRAPHICS_1_CUBEDATA_TEMP__
#define __EKH_SCRATCH_GRAPHICS_1_CUBEDATA_TEMP__

/***********************************************

      3 __________ 7
       /|        /|
    1 /_L_______/ |
     |  |      |5 |
     |  L______L__|
     | / 2     | / 6
     L/________|/
    0          4

***********************************************/

float cube_vertices[] = {
	0,0,0,
	0,0,1,
	0,1,0,
	0,1,1,
	1,0,0,
	1,0,1,
	1,1,0,
	1,1,1 };
float cube_colors[] = {
	0,0,0,
	0,0,1,
	0,1,0,
	0,1,1,
	1,0,0,
	1,0,1,
	1,1,0,
	1,1,1 };
unsigned int cube_faces[] = {
	0,1,3,
	0,3,2,
	0,5,1,
	0,4,5,
	4,6,7,
	4,7,5,
	1,5,7,
	1,7,3,
	0,4,6,
	0,6,2,
	6,2,3,
	6,3,7
};
unsigned int cube_numverts = 8;
unsigned int cube_numfaces = 12;

//#include "Vertex.h"
//#include <vector>

//std::vector<int> ints{ 1,2,3 };

/*std::vector<Vertex> vertexData = {
	{{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},
	{{0.0f,0.0f,1.0f},{0.0f,0.0f,1.0f}},
	{{0.0f,1.0f,0.0f},{0.0f,1.0f,0.0f}},
	{{0.0f,1.0f,1.0f},{0.0f,1.0f,1.0f}},
	{{1.0f,0.0f,0.0f},{1.0f,0.0f,0.0f}},
	{{1.0f,0.0f,1.0f},{1.0f,0.0f,1.0f}},
	{{1.0f,1.0f,0.0f},{1.0f,1.0f,0.0f}},
	{{1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f}}
};*/

#endif//__EKH_SCRATCH_GRAPHICS_1_CUBEDATA_TEMP__