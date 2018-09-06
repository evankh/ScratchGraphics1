#include "Level.h"

// I can't believe this actually works
NamedContainer<Geometry*>& Level::sGeometryLibrary = Game::getInstance().mGeometries;
ShaderManager& Level::sShaderLibrary = Game::getInstance().mShaders;
NamedContainer<Texture*>& Level::sTextureLibrary = Game::getInstance().mTextures;
NamedContainer<InputComponent*>& Level::sInputLibrary = Game::getInstance().mInputs;
NamedContainer<PhysicsComponent*>& Level::sPhysicsLibrary = Game::getInstance().mPhysics;

Level::Level(const char* filepath) {
	// Blah blah blah
}

Level::~Level() {
	for (auto object : mGameObjects)
		delete object;
	mGameObjects.clear();
}
