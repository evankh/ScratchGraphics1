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
	if (mOutputFB != mInputFB) delete mOutputFB;
	delete mInputFB;
	for (auto stage : mProcessingStages)
		delete stage;
	mProcessingStages.clear();
	for (auto frame : mIntermediateFBs)
		delete frame;
	mIntermediateFBs.clear();
}

void PostProcessingPipeline::attach(Program* program) {
	if (mProcessingStages.size() == 0)
		mOutputFB = new FrameBuffer(mWindowWidth, mWindowHeight, 1.0);
	mProcessingStages.push_back(program);
	if (mProcessingStages.size() > 1)
		mIntermediateFBs.push_back(new FrameBuffer(mWindowWidth, mWindowHeight, 1.0));
	mKernels.push_back(std::pair<int, float*>(0, NULL));
}

void PostProcessingPipeline::attach(Program* program, int size, float* kernel) {
	if (mProcessingStages.size() == 0)
		mOutputFB = new FrameBuffer(mWindowWidth, mWindowHeight, 1.0);
	mProcessingStages.push_back(program);
	if (mProcessingStages.size() > 1)
		mIntermediateFBs.push_back(new FrameBuffer(mWindowWidth, mWindowHeight, 1.0));
	mKernels.push_back(std::pair<int, float*>(size, kernel));
}

void PostProcessingPipeline::process() {
	if (mProcessingStages.size() == 0)
		return;
	if (mProcessingStages.size() == 1) {
		mOutputFB->setActive();
		mProcessingStages[0]->use();
		mProcessingStages[0]->sendUniform("pixel_width", 1.0f / mWindowWidth);
		mProcessingStages[0]->sendUniform("pixel_height", 1.0f / mWindowHeight);
		if (mKernels[0].first) mProcessingStages[0]->sendUniform("kernel", mKernels[0].first, mKernels[0].second);
		mInputFB->draw();
		return;
	}
	mIntermediateFBs[0]->setActive();
	mProcessingStages[0]->use();
	mInputFB->draw();
	for (unsigned int i = 1; i < mProcessingStages.size() - 1; i++) {
		mIntermediateFBs[i]->setActive();
		mProcessingStages[i]->use();
		mProcessingStages[i]->sendUniform("pixel_width", 1.0f / mWindowWidth);
		mProcessingStages[i]->sendUniform("pixel_height", 1.0f / mWindowHeight);
		if (mKernels[i].first) mProcessingStages[i]->sendUniform("kernel", mKernels[i].first, mKernels[i].second);
		mIntermediateFBs[i - 1]->draw();
	}
	mOutputFB->setActive();
	mProcessingStages.back()->use();
	mProcessingStages.back()->sendUniform("pixel_width", 1.0f / mWindowWidth);
	mProcessingStages.back()->sendUniform("pixel_height", 1.0f / mWindowHeight);
	if (mKernels.back().first) mProcessingStages.back()->sendUniform("kernel", mKernels.back().first, mKernels.back().second);
	mIntermediateFBs.back()->draw();
}

void PostProcessingPipeline::draw() {
	// Set the active program to an ortho one
	mOutputFB->draw();
}

void PostProcessingPipeline::resize(unsigned int width, unsigned int height) {
	mWindowWidth = width;
	mWindowHeight = height;
	mInputFB->resize(mWindowWidth, mWindowHeight);
	if (mOutputFB != mInputFB) mOutputFB->resize(mWindowWidth, mWindowHeight);
	for (auto buffer : mIntermediateFBs)
		buffer->resize(mWindowWidth, mWindowHeight);
}

void PostProcessingPipeline::enableDrawing() {
	mInputFB->setActive();
}
