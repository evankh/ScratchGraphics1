#ifndef __EKH_SCRATCH_GRAPHICS_1_PROGRAM__
#define __EKH_SCRATCH_GRAPHICS_1_PROGRAM__

#include "Util/KernelManager.h"
#include "Util/NamedContainer.h"
#include <glm/matrix.hpp>
#include <vector>

class Shader;

struct ShaderNode {
	Shader* shader;
	ShaderNode* next;
};

enum class UniformType {
	INT,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4,
	KERNEL,
	TEXTURE
};

struct Uniform {
	UniformType type;
	union {
		int i; unsigned int ui; float f; glm::vec2 v2; glm::vec3 v3; glm::vec4 v4; glm::mat2 m2; glm::mat3 m3; glm::mat4 m4;
	};
	Uniform() { ui = 0; };
};

struct ProgramData {
	// A collection of Textures, Kernels, Uniforms, etc. that defines the actual appearance of a shader
	int numKernels;
	Kernel* kernels;
	std::vector<unsigned int> textures;
	std::vector<unsigned int> outputs;
	NamedContainer<Uniform> uniforms;
	void use();
};

class Program {
	static Program* sScreenDraw;
	static void generateScreenDrawProgram();
private:
	Program(const char* vs, const char* fs);
	unsigned int mHandle;
	ShaderNode* mVS = nullptr;
	ShaderNode* mTCS = nullptr;
	ShaderNode* mTES = nullptr;
	ShaderNode* mGS = nullptr;
	ShaderNode* mFS = nullptr;
	ShaderNode* mCS = nullptr;
	int mSamplesIn, mSamplesOut;
	NamedContainer<UniformType> mUniformTypes;
	NamedContainer<unsigned int> mUniformLocations;
public:
	Program();
	Program(Shader* vs, Shader* fs);
	Program(Shader* vs, Shader* fs, int in, int out);
	Program(Shader* vs, Shader* gs, Shader* fs);
	Program(char* vsFilepath, char* fsFilepath);
	~Program();
	void attach(Shader* shader, unsigned int type);
	void attach(Shader* vs, Shader* fs);
	void detachAll();
	void removeAll();
	void removeAll(unsigned int type);
	bool link(bool allow_output);
	bool link() { return link(true); };
	bool validate(bool allow_output) const;
	bool validate() const { return validate(true); };
	void use() const;	// I'm not sure if it's correct to call this const, because while it's not changing the Program object, it is affecting the OpenGL state.
	void sendUniform(const char* name, const float value) const;	// Same goes for the uniforms
	void sendUniform(const char* name, const float* matrix) const;
	void sendUniform(const char* name, const int size, const int count, const float* value) const;
	void sendUniform(const char* name, const int value) const;
	void sendUniform(const char* name, const unsigned int value) const;
	void sendUniform(const char* name, const int count, const unsigned int* values) const;
	static const Program* getScreenDraw() { return sScreenDraw; };
	int getSamplesIn() const { return mSamplesIn; };
	int getSamplesOut() const { return mSamplesOut; };
	bool isTesselated() const { return mTES != nullptr; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_PROGRAM__