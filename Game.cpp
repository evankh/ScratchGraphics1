#include "Game.h"
#include "Program.h"
#include "KeyboardHandler.h"
#include "Shader.h"
#include "Window.h"
#include "ServiceLocator.h"
#include "Level.h"

#include "cubedata_temp.h"
#include "squaredata_temp.h"

#include "GL\glew.h"	// I literally just need this for GL_VERTEX_SHADER, which is a bit annoying
#include "glm\gtc\matrix_transform.hpp"	// And this one only for the mSceneCamera transforms, which should be done elsewhere

Game::Game() {
	// ?
	// I guess this would be the spot for the initialization of everything: Loading assets, starting services, initializing rendering, etc.
//	mWindow = new Window(800, 600, "Game owns this window");
	KeyboardHandler::getInstance().registerReceiver('r', this);
	KeyboardHandler::getInstance().registerReceiver(27, this);
}

void Game::init() {
	mWindow = new Window(800, 600, "Game owns this window");
	// Set up the window and cameras
	mScreenCamera = new OrthoCamera(mWindow->getWidth(), mWindow->getHeight());
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 2.0f));
	view = glm::rotate(view, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	view = glm::rotate(view, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mSceneCamera = new PerspCamera(view, mWindow->getWidth(), mWindow->getHeight(), 75.0f);
}

void Game::load() {
	FileService& index = ServiceLocator::getFileService(mAssetBasePath + mIndexFilename);
	if (index.good()) {
		// Generic game data
		struct { char* title; int w, h; } window;
		index.extract("\\S\n\\I \\I\n", &window);
		mWindow->resize(window.w, window.h);
		mWindow->rename(window.title);
		delete window.title;
		// Loading asset directories
		char* workingDirectory;
		while (index.good()) {
			if (index.extract("geometry:\"\\S\"\n", &workingDirectory)) {
				ServiceLocator::getLoggingService().error("Found geometry asset path", mAssetBasePath + workingDirectory);
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the geometry data
					while (workingIndex.good()) {
						struct { char* name, *filepath; } geometry;
						while (workingIndex.extract("\\S:\"\\S\"\n", &geometry)) {
							ServiceLocator::getLoggingService().error(geometry.name, geometry.filepath);
							mGeometries.add(geometry.name, new Geometry((mAssetBasePath + workingDirectory + geometry.filepath).data()));
							delete geometry.name;
							delete geometry.filepath;
						}
					}
				}
				else {
					ServiceLocator::getLoggingService().error("Error opening geometry index", mAssetBasePath + workingDirectory + mIndexFilename);
				}
				delete workingDirectory;
			}
			else if (index.extract("shaders:\"\\S\"\n", &workingDirectory)) {
				ServiceLocator::getLoggingService().error("Found shader asset path", mAssetBasePath + workingDirectory);
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the shader data
					while (workingIndex.good()) {
						char* shaderName;
						unsigned int type;
						if (workingIndex.extract("Vertex \"\\S\" ", &shaderName))
							type = GL_VERTEX_SHADER;
						else if (workingIndex.extract("Fragment \"\\S\" ", &shaderName))
							type = GL_FRAGMENT_SHADER;
						else if (workingIndex.extract("Geometry \"\\S\" ", &shaderName))
							type = GL_GEOMETRY_SHADER;
						else {
							if (workingIndex.extract("\\S\n", &shaderName)) {
								ServiceLocator::getLoggingService().error("Unexpected character in index file", shaderName);
								delete shaderName;
								continue;
							}
							else {	// File is empty
								ServiceLocator::getLoggingService().log("Finished parsing shader index.");
								break;
							}
						}
						// Extract and load all versions
						struct { int version; char* filepath; } versionedFile;
						while (workingIndex.extract("\\I:\"\\S\" ", &versionedFile)) {
							ServiceLocator::getLoggingService().error(shaderName, versionedFile.filepath);
							mShaders.add(shaderName, versionedFile.version, new Shader((mAssetBasePath + workingDirectory + versionedFile.filepath).data(), type));
							delete versionedFile.filepath;
						}
						delete shaderName;
						// Handle the uniforms I guess
						if (workingIndex.extract("\\S\n", &shaderName)) {
							ServiceLocator::getLoggingService().error("Found extra data at the end of the line", shaderName);
							delete shaderName;
						}
					}
				}
				else {
					ServiceLocator::getLoggingService().error("Error opening shader index", mAssetBasePath + workingDirectory + mIndexFilename);
				}
				delete workingDirectory;
			}
			else if (index.extract("post:\"\\S\"\n", &workingDirectory)) {
				ServiceLocator::getLoggingService().error("Found postprocessing asset path", mAssetBasePath + workingDirectory);
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the postprocessing data
				}
				else {
					ServiceLocator::getLoggingService().error("Error opening postprocessing index", mAssetBasePath + workingDirectory + mIndexFilename);
				}
				delete workingDirectory;
				mGameObjectsPost.init(mWindow->getWidth(), mWindow->getHeight());
				mMenuPost.init(mWindow->getWidth(), mWindow->getHeight());
			}
			else if (index.extract("levels:\"\\S\"\n", &workingDirectory)) {
				ServiceLocator::getLoggingService().error("Found level asset path", mAssetBasePath + workingDirectory);
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the level data
					while (workingIndex.good()) {
						struct { char* name, *filepath; } level;
						while (workingIndex.extract("\\S:\"\\S\"\n", &level)) {
							ServiceLocator::getLoggingService().error(level.name, level.filepath);
							mLevels.add(level.name, new Level((mAssetBasePath + workingDirectory + level.filepath).data()));
							delete level.name;
							delete level.filepath;
						}
					}
					mCurrentLevel = mLevels.get("debug_world");
				}
				else {
					ServiceLocator::getLoggingService().error("Error opening levels index", mAssetBasePath + workingDirectory + mIndexFilename);
				}
				delete workingDirectory;
			}
			else if (index.extract("textures:\"\\S\"\n", &workingDirectory)) {
				ServiceLocator::getLoggingService().error("Found texture asset path", mAssetBasePath + workingDirectory);
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the geometry data
				}
				else {
					ServiceLocator::getLoggingService().error("Error opening texture index", mAssetBasePath + workingDirectory + mIndexFilename);
				}
				delete workingDirectory;
			}
			else {
				if (index.extract("\\S\n", &workingDirectory)) {
					ServiceLocator::getLoggingService().error("Unrecognized string in index file", workingDirectory);
					delete workingDirectory;
				} else break;
			}
		}
	} else {
		ServiceLocator::getLoggingService().error("Could not open asset index file", mAssetBasePath + mIndexFilename);
		// Hard quit I guess
	}
}

