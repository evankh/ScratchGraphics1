#ifndef __EKH_SCRATCH_GRAPHICS_1_POST_PROCESSING_PIPELINE__
#define __EKH_SCRATCH_GRAPHICS_1_POST_PROCESSING_PIPELINE__

#include "FrameBuffer.h"
#include "KernelManager.h"
#include "Program.h"
#include <vector>

/*
struct ProcessingStage {
	FrameBuffer** sources;
	int numSources;
	Program* filter;
	int samplersIn, samplersOut;
	int numKernels;
	Kernel* kernels;	// If just 1, can put its address here	// Except can't, bcause statically allocated
	Kernel kernel;	// I hate everything about how these are being distinguished but at least it should stop leaking
	FrameBuffer* target;
};*/

struct ProcessingStage {
	Program* filter;
	ProgramData data;
	int numInputs;
	FrameBuffer* input;
	FrameBuffer* output;
};

class PostProcessingPipeline {
private:
	unsigned int mWindowWidth;	// Duplicating this data in so many places is unecessary and begging for trouble, consider using a reference
	unsigned int mWindowHeight;
	FrameBuffer* mInputFB;
	FrameBuffer* mOutputFB;
	std::vector<ProcessingStage> mProcessingStages;
	ProcessingStage* mCompositingStage = nullptr;
	std::vector<FrameBuffer*> mCompositingInputs;
public:
	PostProcessingPipeline();
	~PostProcessingPipeline();
	void init(unsigned int width, unsigned int height);
	/*void attach(Program* program, bool isCompositingInput, int numSamplersIn, int numSamplersOut, Kernel kernel = Kernel{ 0,NULL }, float relativeSize = 1.0f);	// Or I suppose other types of data, but now it's just kernels
	void attach(Program* program, bool isCompositingInput, int numSamplersIn, int numSamplersOut, int numKernels, Kernel* kernels, float relativeSize = 1.0f);	// numKernels should equal numSamplersIn
	void attach(Program* program, int numSamplersIn, int numSamplersOut = 1, float relativeScale = 1.0f);	// For compositing stages that don't need any fancy kernels*/
	void attach(Program* program, bool isCompositingInput, float relativeScale = 1.0f);
	void attach(Program* program, bool isCompositingInput, Kernel kernel, float relativeScale = 1.0f);
	void attach(Program* program, bool isCompositingInput, int numKernels, Kernel* kernels, float relativeScale = 1.0f);
	void attachCompositor(Program* compositor);
	void process();
	void draw();	// Draw from the output of this PPP
	void resize(unsigned int width, unsigned int height);
	void enableDrawing();	// Enable drawing onto the input this PPP
	void clear();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_POST_PROCESSING_PIPELINE__