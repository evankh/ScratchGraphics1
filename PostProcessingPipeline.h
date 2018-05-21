#ifndef __EKH_SCRATCH_GRAPHICS_1_POST_PROCESSING_PIPELINE__
#define __EKH_SCRATCH_GRAPHICS_1_POST_PROCESSING_PIPELINE__

#include "FrameBuffer.h"
#include "Program.h"
#include <vector>

class PostProcessingPipeline {
private:
	unsigned int mWindowWidth;	// Duplicating this data in so many places is unecessary and begging for trouble, consider using a reference
	unsigned int mWindowHeight;
	FrameBuffer* mInputFB;
	FrameBuffer* mOutputFB;
	std::vector<FrameBuffer*> mIntermediateFBs;
	std::vector<Program*> mProcessingStages;
	std::vector<std::pair<int, float*>> mKernels;
public:
	PostProcessingPipeline();
	~PostProcessingPipeline();
	void init(unsigned int width, unsigned int height);
	void attach(Program* program);
	void attach(Program* program, int size, float* kernel);	// Or I suppose other types of data, but now it's just kernels
	void process();
	void draw();
	void resize(unsigned int width, unsigned int height);
	void enableDrawing();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_POST_PROCESSING_PIPELINE__