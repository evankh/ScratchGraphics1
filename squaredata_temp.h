#ifndef __EKH_SCRATCH_GRAPHICS_1_SQUARE_DATA_TEMP__
#define __EKH_SCRATCH_GRAPHICS_1_SQUARE_DATA_TEMP__

unsigned int square_numverts = 4;
float square_vertices[12] = {
	0.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f,
	0.0f,1.0f,0.0f,
	1.0f,1.0f,0.0f
};

unsigned int square_numfaces = 2;
unsigned int square_faces[6] = {
	0,1,2,
	1,3,2
};

float square_colors[12] = {
	1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f
};

#endif//__EKH_SCRATCH_GRPAHICS_1_SQUARE_DATA_TEMP__