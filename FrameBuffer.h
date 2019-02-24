#ifndef __EKH_SCRATCH_GRAPHICS_1_FRAME_BUFFER__
#define __EKH_SCRATCH_GRAPHICS_1_FRAME_BUFFER__

#define NULL 0

class FrameBuffer {
private:
	unsigned int mHandle;
	unsigned int* mColorTextureHandles = NULL;
	unsigned int mDepthTextureHandle;
	float mRelativeScale;
	unsigned int mWidth, mHeight;
	int mSamplersOut;
public:
	FrameBuffer(unsigned int windowWidth, unsigned int windowHeight, float scale = 1.0f, int samplersOut = 1);
	~FrameBuffer();
	void setActive();
	void draw();
	void resize(unsigned int width, unsigned int height);
	float getRelativeScale() const { return mRelativeScale; };
	int getSamplersOut() const { return mSamplersOut; };
	unsigned int const* getOutputTextures() const { return mColorTextureHandles; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_FRAME_BUFFER__