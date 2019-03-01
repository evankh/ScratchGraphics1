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

void PostProcessingPipeline::attach(Program* program, bool isCompositingInput, int numSamplersIn, int numSamplersOut, Kernel kernel, float relativeScale) {
	if (numSamplersIn != mOutputFB->getSamplersOut()) throw std::invalid_argument("That stage won't fit here");
	FrameBuffer** target = new FrameBuffer*;
	if (mProcessingStages.size())
		*target = mProcessingStages.back().target;
	else
		*target = mInputFB;
	mProcessingStages.push_back({ target, 1, program, numSamplersIn, numSamplersOut, 0, NULL, kernel, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale, numSamplersOut) });
	program->use();
	for (int i = 0; i < numSamplersIn; i++)
		program->sendUniform((std::string("uTexture") + std::to_string(i)).data(), i);	// Don't like this, Program should have a list of its own uniforms (but how to initialize them?)
	if (isCompositingInput) mCompositingInputs.push_back(mProcessingStages.back().target);
	mOutputFB = mProcessingStages.back().target;
}

void PostProcessingPipeline::attach(Program* program, bool isCompositingInput, int numSamplersIn, int numSamplersOut, int numKernels, Kernel* kernels, float relativeScale) {
	if (numSamplersIn != mOutputFB->getSamplersOut()) throw std::invalid_argument("That stage won't fit here");
	FrameBuffer** target =  new FrameBuffer*;
	if (mProcessingStages.size())
		*target = mProcessingStages.back().target;
	else
		// Hypothetically, with multiple input samplers, it won't ever be the first stage, but no way in heck am I just going to assume that
		*target = mInputFB;
	mProcessingStages.push_back({ target, 1, program, numSamplersIn, numSamplersOut, numKernels, kernels, Kernel{0,NULL}, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale, numSamplersOut) });
	program->use();
	for (int i = 0; i < numSamplersIn; i++)
		program->sendUniform((std::string("uTexture") + std::to_string(i)).data(), i);
	if (isCompositingInput) mCompositingInputs.push_back(mProcessingStages.back().target);
	mOutputFB = mProcessingStages.back().target;
}

void PostProcessingPipeline::attach(Program* program, int numSamplersIn, int numSamplersOut, float relativeScale) {
	if (numSamplersIn != mCompositingInputs.size()) throw std::invalid_argument("Given number of input samplers != number of stages listed as compositing inputs");
	FrameBuffer** targets;
	if (mProcessingStages.size())
		targets = mCompositingInputs.data();
	else
		throw std::runtime_error("Cannot put a compositing stage first");
	mProcessingStages.push_back({ targets, numSamplersIn, program, numSamplersIn, numSamplersOut, 0, NULL, Kernel{0,NULL}, new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale,numSamplersOut) });
	program->use();
	unsigned int* inputs = new unsigned int[numSamplersIn];
	int j = 0;
	for (auto input : mCompositingInputs) {
		const unsigned int* stage = input->getOutputTextures();
		for (int i = 0; i < input->getSamplersOut(); i++)
			inputs[i + j] = stage[i];
		j += input->getSamplersOut();
	}
	program->sendUniform("uTexture", numSamplersIn, inputs);
	delete[] inputs;
	mOutputFB = mProcessingStages.back().target;
}

#include "Geometry.h"

void PostProcessingPipeline::process() {
	if (mProcessingStages.size() == 0)
		return;
	for (unsigned int i = 0; i < mProcessingStages.size(); i++) {
		mProcessingStages[i].target->setActive();
		mProcessingStages[i].filter->use();
		mProcessingStages[i].filter->sendUniform("uPixWidth", 1.0f / (mProcessingStages[i].target->getRelativeScale() * mWindowWidth));
		mProcessingStages[i].filter->sendUniform("uPixHeight", 1.0f / (mProcessingStages[i].target->getRelativeScale() * mWindowHeight));
		if (mProcessingStages[i].kernel.samples)
			mProcessingStages[i].filter->sendUniform("uKernel", 1, mProcessingStages[i].kernel.samples, mProcessingStages[i].kernel.weights);
		for (int j = 0; j < mProcessingStages[i].numKernels; j++)
			mProcessingStages[i].filter->sendUniform((std::string("uKernel") + std::to_string(j)).data(), 1, mProcessingStages[i].kernels[j].samples, mProcessingStages[i].kernels[j].weights);
		for (int j = 0; j < mProcessingStages[i].numSources; j++)
			mProcessingStages[i].sources[j]->draw(i);	// Will only allow one output from each source for compositors
		Geometry::getScreenQuad()->render();
	}
}

void PostProcessingPipeline::draw() {
	// Set the active program to an ortho one
	if (mOutputFB) mOutputFB->draw(0);
	Geometry::getScreenQuad()->render();
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
