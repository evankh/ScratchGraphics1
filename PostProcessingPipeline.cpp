#include "PostProcessingPipeline.h"

PostProcessingPipeline::PostProcessingPipeline() {
	mInputFB = mOutputFB = NULL;
}

void PostProcessingPipeline::init(unsigned int width, unsigned int height) {
	mWindowWidth = width;
	mWindowHeight = height;
	mInputFB = new FrameBuffer(mWindowWidth, mWindowHeight);
	mOutputFB = mInputFB;
}

PostProcessingPipeline::~PostProcessingPipeline() {
	mProcessingStages.clear();
}

void PostProcessingPipeline::attach(Program* program, int numSamplersIn, int numSamplersOut, Kernel kernel, float relativeScale) {
	if (numSamplersIn != mOutputFB->getSamplersOut()) throw std::invalid_argument("That stage won't fit here");
	if (mProcessingStages.size())
		mProcessingStages.push_back({ mProcessingStages.back().target, program, numSamplersIn, numSamplersOut, (kernel.samples>0)?1:0, &kernel, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale, numSamplersOut) });
	else
		mProcessingStages.push_back({ mInputFB, program, numSamplersIn, numSamplersOut, (kernel.samples>0)?1:0, &kernel, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale, numSamplersOut) });
	program->use();
	for (unsigned int i = 0; i < numSamplersIn; i++)
		program->sendUniform((std::string("uTexture") + std::to_string(i)).data(), i);
	mOutputFB = mProcessingStages.back().target;
}

void PostProcessingPipeline::attach(Program* program, int numSamplersIn, int numSamplersOut, int numKernels, Kernel* kernels, float relativeScale) {
	if (numSamplersIn != mOutputFB->getSamplersOut()) throw std::invalid_argument("That stage won't fit here");
	if (mProcessingStages.size())
		mProcessingStages.push_back({ mProcessingStages.back().target, program, numSamplersIn, numSamplersOut, numKernels, kernels, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale, numSamplersOut) });
	else
		// Hypothetically, with multiple input samplers, it won't ever be the first stage, but no way in heck am I just going to assume that
		mProcessingStages.push_back({ mInputFB, program, numSamplersIn, numSamplersOut, numKernels, kernels, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale, numSamplersOut) });
	program->use();
	for (unsigned int i = 0; i < numSamplersIn; i++)
		program->sendUniform((std::string("uTexture") + std::to_string(i)).data(), i);
	mOutputFB = mProcessingStages.back().target;
}

void PostProcessingPipeline::process() {
	if (mProcessingStages.size() == 0)
		return;
	for (unsigned int i = 0; i < mProcessingStages.size(); i++) {
		mProcessingStages[i].target->setActive();
		mProcessingStages[i].filter->use();
		mProcessingStages[i].filter->sendUniform("uPixWidth", 1.0f / (mProcessingStages[i].target->getRelativeScale() * mWindowWidth));
		mProcessingStages[i].filter->sendUniform("uPixHeight", 1.0f / (mProcessingStages[i].target->getRelativeScale() * mWindowHeight));
		for (int j = 0; j < mProcessingStages[i].numKernels; j++)
			mProcessingStages[i].filter->sendUniform((std::string("uKernel") + std::to_string(j)).data(), 1, mProcessingStages[i].kernels[j].samples, mProcessingStages[i].kernels[j].weights);
		mProcessingStages[i].source->draw();
	}
}

void PostProcessingPipeline::draw() {
	// Set the active program to an ortho one
	if (mOutputFB) mOutputFB->draw();
}

void PostProcessingPipeline::resize(unsigned int width, unsigned int height) {
	mWindowWidth = width;
	mWindowHeight = height;
	if (mInputFB) mInputFB->resize(mWindowWidth, mWindowHeight);
	for (auto buffer : mProcessingStages)
		buffer.target->resize(mWindowWidth, mWindowHeight);
}

void PostProcessingPipeline::enableDrawing() {
	if (mInputFB) mInputFB->setActive();
}

void PostProcessingPipeline::clear() {
	for (auto stage : mProcessingStages)
		delete stage.target;
	mProcessingStages.clear();
}
