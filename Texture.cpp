#include "Texture.h"
#include "ServiceLocator.h"
#include "IL\ilut.h"

Texture::Texture(const char* filepath) {
	mFilepath = filepath;
	mHandle = ilutGLLoadImage(const_cast<char*>(filepath));
	// As it turns out it would not actually be that hard to implement PNGs myself, so worth considering in the future
	// Still not clear on what it does with the resolution... but is it even necessary for anything?
	if (mHandle) {
		// Configure OpenGL texture settings
		glBindTexture(GL_TEXTURE_2D, mHandle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		ServiceLocator::getLoggingService().badFileError(mFilepath);
	}
}

Texture::Texture(std::string filepath) :Texture(filepath.data()) {}

/*
Texture::Texture(std::string filepath) {
	mFilepath = filepath;
	mHandle = ilutGLLoadImage(mFilepath.data());
	if (mHandle) {
		// Configure texture settings
	}
	else {
		ServiceLocator::getLoggingService().error("Could not open file", mFilepath);
	}
}
*/

Texture::~Texture() {
	glDeleteTextures(1, &mHandle);
}

void Texture::setWrapX(bool shouldWrap) {
	glBindTexture(GL_TEXTURE_2D, mHandle);
	if (shouldWrap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setWrapY(bool shouldWrap) {
	glBindTexture(GL_TEXTURE_2D, mHandle);
	if (shouldWrap)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}