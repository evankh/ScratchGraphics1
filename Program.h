#ifndef __EKH_SCRATCH_GRAPHICS_1_PROGRAM__
#define __EKH_SCRATCH_GRAPHICS_1_PROGRAM__

class Shader;

class Program {
	static Program* sScreenDraw;
	static void generateScreenDrawProgram();
private:
	Program(const char* vs, const char* fs);
	unsigned int mHandle;
	Shader* mVS;
	Shader* mGS;
	Shader* mFS;
	int mSamplesIn, mSamplesOut;
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
	void link();
	void validate();
	// I'm wondering if maybe subclassing for each type of uniforms is the right way to go? Should something else be sending uniforms?
	void use() const;	// I'm not sure if it's correct to call this const, because while it's not changing the Program object, it is affecting the OpenGL state.
	void sendUniform(const char* name, const float value) const;	// Same goes for the uniforms
	void sendUniform(const char* name, const float* matrix) const;
	void sendUniform(const char* name, const int size, const int count, const float* value) const;
	void sendUniform(const char* name, const unsigned int value) const;
	static const Program* getScreenDraw() { return sScreenDraw; };
	int getSamplesIn() const { return mSamplesIn; };
	int getSamplesOut() const { return mSamplesOut; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_PROGRAM__