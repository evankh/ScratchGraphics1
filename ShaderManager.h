#ifndef __EKH_SCRATCH_GRAPHICS_1_SHADER_MANAGER__
#define __EKH_SCRATCH_GRAPHICS_1_SHADER_MANAGER__

#include "Shader.h"

#include <map>
#include <string>

class ShaderManager {
private:
	std::map<std::string, std::map<int, Shader*>> mShaderLibrary;
	int mGLVersion = 100;
public:
	void setGLVersion(int version);
	void add(std::string name, int version, Shader* shader);
	Shader* get(std::string name);
	void clear();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SHADER_MANAGER__