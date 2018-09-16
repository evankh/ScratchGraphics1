#include "Level.h"
#include "ServiceLocator.h"

// I can't believe this actually works
NamedContainer<Geometry*>& Level::sGeometryLibrary = Game::getInstance().mGeometries;
ShaderManager& Level::sShaderLibrary = Game::getInstance().mShaders;
NamedContainer<Texture*>& Level::sTextureLibrary = Game::getInstance().mTextures;
NamedContainer<InputComponent*>& Level::sInputLibrary = Game::getInstance().mInputs;
NamedContainer<PhysicsComponent*>& Level::sPhysicsLibrary = Game::getInstance().mPhysics;

Level::Level(const char* filepath) {
	// Blah blah blah
	FileService& file = ServiceLocator::getFileService(filepath);
	if (file.good()) {

	}
	else {
		ServiceLocator::getLoggingService().badFileError(filepath);
	}
	file.close();
}

Level::Level(std::string filepath) :Level(filepath.data()) {}

Level::~Level() {
	for (auto object : mGameObjects)
		delete object;
	mGameObjects.clear();
}
