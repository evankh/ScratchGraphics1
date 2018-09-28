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
	delete mInputFB;
	for (auto stage : mProcessingStages)
		delete stage.second;
	mProcessingStages.clear();
	/*for (auto frame : mIntermediateFBs)
		delete frame;
	mIntermediateFBs.clear();*/
}

void PostProcessingPipeline::attach(Program* program, float relativeScale) {
	mProcessingStages.push_back(std::pair<Program*, FrameBuffer*>(program, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale)));
	mOutputFB = mProcessingStages.back().second;
	/*if (mProcessingStages.size() == 0)
		mOutputFB = new FrameBuffer(mWindowWidth, mWindowHeight, 1.0);
	mProcessingStages.push_back(program);
	if (mProcessingStages.size() > 1)
		mIntermediateFBs.push_back(new FrameBuffer(mWindowWidth, mWindowHeight, 1.0));*/
	mKernels.push_back(std::pair<int, float*>(0, NULL));
}

void PostProcessingPipeline::attach(Program* program, int size, float* kernel, float relativeScale) {
	mProcessingStages.push_back(std::pair<Program*, FrameBuffer*>(program, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale)));
	mOutputFB = mProcessingStages.back().second;
	/*if (mProcessingStages.size() == 0)
		mOutputFB = new FrameBuffer(mWindowWidth, mWindowHeight, 1.0);
	mProcessingStages.push_back(program);
	if (mProcessingStages.size() > 1)
		mIntermediateFBs.push_back(new FrameBuffer(mWindowWidth, mWindowHeight, 1.0));*/
	mKernels.push_back(std::pair<int, float*>(size, kernel));
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
	mProcessingStages[0].second->setActive();
	mProcessingStages[0].first->use();
	mProcessingStages[0].first->sendUniform("uPixWidth", 1.0f / (mProcessingStages[0].second->getRelativeScale() * mWindowWidth));
	mProcessingStages[0].first->sendUniform("uPixHeight", 1.0f / (mProcessingStages[0].second->getRelativeScale() * mWindowHeight));
	if (mKernels[0].first) mProcessingStages[0].first->sendUniform("uKernel", mKernels[0].first, mKernels[0].second);
	mInputFB->draw();
	for (unsigned int i = 1; i < mProcessingStages.size(); i++) {
		mProcessingStages[i].second->setActive();
		mProcessingStages[i].first->use();
		mProcessingStages[i].first->sendUniform("uPixWidth", 1.0f / (mProcessingStages[i].second->getRelativeScale() * mWindowWidth));
		mProcessingStages[i].first->sendUniform("uPixHeight", 1.0f / (mProcessingStages[i].second->getRelativeScale() * mWindowHeight));
		if (mKernels[i].first) mProcessingStages[i].first->sendUniform("uKernel", mKernels[i].first, mKernels[i].second);
		mProcessingStages[i - 1].second->draw();
	}
	/*mOutputFB->setActive();
	mProcessingStages.back().first->use();
	mProcessingStages.back().first->sendUniform("pixel_width", 1.0f / mWindowWidth);
	mProcessingStages.back().first->sendUniform("pixel_height", 1.0f / mWindowHeight);
	if (mKernels.back().first) mProcessingStages.back()->sendUniform("kernel", mKernels.back().first, mKernels.back().second);
	mIntermediateFBs.back()->draw();*/
}

void PostProcessingPipeline::draw() {
	// Set the active program to an ortho one
	mOutputFB->draw();
}

void PostProcessingPipeline::resize(unsigned int width, unsigned int height) {
	mWindowWidth = width;
	mWindowHeight = height;
	mInputFB->resize(mWindowWidth, mWindowHeight);
	//if (mOutputFB != mInputFB) mOutputFB->resize(mWindowWidth, mWindowHeight);
	for (auto buffer : mProcessingStages)
		buffer.second->resize(mWindowWidth, mWindowHeight);
}

void PostProcessingPipeline::enableDrawing() {
	mInputFB->setActive();
}

void PostProcessingPipeline::clear() {
	for (auto stage : mProcessingStages)
		delete stage.second;
	mProcessingStages.clear();
	for (auto kernel : mKernels)
		if (kernel.first) delete kernel.second;
	mKernels.clear();
}
