#ifndef __EKH_SCRATCH_GRAPHICS_1_POSTPROCESSING__
#define __EKH_SCRATCH_GRAPHICS_1_POSTPROCESSING__

#include <vector>

class Framebuffer;
#include "Util/KernelManager.h"
class Program;

struct StageData {
	Framebuffer* input, *output;
	Program* filter;
	float scale;
	std::vector<Kernel> kernels;
};

class PostprocessingPipeline {
private:
	unsigned int mWindowWidth, mWindowHeight;
	Framebuffer* mInput = nullptr, *mOutput = nullptr;
	std::vector<StageData> mStages;
	StageData* mCompositor = nullptr;
	std::vector<Framebuffer*> mCompositingInputs;
public:
	PostprocessingPipeline(unsigned int windowWidth, unsigned int windowHeight);
	~PostprocessingPipeline();
	void attach(StageData &data, bool isCompositingInput = false);
	void attachCompositor(Program* compositor);
	void process();
	void resize(unsigned int width, unsigned int height);
	void setAsDrawTarget();
	void setAsTextureSource();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_POSTPROCESSING__