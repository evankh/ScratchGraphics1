#include "Graphics/PostProcessingPipeline.h"

#include "Graphics/FrameBuffer.h"
#include "Components/Geometry.h"
#include "Graphics/Program.h"

PostprocessingPipeline::PostprocessingPipeline(unsigned int windowWidth, unsigned int windowHeight) {
	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;
	mInput = mOutput = new Framebuffer(windowWidth, windowHeight);
	mInput->attach(ATTACHMENT_COLOR);
	mInput->attach(ATTACHMENT_DEPTH);
}

PostprocessingPipeline::~PostprocessingPipeline() {
	delete mInput;
	for (StageData stage : mStages) {
		delete stage.output;
		stage.kernels.clear();
	}
	mStages.clear();
	mCompositingInputs.clear();
	if (mCompositor) {
		delete mCompositor->output;
		delete mCompositor;
	}
}

void PostprocessingPipeline::attach(StageData &data, bool isCompositingInput) {
	if (mStages.size() > 0) {
		if (mStages.back().filter->getSamplesOut() != data.filter->getSamplesIn()) throw std::invalid_argument("That stage won't fit here in this pipeline");
		data.input = mStages.back().output;
		data.scale *= mStages.back().scale;
	}
	else data.input = mInput;
	data.output = new Framebuffer(mWindowWidth, mWindowHeight, data.scale);
	// Assume for now that all postprocessing filters will be dealing only with colors; the integer texture will be reserved for the MouseHandler to use
	for (int i = 0; i < data.filter->getSamplesOut(); i++)
		data.output->attach(ATTACHMENT_COLOR);
	data.output->attach(ATTACHMENT_DEPTH);
	mOutput = data.output;
	mStages.push_back(data);
	if (isCompositingInput) mCompositingInputs.push_back(mOutput);
	// Uniforms?
	mOutput->validate();
}

void PostprocessingPipeline::attachCompositor(Program* compositor) {
	if (mCompositor) {
		delete mCompositor->output;
		delete mCompositor;
	}
	mCompositor = new StageData;
	mCompositor->filter = compositor;
	mCompositor->scale = 1.0f;
	mCompositor->output = new Framebuffer(mWindowWidth, mWindowHeight);
	mOutput = mCompositor->output;
	mOutput->attach(ATTACHMENT_COLOR);
	mOutput->attach(ATTACHMENT_DEPTH);
	mOutput->validate();
}

void PostprocessingPipeline::process() {
	for (auto stage : mStages) {
		stage.filter->use();
		stage.input->setAsTextureSource();
		stage.output->setAsDrawingTarget();
		// Activate whatever uniforms. Once again this would be simpler and cleaner if Program were keeping track of its own uniforms.
		stage.filter->sendUniform("uPixWidth", stage.input->getPixelWidth());
		stage.filter->sendUniform("uPixHeight", stage.input->getPixelHeight());
		if (stage.kernels.size() == 1)
			stage.filter->sendUniform("uKernel", 1, stage.kernels[0].samples, stage.kernels[0].weights);
		else for (unsigned int i = 0; i < stage.kernels.size(); i++)
			stage.filter->sendUniform((std::string("uKernel") + std::to_string(i)).c_str(), 1, stage.kernels[i].samples, stage.kernels[i].weights);
		GeometryComponent::getScreenQuad()->render();
	}
	if (mCompositor) {
		mCompositor->filter->use();
		mCompositor->filter->sendUniform("uNumTextures", mCompositingInputs.size());
		int texInputs = 0;
		for (auto stage : mCompositingInputs) {
			stage->setAsTextureSource(texInputs);
			texInputs += stage->getNumAttachments();
		}
		mCompositor->output->setAsDrawingTarget();
		GeometryComponent::getScreenQuad()->render();
	}
}

void PostprocessingPipeline::resize(unsigned int width, unsigned int height) {
	mWindowWidth = width;
	mWindowHeight = height;
	mInput->resize(width, height);
	for (auto stage : mStages)
		stage.output->resize(width, height);
	if (mCompositor)
		mCompositor->output->resize(width, height);
}

void PostprocessingPipeline::setAsDrawTarget() {
	mInput->setAsDrawingTarget();
}

void PostprocessingPipeline::setAsTextureSource() {
	mOutput->setAsTextureSource();
}
