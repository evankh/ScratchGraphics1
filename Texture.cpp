#include "Texture.h"
#include "ServiceLocator.h"

Texture::Texture(std::string filepath) {
	mFilepath = filepath;
	FileService& file = ServiceLocator::getFileService(mFilepath);
	if (file.good()) {
		// Maybe a mistake to try writing the image loading code before even installing gli...
		file.close();
	}
	else {
		ServiceLocator::getLoggingService().error("Error opening file", mFilepath);
	}
}

Texture::~Texture() {
	// Stuff
	// I may need some more sophisticated controls for managing the lifetime on the GPU
}