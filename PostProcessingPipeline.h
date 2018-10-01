#ifndef __EKH_SCRATCH_GRAPHICS_1_POST_PROCESSING_PIPELINE__
#define __EKH_SCRATCH_GRAPHICS_1_POST_PROCESSING_PIPELINE__

#include "FrameBuffer.h"
#include "KernelManager.h"
#include "Program.h"
#include <vector>

struct ProcessingStage {
	FrameBuffer* source;
	Program* filter;
	Kernel kernel;
	FrameBuffer* target;
};

class PostProcessingPipeline {
private:
	unsigned int mWindowWidth;	// Duplicating this data in so many places is unecessary and begging for trouble, consider using a reference
	unsigned int mWindowHeight;
	FrameBuffer* mInputFB;
	FrameBuffer* mOutputFB;
	std::vector<ProcessingStage> mProcessingStages;
public:
	PostProcessingPipeline();
	~PostProcessingPipeline();
	void init(unsigned int width, unsigned int height);
	void attach(Program* program, Kernel kernel = Kernel{ 0,NULL }, float relativeSize = 1.0f);	// Or I suppose other types of data, but now it's just kernels
	void process();
	void draw();
	void resize(unsigned int width, unsigned int height);
	void enableDrawing();
	void clear();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_POST_PROCESSING_PIPELINE__