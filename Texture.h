#ifndef __EKH_SCRATCH_GRAPHICS_1_TEXTURE__
#define __EKH_SCRATCH_GRAPHICS_1_TEXTURE__

#include <string>

// Include the GLI libraries, probably some glew/glut as well

class Texture {
private:
	// Probably a handle or something, maybe a filepath
	std::string mFilepath;
public:
	// Meh
	Texture(std::string filepath);
	~Texture();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_TEXTURE__