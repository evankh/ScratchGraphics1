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
/*
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
		program->sendUniform((std::string("uTexture") + std::to_string(i)).data(), i);	// Does that even work? I don't think it's supposed to
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
*/

void PostProcessingPipeline::attach(Program* program, bool isCompositingInput, float relativeScale) {
	ProcessingStage stage;
	stage.filter = program;
	stage.data.numKernels = 0;
	stage.data.kernels = nullptr;
	Uniform pixSize;
	pixSize.type = UniformType::FLOAT;
	pixSize.f = 1.0f / (relativeScale * mWindowWidth);
	stage.data.uniforms.add("uPixWidth", pixSize);
	pixSize.f = 1.0f / (relativeScale * mWindowHeight);
	stage.data.uniforms.add("uPixHeight", pixSize);
	stage.numInputs = 1;
	stage.input = new FrameBuffer*;
	stage.input[0] = mOutputFB;
	stage.output = new FrameBuffer(mWindowWidth, mWindowHeight, relativeScale);
	mProcessingStages.push_back(stage);
}

#include "Geometry.h"
/*
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
}*/

void PostProcessingPipeline::process() {
	for (auto stage : mProcessingStages) {
		stage.output->setActive();
		stage.data.use();
		stage.filter->use();
		stage.input[0]->activate(0);
		Geometry::getScreenQuad()->render();
	}
	if (mCompositingStage) {
		int texInputs = 0;
		for (int i = 0; i < mCompositingStage->numInputs; i++) {
			mCompositingStage->filter->use();
			mCompositingStage->input[i]->activate(texInputs);
			Geometry::getScreenQuad()->render();
			texInputs += mCompositingStage->input[i]->getSamplersOut();
		}
	}
}

void PostProcessingPipeline::draw() {
	if (mOutputFB) mOutputFB->activate(0);
	Geometry::getScreenQuad()->render();
}

void PostProcessingPipeline::resize(unsigned int width, unsigned int height) {
	mWindowWidth = width;
	mWindowHeight = height;
	if (mInputFB) mInputFB->resize(mWindowWidth, mWindowHeight);
	for (auto buffer : mProcessingStages)
		buffer.output->resize(mWindowWidth, mWindowHeight);
}

void PostProcessingPipeline::enableDrawing() {
	if (mInputFB) mInputFB->setActive();
}

void PostProcessingPipeline::clear() {
	for (auto stage : mProcessingStages)
		delete stage.output;
	mProcessingStages.clear();
}
