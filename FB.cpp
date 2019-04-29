#include "FB.h"

#include "GL/glew.h"
#include "ServiceLocator.h"

// This would make much more sense if I could somehow make my enum have members, e.g. AttachmentType::COLOR.size == GL_RGBA32F
// There may be some way to make that syntax work but I think it's more complicated than necessary
GLint attachmentSizes[] = {
	GL_RGBA32F,
	GL_R32UI,
	GL_DEPTH_COMPONENT
};
GLint attachmentFormats[] = {
	GL_RGBA,
	GL_RED_INTEGER,
	GL_DEPTH_COMPONENT
};
GLint attachmentTypes[] = {
	GL_FLOAT,
	GL_UNSIGNED_INT,
	GL_FLOAT
};
GLenum attachments[] = {
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

Framebuffer::Framebuffer(unsigned int width, unsigned int height, float baseScale) {
	mRelativeScale = baseScale;
	mWidth = (unsigned int)(width * baseScale);
	mHeight = (unsigned int)(height * baseScale);
	glGenFramebuffers(1, &mHandle);
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &mHandle);
	glDeleteTextures(mAttachments.size(), mAttachments.data());
	glDeleteTextures(1, &mDepthAttachment);
	mAttachments.clear();
}

void Framebuffer::attach(AttachmentType type) {
	unsigned int handle;
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, attachmentSizes[type], mWidth, mHeight, 0, attachmentFormats[type], attachmentTypes[type], NULL);
	if (type == ATTACHMENT_DEPTH) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, handle, 0);
		mDepthAttachment = handle;
	}
	else {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + mAttachments.size(), GL_TEXTURE_2D, handle, 0);
		mAttachments.push_back(handle);
		mTypes.push_back(type);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffers(mAttachments.size(), attachments);
}

void Framebuffer::validate() {
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			throw std::invalid_argument("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
			throw std::invalid_argument("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			throw std::invalid_argument("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		default:
			ServiceLocator::getLoggingService().error("FrameBuffer isn't complete", std::to_string((int)glCheckFramebufferStatus(GL_FRAMEBUFFER)));
			throw std::invalid_argument(std::to_string((int)glCheckFramebufferStatus(GL_FRAMEBUFFER)));
			break;
		}
}

void Framebuffer::resize(unsigned int width, unsigned int height) {
	mWidth = (unsigned int)(width * mRelativeScale);
	mHeight = (unsigned int)(height * mRelativeScale);
	for (unsigned int i = 0; i < mAttachments.size(); i++) {
		unsigned int handle = mAttachments[i];
		AttachmentType type = mTypes[i];
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexImage2D(GL_TEXTURE_2D, 0, attachmentSizes[type], mWidth, mHeight, 0, attachmentFormats[type], attachmentTypes[type], NULL);
	}
	glBindTexture(GL_TEXTURE_2D, mDepthAttachment);
	glTexImage2D(GL_TEXTURE_2D, 0, attachmentSizes[ATTACHMENT_DEPTH], mWidth, mHeight, 0, attachmentFormats[ATTACHMENT_DEPTH], attachmentTypes[ATTACHMENT_DEPTH], NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::setAsDrawingTarget() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mHandle);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, mWidth, mHeight);
}

void Framebuffer::setAsTextureSource(int start) {
	for (unsigned int i = 0; i < mAttachments.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i + start);
		glBindTexture(GL_TEXTURE_2D, mAttachments[i]);
	}
}
