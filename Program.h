#ifndef __EKH_SCRATCH_GRAPHICS_1_PROGRAM__
#define __EKH_SCRATCH_GRAPHICS_1_PROGRAM__

class Shader;
#include "glm/matrix.hpp"	// Probably not ideal, should break this file out into separate files for each subclass, which will only include the files necessary for its own arguments

class Program {
private:
	unsigned int mHandle;
	Shader* mVS;
	Shader* mGS;
	Shader* mFS;
public:
	Program();
	Program(Shader* vs, Shader* fs);
	Program(Shader* vs, Shader* gs, Shader* fs);
	Program(char* vsFilepath, char* fsFilepath);
	~Program();
	void attach(Shader* vs, Shader* fs);
	void link();
	void validate();
	// I'm wondering if maybe subclassing for each type of uniforms is the right way to go? Should something else be sending uniforms?
	void use();
	void sendUniform(const char* name, float value);
	void sendUniform(const char* name, float* matrix);
	void sendUniform(const char* name, int size, float* value);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_PROGRAM__