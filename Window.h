#ifndef __EKH_SCRATCH_GRAPHICS_1_WINDOW__
#define __EKH_SCRATCH_GRAPHICS_1_WINDOW__

class Window {
private:
	unsigned int mHandle;
	unsigned int mWidth;
	unsigned int mHeight;
	const char* mTitle;
public:
	Window();
	Window(int width, int height, char* title);
	~Window();
	void enableDrawing();	// Enables drawing to the default (window) FrameBuffer
	void resize(unsigned int width, unsigned int height);
	void rename(const char* title);
	unsigned int getWidth() const { return mWidth; };
	unsigned int getHeight() const { return mHeight; };
	void printGLVersion() const;
	int getGLVersion() const;
	void update() const;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_WINDOW__