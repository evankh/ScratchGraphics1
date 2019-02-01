#include "FrameBuffer.h"
#include "Geometry.h"
#include "ServiceLocator.h"
#include "GL\glew.h"

const GLenum FRAMEBUFFER_ATTACHMENTS[] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,
	GL_COLOR_ATTACHMENT9,
};

FrameBuffer::FrameBuffer(unsigned int windowWidth, unsigned int windowHeight, float scale, int samplersOut) {
	mRelativeScale = scale;
	mWidth = (int)(mRelativeScale * windowWidth);
	mHeight = (int)(mRelativeScale * windowHeight);
	mSamplersOut = samplersOut;
	mColorTextureHandles = new unsigned int[mSamplersOut];
	glGenFramebuffers(1, &mHandle);

	// Attach the typical images to it: Color, Depth
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	glGenTextures(mSamplersOut, mColorTextureHandles);
	for (int i = 0; i < mSamplersOut; i++) {
		glBindTexture(GL_TEXTURE_2D, mColorTextureHandles[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, FRAMEBUFFER_ATTACHMENTS[i], GL_TEXTURE_2D, mColorTextureHandles[i], 0);
	}
	glDrawBuffers(mSamplersOut, FRAMEBUFFER_ATTACHMENTS);

	glGenTextures(1, &mDepthTextureHandle);
	glBindTexture(GL_TEXTURE_2D, mDepthTextureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTextureHandle, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		default:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", std::to_string((int)glCheckFramebufferStatus(GL_FRAMEBUFFER)));
			break;
		}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Geometry::getScreenQuad()->transfer();
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &mHandle);
	glDeleteTextures(mSamplersOut, mColorTextureHandles);
	glDeleteTextures(1, &mDepthTextureHandle);
}

void FrameBuffer::setActive() {
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, mWidth, mHeight);
}

void FrameBuffer::draw() {
	// Draw the Magic Quad to the active FrameBuffer (which should already be set) using the active Program (which should already be set)
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mColorTextureHandle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mDepthTextureHandle);*/
	for (int i = 0; i < mSamplersOut; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mColorTextureHandles[i]);
	}
	glActiveTexture(GL_TEXTURE0 + mSamplersOut);
	glBindTexture(GL_TEXTURE_2D, mDepthTextureHandle);
	Geometry::getScreenQuad()->render();
	glActiveTexture(GL_TEXTURE0);
}

void FrameBuffer::resize(unsigned int width, unsigned int height) {
	mWidth = (int)(mRelativeScale * width);
	mHeight = (int)(mRelativeScale * height);
	for (int i = 0; i < mSamplersOut; i++) {
		glBindTexture(GL_TEXTURE_2D, mColorTextureHandles[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	glBindTexture(GL_TEXTURE_2D, mDepthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
