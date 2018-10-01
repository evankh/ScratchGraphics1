#include "PostProcessingPipeline.h"

PostProcessingPipeline::PostProcessingPipeline() {
	mInputFB = mOutputFB = NULL;
}

void PostProcessingPipeline::init(unsigned int width, unsigned int height) {
	mWindowWidth = width;
	mWindowHeight = height;
	mInputFB = new FrameBuffer(mWindowWidth, mWindowHeight, 1.0);
	mOutputFB = mInputFB;
}

PostProcessingPipeline::~PostProcessingPipeline() {
	mProcessingStages.clear();
}

void PostProcessingPipeline::attach(Program* program, Kernel kernel, float relativeScale) {
	if (mProcessingStages.size())
		mProcessingStages.push_back({ mProcessingStages.back().target, program, kernel, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale) });
	else
		mProcessingStages.push_back({ mInputFB, program, kernel, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale) });
	mOutputFB = mProcessingStages.back().target;
}

void PostProcessingPipeline::process() {
	if (mProcessingStages.size() == 0)
		return;
	/*if (mProcessingStages.size() == 1) {
		mOutputFB->setActive();
		mProcessingStages[0].first->use();
		mProcessingStages[0].first->sendUniform("pixel_width", 1.0f / mWindowWidth);
		mProcessingStages[0].first->sendUniform("pixel_height", 1.0f / mWindowHeight);
		if (mKernels[0].first) mProcessingStages[0].first->sendUniform("kernel", mKernels[0].first, mKernels[0].second);
		mInputFB->draw();
		return;
	}*/
	for (unsigned int i = 0; i < mProcessingStages.size(); i++) {
		mProcessingStages[i].target->setActive();
		mProcessingStages[i].filter->use();
		mProcessingStages[i].filter->sendUniform("uPixWidth", 1.0f / (mProcessingStages[i].target->getRelativeScale() * mWindowWidth));
		mProcessingStages[i].filter->sendUniform("uPixHeight", 1.0f / (mProcessingStages[i].target->getRelativeScale() * mWindowHeight));
		if (mProcessingStages[i].kernel.samples) mProcessingStages[i].filter->sendUniform("uKernel", mProcessingStages[i].kernel.samples, mProcessingStages[i].kernel.weights);
		mProcessingStages[i].source->draw();
	}
}

void PostProcessingPipeline::draw() {
	// Set the active program to an ortho one
	mOutputFB->draw();
}

void PostProcessingPipeline::resize(unsigned int width, unsigned int height) {
	mWindowWidth = width;
	mWindowHeight = height;
	mInputFB->resize(mWindowWidth, mWindowHeight);
	for (auto buffer : mProcessingStages)
		buffer.target->resize(mWindowWidth, mWindowHeight);
}

void PostProcessingPipeline::enableDrawing() {
	mInputFB->setActive();
}

void PostProcessingPipeline::clear() {
	for (auto stage : mProcessingStages)
		delete stage.target;
	mProcessingStages.clear();
}
