#ifndef __EKH_SCRATCH_GRAPHICS_1_TEXTURE__
#define __EKH_SCRATCH_GRAPHICS_1_TEXTURE__

class Texture {
private:
	unsigned int mHandle;
	unsigned int mWidth;
	unsigned int mHeight;
public:
	Texture(unsigned int width, unsigned int height);
	Texture(const char* filename);
	~Texture();
	void resize(unsigned int width, unsigned int height);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_TEXTURE__