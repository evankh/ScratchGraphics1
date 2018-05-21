#ifndef __EKH_SCRATCH_GRAPHICS_1_FRAME_BUFFER__
#define __EKH_SCRATCH_GRAPHICS_1_FRAME_BUFFER__

#include "Geometry.h"

class FrameBuffer {
	static Geometry sMagicQuad;
private:
	unsigned int mHandle;
	unsigned int mColorTextureHandle;
	unsigned int mDepthTextureHandle;
	float mRelativeScale;
public:
	FrameBuffer(unsigned int windowWidth, unsigned int windowHeight, float scale);
	~FrameBuffer();
	void setActive();
	void draw();
	void resize(unsigned int width, unsigned int height);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_FRAME_BUFFER__