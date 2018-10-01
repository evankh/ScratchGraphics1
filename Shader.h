#ifndef __EKH_SCRATCH_GRAPHICS_1_SHADER__
#define __EKH_SCRATCH_GRAPHICS_1_SHADER__

#include <string>

class Shader {
	friend class Program;
private:
	std::string mFilepath;
	unsigned int mType;
	unsigned int mHandle;
public:
	Shader(std::string filepath, unsigned int type);
	~Shader();
	void reload();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SHADER__