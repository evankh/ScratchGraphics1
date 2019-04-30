#ifndef __EKH_SCRATCH_GRAPHICS_1_FRAMEBUFFER__
#define __EKH_SCRATCH_GRAPHICS_1_FRAMEBUFFER__

#include <map>
#include <vector>

enum AttachmentType {
	ATTACHMENT_COLOR = 0,
	ATTACHMENT_INTEGER,
	ATTACHMENT_DEPTH
};

class Framebuffer {
private:
	unsigned int mHandle;
	unsigned int mWidth, mHeight;
	float mRelativeScale = 1.0f;
	std::vector<unsigned int> mAttachments;
	unsigned int mDepthAttachment;
	std::vector<AttachmentType> mTypes;
public:
	Framebuffer(unsigned int width, unsigned int height, float relativeScale = 1.0f);
	~Framebuffer();
	void attach(AttachmentType type);
	void validate() const;
	void resize(unsigned int width, unsigned int height);
	void setAsDrawingTarget() const;
	void setAttachmentAsTextureSource(int attachment = 0, int slot = 0) const;
	void setAsTextureSource(int start = 0) const;
	int getNumAttachments() const { return mAttachments.size(); };
	float getPixelWidth() const { return 1.0f / (mWidth * mRelativeScale); };
	float getPixelHeight() const { return 1.0f / (mHeight * mRelativeScale); };
	void getPixel(unsigned int x, unsigned int y, unsigned int attachment, void* result);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_FRAMEBUFFER__