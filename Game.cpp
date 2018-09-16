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
}

void Game::load() {
	// Load assets from the folders & index files
	FileService& index = ServiceLocator::getFileService(mAssetBasePath + mIndexFilename);
	if (index.good()) {
		// Generic game data
		struct { char* title; int w, h; } window;
		index.extract("\\S\n\\I \\I\n", &window);
		mWindow->resize(window.w, window.h);
		mWindow->rename(window.title);
		delete window.title;
		mScreenCamera = new OrthoCamera(mWindow->getWidth(), mWindow->getHeight());
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 2.0f));
		view = glm::rotate(view, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		view = glm::rotate(view, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mSceneCamera = new PerspCamera(view, mWindow->getWidth(), mWindow->getHeight(), 75.0f);
		// Loading asset directories
		char* workingDirectory;
		while (index.good()) {
			if (index.extract("geometry:\"\\S\"\n", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the geometry data
					while (workingIndex.good()) {
						struct { char* name, *filepath; } geometry;
						if (workingIndex.extract("\\S:\"\\S\"\n", &geometry)) {
							mGeometries.add(geometry.name, new Geometry((mAssetBasePath + workingDirectory + geometry.filepath).data()));
							delete geometry.name;
							delete geometry.filepath;
						}
						else if (workingIndex.extract("\\S\n", &geometry.name)) {
							ServiceLocator::getLoggingService().error("Unexpected line in geometry index file", geometry.name);
							delete geometry.name;
						}
					}
				}
				else {
					ServiceLocator::getLoggingService().badFileError(mAssetBasePath + workingDirectory + mIndexFilename);
				}
				workingIndex.close();
				delete workingDirectory;
			}
			else if (index.extract("shaders:\"\\S\"\n", &workingDirectory)) {
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
								break;
							}
						}
						// Extract and load all versions
						struct { int version; char* filepath; } versionedFile;
						while (workingIndex.extract("\\I:\"\\S\" ", &versionedFile)) {
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
					ServiceLocator::getLoggingService().badFileError(mAssetBasePath + workingDirectory + mIndexFilename);
				}
				workingIndex.close();
				delete workingDirectory;
			}
			else if (index.extract("post:\"\\S\"\n", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				mGameObjectsPost.init(mWindow->getWidth(), mWindow->getHeight());
				mMenuPost.init(mWindow->getWidth(), mWindow->getHeight());
				if (workingIndex.good()) {
					while (workingIndex.good()) {
						// Extract the postprocessing data
						struct { char* name; int samples; char* path; } shaderData;
						struct { char* name; int samples; float* weights; } kernelData;
						struct { char* name, *sampler, *processor, *kernel; } filterData;
						if (workingIndex.extract("Sampler \"\\S\" \\I \"\\S\"\n", &shaderData)) {
							// Need a separate PostProcessingManager to hold these and pull out the ones with the right number of samples, but they can go in the regular shader manager for now
							mShaders.add(shaderData.name, shaderData.samples, new Shader(mAssetBasePath + workingDirectory + shaderData.path, GL_VERTEX_SHADER));
							delete shaderData.name;
							delete shaderData.path;
						}
						else if (workingIndex.extract("Processor \"\\S\" \\I \"\\S\"\n", &shaderData)) {
							// Need a separate PostProcessingManager to hold these and pull out the ones with the right number of samples, but they can go in the regular shader manager for now
							mShaders.add(shaderData.name, shaderData.samples, new Shader(mAssetBasePath + workingDirectory + shaderData.path, GL_FRAGMENT_SHADER));
							delete shaderData.name;
							delete shaderData.path;
						}
						else if (workingIndex.extract("Kernel \"\\S\" \\I", &kernelData)) {
							if (workingIndex.extract(" [", NULL)) {
								kernelData.weights = new float[kernelData.samples];
								for (int i = 0; i < kernelData.samples - 1; i++)
									workingIndex.extract("\\F,", &kernelData.weights[i]);
								workingIndex.extract("\\F]\n", &kernelData.weights[kernelData.samples - 1]);
								// Need a KernelManager to store these in
							}
							else {
								char* err;
								workingIndex.extract("\\S\n", &err);
								ServiceLocator::getLoggingService().error("Unexpected characters in kernel definition", err);
								delete err;
							}
							delete kernelData.name;
						}
						else if (workingIndex.extract("Filter \"\\S\" Sampler:\"\\S\" Processor:\"\\S\"", &filterData)) {
							// I will have to redo a fair bit of this after adding the separate data structures, but for right now I'm just desparate to get something onto the screen
							Program* program = new Program(mShaders.get(filterData.sampler), mShaders.get(filterData.processor));
							mPrograms.add(filterData.name, program);
							if (workingIndex.extract(" Kernel:\"\\S\"", &filterData.kernel)) {
								//mGameObjectsPost.attach(program, , mKernels.get(filterData.kernel));
								delete filterData.kernel;
							}
							else
								mGameObjectsPost.attach(program);
							if (!workingIndex.extract("\n", NULL)) {
								char* err;
								workingIndex.extract("\\S\n", &err);
								ServiceLocator::getLoggingService().error("Unexpected characters in filter definition", err);
								delete err;
							}
							delete filterData.name;
							delete filterData.sampler;
							delete filterData.processor;
						}
						else if (workingIndex.extract("\\S\n", &shaderData.name)) {
							ServiceLocator::getLoggingService().error("Unexpected line in postprocessing index", shaderData.name);
							delete shaderData.name;
						}
					}
				}
				else {
					ServiceLocator::getLoggingService().badFileError(mAssetBasePath + workingDirectory + mIndexFilename);
				}
				workingIndex.close();
				delete workingDirectory;
			}
			else if (index.extract("levels:\"\\S\"\n", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the level data
					while (workingIndex.good()) {
						struct { char* name, *filepath; } level;
						if (workingIndex.extract("\\S:\"\\S\"\n", &level)) {
							mLevels.add(level.name, new Level(mAssetBasePath + workingDirectory + level.filepath));
							delete level.name;
							delete level.filepath;
						}
						else if (workingIndex.extract("\\S\n", &level.name)) {
							ServiceLocator::getLoggingService().error("Unexpected line in level index file", level.name);
							delete level.name;
						}
					}
					mCurrentLevel = mLevels.get("debug_world");
				}
				else {
					ServiceLocator::getLoggingService().badFileError(mAssetBasePath + workingDirectory + mIndexFilename);
				}
				workingIndex.close();
				delete workingDirectory;
			}
			else if (index.extract("textures:\"\\S\"\n", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					while (workingIndex.good()) {
						struct { char* name, *path; } textureData;	// Maybe need to store the resolutions too, depends on what gli offers
						if (workingIndex.extract("Texture \"\\S\" \"\\S\"", &textureData)) {
							mTextures.add(textureData.name, new Texture(mAssetBasePath + workingDirectory + textureData.path));
							delete textureData.name;
							delete textureData.path;
						}
						else if (workingIndex.extract("\\S\n", &textureData.name)) {
							ServiceLocator::getLoggingService().error("Unexpected line in texture index file", textureData.name);
							delete textureData.name;
						}
					}
				}
				else {
					ServiceLocator::getLoggingService().badFileError(mAssetBasePath + workingDirectory + mIndexFilename);
				}
				workingIndex.close();
				delete workingDirectory;
			}
			else if (index.extract("\\S\n", &workingDirectory)) {
				ServiceLocator::getLoggingService().error("Unexpected line in base index file", workingDirectory);
				delete workingDirectory;
			}
		}
		index.close();
	} else {
		ServiceLocator::getLoggingService().badFileError(mAssetBasePath + mIndexFilename);
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
	mPrograms.get("none")->use();
	mGameObjectsPost.draw();
	for (auto hud : mHUDItems) {
		hud->render(mScreenCamera);
	}

	if (mIsMenuActive) {
		mMenuPost.process();
		mWindow->enableDrawing();
		mPrograms.get("none")->use();
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