void Game::cleanup() {
	mGameObjectsPost.clear();
	mMenuPost.clear();
	mLevels.clear_delete();
	for (auto item : mHUDItems)
		delete item;
	mHUDItems.clear();
	mGeometries.clear_delete();
	mPrograms.clear_delete();
	mShaders.clear();
	delete mSceneCamera;
	delete mScreenCamera;
	//KeyboardHandler::unregisterReceiver(this);
}

Game::~Game() {
	delete mWindow;
	KeyboardHandler::getInstance().unregisterReceiver(this);
}

Game& Game::getInstance() {
	static Game* sInstance = new Game();
	return *sInstance;
}

void Game::update(float dt) {
	// Handle events
	for (auto object : mCurrentLevel->getObjectList()) {
		object->update(dt);
	}
}

void Game::render(float dt) {
	// A broad sketch of the rendering process follows:
	//  - clear the various framebuffers and fill with an appropriate background
	//  - render all GameObjects to an intermediate FrameBuffer
	//  - run a post-processing pipeline, consisting of as many intermediate steps as necessary, and ultimately rendering to the window's FrameBuffer
	//  - render the Window FrameBuffer
	//  - render all HUD elements
	//  - if in a menu:
	//    - run a second post-processing step
	//    - render all menu items

	// Set the active framebuffer to an intermediate one
	mGameObjectsPost.enableDrawing();
	for (auto object : mCurrentLevel->getObjectList()) {
		object->render(mSceneCamera);
	}
	mGameObjectsPost.process();

	if (mIsMenuActive)
		mMenuPost.enableDrawing();
	else
		mWindow->enableDrawing();
	mPrograms.get("post_none")->use();
	mGameObjectsPost.draw();
	for (auto hud : mHUDItems) {
		hud->render(mScreenCamera);
	}

	if (mIsMenuActive) {
		mMenuPost.process();
		mWindow->enableDrawing();
		mPrograms.get("post_none")->use();
		mMenuPost.draw();
//		for (auto item : mCurrentMenu->items()) {
//			item.render(mScreenCamera);
//		}
	}
}

void Game::resize(unsigned int width, unsigned int height) {
	// Presumably OpenGL won't let me resize it to something impossible, so there's no need to check the dimensions
	mWindow->resize(width, height);
	mSceneCamera->resize(width, height);
	mScreenCamera->resize(width, height);
	mGameObjectsPost.resize(width, height);
	mMenuPost.resize(width, height);
	//mActiveMenu.layout(width, height);	// Yikes, that's gonna be a motherfucker of a function
}

void Game::reloadAll() {
	cleanup();
	load();
}

void Game::handle(Event event) {
	switch (event.mType) {
	case EKH_EVENT_KEY_PRESSED:
		switch (event.mData.keyboard.key) {
		case 'r':
			reloadAll();
			break;
		case 27:
			// Pause / unpause
			break;
		}
		break;
	}
}
