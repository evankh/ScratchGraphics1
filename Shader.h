#ifndef __EKH_SCRATCH_GRAPHICS_1_SHADER__
#define __EKH_SCRATCH_GRAPHICS_1_SHADER__

//#include <fstream>

class Shader {
	friend class Program;
private:
	const char* mFilepath;
	unsigned int mType;
	unsigned int mHandle;
public:
	Shader(const char* filepath, unsigned int type);
	Shader(std::string filepath, unsigned int type);
	~Shader();
	void reload();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_SHADER__