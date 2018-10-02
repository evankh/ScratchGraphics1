#include "Game.h"
#include "InputComponent.h"
#include "KeyboardHandler.h"
#include "Level.h"
#include "Program.h"
#include "ServiceLocator.h"
#include "Shader.h"
#include "Sound.h"
#include "SoundHandler.h"
#include "Texture.h"
#include "Window.h"

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
		index.extract("\\S\\L\\I \\I\\L", &window);
		mWindow->resize(window.w, window.h);	// Not sure why this doesn't work on the first load
		mWindow->rename(window.title);
		delete window.title;
		mScreenCamera = new OrthoCamera(mWindow->getWidth(), mWindow->getHeight());
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 2.0f));
		view = glm::rotate(view, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		view = glm::rotate(view, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mSceneCamera = new PerspCamera(view, mWindow->getWidth(), mWindow->getHeight(), 75.0f);
		mSoundSystem.registerListener(mSceneCamera->getPhysics());
		// Loading InputComponents - I think this will not last long, I will switch over to function pointers instead ( (void)update(State*,Event) or similar )
		mInputs.add("player1", new KeyboardInputComponent(4,"wasd"));
		mInputs.add("player2", new KeyboardInputComponent(4, "ijkl"));
		//mInputs.add("mouse", new KeyboardInputComponent(2, { true,true,false,false,false }));	// Need a different way to do that
		// Loading asset directories
		char* workingDirectory;
		while (index.good()) {
			if (index.extract("//\\S\\L", NULL));
			else if (index.extract("geometry:\"\\S\"\\L", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the geometry data
					while (workingIndex.good()) {
						struct { char* name, *filepath; } geometry;
						if (workingIndex.extract("//\\S\\L", NULL));
						else if (workingIndex.extract("\"\\S\":\"\\S\"\\L", &geometry)) {
							Geometry* geom = new Geometry((mAssetBasePath + workingDirectory + geometry.filepath).data());
							mGeometries.add(geometry.name, geom);
							//geom->transfer();	// Apparently geometry gets transferred when it's used by an object, so no need to do it separately!
							delete geometry.name;
							delete geometry.filepath;
						}
						else if (workingIndex.extract("\\S\\L", &geometry.name)) {
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
			else if (index.extract("shaders:\"\\S\"\\L", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the shader data
					while (workingIndex.good()) {
						char* shaderName;
						unsigned int type;
						struct { char* name, *vs, *fs, *gs; } programData;
						if (workingIndex.extract("//\\S\\L", NULL)) continue;
						else if (workingIndex.extract("Vertex \"\\S\"", &shaderName))
							type = GL_VERTEX_SHADER;
						else if (workingIndex.extract("Fragment \"\\S\"", &shaderName))
							type = GL_FRAGMENT_SHADER;
						else if (workingIndex.extract("Geometry \"\\S\"", &shaderName))
							type = GL_GEOMETRY_SHADER;
						else if (workingIndex.extract("Program \"\\S\" Vertex:\"\\S\" Fragment:\"\\S\"", &programData)) {
							Program* program = new Program;
							program->attach(mShaders.get(programData.vs), GL_VERTEX_SHADER);
							program->attach(mShaders.get(programData.fs), GL_FRAGMENT_SHADER);
							if (workingIndex.extract(" Geometry:\"\\S\"", &programData.gs)) {
								program->attach(mShaders.get(programData.gs), GL_GEOMETRY_SHADER);
								delete programData.gs;
							}
							program->link();
							program->validate();
							program->detachAll();
							mPrograms.add(programData.name, program);
							delete programData.name;
							delete programData.vs;
							delete programData.fs;
							continue;
						}
						else {
							if (workingIndex.extract("\\?S\\L", &shaderName)) {
								if (shaderName) {
									ServiceLocator::getLoggingService().error("Unexpected character in index file", shaderName);
									delete shaderName;
								}
								continue;
							}
							else {	// File is empty
								break;
							}
						}
						// Extract and load all versions
						struct { int version; char* filepath; } versionedFile;
						while (workingIndex.extract(" \\I:\"\\S\"", &versionedFile)) {
							mShaders.add(shaderName, versionedFile.version, new Shader((mAssetBasePath + workingDirectory + versionedFile.filepath).data(), type));
							delete versionedFile.filepath;
						}
						delete shaderName;
						// Handle the uniforms I guess
						if (!workingIndex.extract("\\L", NULL)) {
							workingIndex.extract("\\S\\L", &shaderName);
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
			else if (index.extract("post:\"\\S\"\\L", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				mGameObjectsPost.init(mWindow->getWidth(), mWindow->getHeight());
				mMenuPost.init(mWindow->getWidth(), mWindow->getHeight());
				if (workingIndex.good()) {
					while (workingIndex.good()) {
						// Extract the postprocessing data
						struct { char* name; int samples; char* path; } shaderData;
						struct { char* name; Kernel kernel; } kernelData;
						struct { char* name, *sampler, *processor, *kernel; } filterData;
						if (workingIndex.extract("//\\S\\L", NULL));
						else if (workingIndex.extract("Sampler \"\\S\" \\I \"\\S\"\\L", &shaderData)) {
							// Need a separate PostProcessingManager to hold these and pull out the ones with the right number of samples, but they can go in the regular shader manager for now
							mPostShaders.add(shaderData.name, shaderData.samples, new Shader(mAssetBasePath + workingDirectory + shaderData.path, GL_VERTEX_SHADER));
							delete shaderData.name;
							delete shaderData.path;
						}
						else if (workingIndex.extract("Processor \"\\S\" \\I \"\\S\"\\L", &shaderData)) {
							// Need a separate PostProcessingManager to hold these and pull out the ones with the right number of samples, but they can go in the regular shader manager for now
							mPostShaders.add(shaderData.name, shaderData.samples, new Shader(mAssetBasePath + workingDirectory + shaderData.path, GL_FRAGMENT_SHADER));
							delete shaderData.name;
							delete shaderData.path;
						}
						else if (workingIndex.extract("Kernel \"\\S\" \\I", &kernelData)) {
							if (workingIndex.extract(" [", NULL)) {
								kernelData.kernel.weights = new float[kernelData.kernel.samples];
								// TODO: Checking the number of samples (don't need to store it, can use a while loop & a counter)
								for (int i = 0; i < kernelData.kernel.samples - 1; i++)
									workingIndex.extract("\\F,", &kernelData.kernel.weights[i]);
								workingIndex.extract("\\F]\\L", &kernelData.kernel.weights[kernelData.kernel.samples - 1]);
								mKernels.add(kernelData.name, kernelData.kernel);
							}
							else {
								char* err;
								workingIndex.extract("\\S\\L", &err);
								ServiceLocator::getLoggingService().error("Unexpected characters in kernel definition", err);
								delete err;
							}
							delete kernelData.name;
						}
						else if (workingIndex.extract("Filter \"\\S\" Sampler:\"\\S\" Processor:\"\\S\"", &filterData)) {
							Shader* sampler = mPostShaders.get(filterData.sampler);
							Shader* processor = mPostShaders.get(filterData.processor);
							if (sampler && processor) {
								Program* program = new Program(sampler, processor);
								if (workingIndex.extract(" Kernel:\"\\S\"", &filterData.kernel)) {
									Kernel kernel = mKernels.get(filterData.kernel);
									if (kernel.weights) {
										mGameObjectsPost.attach(program, kernel);
										mFilters.add(filterData.name, program);	// Doesn't have a way to store the kernel - is this a problem? When do we pull it out of here?
									}
									else {
										ServiceLocator::getLoggingService().error("Cannot locate kernel", filterData.kernel);
										delete program;
									}
									delete filterData.kernel;
								}
								else {
									mGameObjectsPost.attach(program);
									mFilters.add(filterData.name, program);
								}
							}
							else {
								if (!sampler)
									ServiceLocator::getLoggingService().error("Cannot locate sampler", filterData.sampler);
								if (!processor)
									ServiceLocator::getLoggingService().error("Cannot locate processor", filterData.processor);
							}
							delete filterData.name;
							delete filterData.sampler;
							delete filterData.processor;
							if (!workingIndex.extract("\\L", NULL)) {
								char* err;
								workingIndex.extract("\\?S\\L", &err);
								ServiceLocator::getLoggingService().error("Unexpected characters in filter definition", err);
								delete err;
							}
						}
						else if (workingIndex.extract("\\S\\L", &shaderData.name)) {
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
			else if (index.extract("levels:\"\\S\"\\L", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the level data
					while (workingIndex.good()) {
						struct { char* name, *filepath; } level;
						if (workingIndex.extract("//\\S\\L", NULL));
						else if (workingIndex.extract("\"\\S\":\"\\S\"\\L", &level)) {
							mLevels.add(level.name, new Level(mAssetBasePath + workingDirectory + level.filepath));
							delete level.name;
							delete level.filepath;
						}
						else if (workingIndex.extract("\\S\\L", &level.name)) {
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
			else if (index.extract("textures:\"\\S\"\\L", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					while (workingIndex.good()) {
						struct { char* name, *path; } textureData;	// Maybe need to store the resolutions too, depends on what gli offers
						if (workingIndex.extract("//\\S\\L", NULL));
						else if (workingIndex.extract("Texture \"\\S\" \"\\S\"\\L", &textureData)) {
							mTextures.add(textureData.name, new Texture(mAssetBasePath + workingDirectory + textureData.path));
							delete textureData.name;
							delete textureData.path;
						}
						else if (workingIndex.extract("\\S\\L", &textureData.name)) {
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
			else if (index.extract("sounds:\"\\S\"\\L", &workingDirectory)) {
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					while (workingIndex.good()) {
						struct { char* name, *path; } soundData;
						if (workingIndex.extract("//\\S\\L", NULL));
						else if (workingIndex.extract("Procedural \"\\S\" \"\\S\"\\L", &soundData)) {
							mSounds.add(soundData.name, new ProceduralSound(mAssetBasePath + workingDirectory + soundData.path));
							delete soundData.name;
							delete soundData.path;
						}
						else if (workingIndex.extract("File \"\\S\" \"\\S\"\\L", &soundData)) {
							mSounds.add(soundData.name, new FileSound(mAssetBasePath + workingDirectory + soundData.path));
							delete soundData.name;
							delete soundData.path;
						}
						else if (workingIndex.extract("\\S\\L", &soundData.name)) {
							ServiceLocator::getLoggingService().error("Unexpected line in sound index file", soundData.name);
							delete soundData.name;
						}
					}
				}
				else {
					ServiceLocator::getLoggingService().badFileError(mAssetBasePath + workingDirectory + mIndexFilename);
				}
				workingIndex.close();
				delete workingDirectory;
			}
			else if (index.extract("\\S\\L", &workingDirectory)) {
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
	mFilters.clear_delete();
	mPostShaders.clear();
	mKernels.clear();
	mSounds.clear();
	mLevels.clear_delete();
	for (auto item : mHUDItems)
		delete item;
	mHUDItems.clear();
	mGeometries.clear_delete();
	mPrograms.clear_delete();
	mShaders.clear();
	mInputs.clear_delete();
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
	mFilters.get("none")->use();
	mGameObjectsPost.draw();
	for (auto hud : mHUDItems) {
		hud->render(mScreenCamera);
	}

	if (mIsMenuActive) {
		mMenuPost.process();
		mWindow->enableDrawing();
		mFilters.get("none")->use();
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
	ServiceLocator::getLoggingService().log("======== RELOADING ALL ========");
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
