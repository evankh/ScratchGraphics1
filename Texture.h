#ifndef __EKH_SCRATCH_GRAPHICS_1_TEXTURE__
#define __EKH_SCRATCH_GRAPHICS_1_TEXTURE__

#include <string>

class Texture {
private:
	std::string mFilepath;
	unsigned int mHandle;
public:
	Texture(std::string filepath);	// $#!@#! external libraries not being const-correct means I can't be either... which means no std::string
	~Texture();
	void setWrapX(bool shouldWrap);	// May need to change to an enum
	void setWrapY(bool shouldWrap);
	unsigned int getHandle() const { return mHandle; };
	void activate();
	void deactivate();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_TEXTURE__