#include "FrameBuffer.h"
#include "Geometry.h"
#include "ServiceLocator.h"
#include "GL\glew.h"

#define NULL 0

float magicQuadData[20] = { -1.0f,-1.0f,0.0f, 0.0f,0.0f, 1.0f,-1.0f,0.0f, 1.0f,0.0f, -1.0f,1.0f,0.0f, 0.0f,1.0f, 1.0f,1.0f,0.0f, 1.0f,1.0f };
unsigned int magicQuadFaces[6] = { 0,1,2, 2,1,3 };
Geometry FrameBuffer::sMagicQuad = Geometry(4, magicQuadData, 2, magicQuadFaces, { A_POSITION,A_TEXCOORD0 });

FrameBuffer::FrameBuffer(unsigned int windowWidth, unsigned int windowHeight, float scale) {
	mRelativeScale = scale;
	glGenFramebuffers(1, &mHandle);

	// Attach the typical images to it: Color, Depth
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	glGenTextures(1, &mColorTextureHandle);
	glBindTexture(GL_TEXTURE_2D, mColorTextureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)(mRelativeScale * windowWidth), (int)(mRelativeScale * windowHeight), 0, GL_RGBA, GL_FLOAT, NULL);

	glGenTextures(1, &mDepthTextureHandle);
	glBindTexture(GL_TEXTURE_2D, mDepthTextureHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)(mRelativeScale * windowWidth), (int)(mRelativeScale * windowHeight), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTextureHandle, 0);
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
	sMagicQuad.transfer();
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &mHandle);
	glDeleteTextures(1, &mColorTextureHandle);
	glDeleteTextures(1, &mDepthTextureHandle);
}

void FrameBuffer::setActive() {
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::draw() {
	// Draw the Magic Quad to the active FrameBuffer (which should already be set) using the active Program (which should already be set)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mColorTextureHandle);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mDepthTextureHandle);
	sMagicQuad.render();
	glActiveTexture(GL_TEXTURE0);
}

void FrameBuffer::resize(unsigned int width, unsigned int height) {
	glBindTexture(GL_TEXTURE_2D, mColorTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)(width * mRelativeScale), (int)(height * mRelativeScale), 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, mDepthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)(width * mRelativeScale), (int)(height * mRelativeScale), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
