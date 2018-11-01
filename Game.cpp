#include "Game.h"
#include "Bounds.h"
#include "Geometry.h"
#include "KeyboardHandler.h"
#include "Level.h"
#include "UI/Menu.h"
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
	KeyboardHandler::getInstance().registerReceiver("rb", this);
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
		// Loading InputComponents - I think this will not last long, I will switch over to function pointers instead ( (void)update(State*,Event) or similar )
		//mInputs.add("player1", new KeyboardInputComponent(4,"wasd"));
		//mInputs.add("player2", new KeyboardInputComponent(4, "ijkl"));
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
							mShaders.add(shaderName, versionedFile.version, new Shader(mAssetBasePath + workingDirectory + versionedFile.filepath, type));
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
					mSoundSystem.registerListener(mCurrentLevel->getCurrentCamera()->getPhysics());
					mCurrentLevel->setBackgroundMusicVolume(0.25);
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
	mFilters.clear();
	mPostShaders.clear();
	mKernels.clear();
	mSounds.clear();
	mLevels.clear();
	while (mCurrentMenu) {
		auto top = mCurrentMenu;
		mCurrentMenu = top->mParent;
		delete top;
	}
	mGeometries.clear();
	mPrograms.clear();
	mShaders.clear();
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
	mSoundSystem.update();
	// Collision detection
	// Yo, I just really want to do this in the least efficient way possible
	auto objects = mCurrentLevel->getObjectList();
	// A compelling reason to separate out PhysicsComponents, and possibly Bounds
	for (auto i = objects.begin(); i != objects.end(); i++) {
		if (i->second->getBounds()) {
			for (auto j = i; j != objects.end(); j++) {
				if (j->second->getBounds()) {
					if (collides(i->second->getBounds(), j->second->getBounds())) {
						CollisionData collision;
						collision.first = i->second->getPhysicsComponent();
						collision.second = j->second->getPhysicsComponent();
						i->second->handle(Event(collision));
						j->second->handle(Event(collision));
					}
				}
			}
		}
	}
	// Object updates
	for (auto object : mCurrentLevel->getObjectList()) {
		object.second->update(dt);	// A compelling reason to separate further into Components which can be updated individually
	}
}

// Hey look, time to bring this back in again
#include <glm/gtc/type_ptr.hpp>

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
		object.second->render(mCurrentLevel->getCurrentCamera());
	}

	float debug_collision[]{ 0.0f,1.0f,0.0f,0.5f }, debug_nocollision[]{ 0.0f,0.0f,1.0f,0.5f };
	if (mDebugMode) {
		auto program = mPrograms.get("debug_bbs");
		program->use();
		program->sendUniform("uVP", glm::value_ptr(mCurrentLevel->getCurrentCamera()->getViewProjectionMatrix()));
		for (auto object : mCurrentLevel->getObjectList()) {
			if (object.second->hasCollision())
				program->sendUniform("uDebugColor", 4, debug_collision);
			else
				program->sendUniform("uDebugColor", 4, debug_nocollision);
			program->sendUniform("uM", glm::value_ptr(object.second->getPhysicsComponent()->getModelMatrix()));
			object.second->getBounds()->debugDraw();
		}

		program = mPrograms.get("debug_axes");
		program->use();
		program->sendUniform("uVP", glm::value_ptr(mCurrentLevel->getCurrentCamera()->getViewProjectionMatrix()));
		program->sendUniform("uM", glm::value_ptr(glm::mat4(1.0f)));
		Geometry::drawAxes();
		for (auto object : mCurrentLevel->getObjectList()) {
			program->sendUniform("uM", glm::value_ptr(object.second->getPhysicsComponent()->getModelMatrix()));
			Geometry::drawAxes();
		}
	}
	mGameObjectsPost.process();

	if (mCurrentMenu)
		mMenuPost.enableDrawing();
	else
		mWindow->enableDrawing();
	mFilters.get("none")->use();
	mGameObjectsPost.draw();

	if (mCurrentMenu) {
		mCurrentMenu->draw();
		mMenuPost.process();
		mWindow->enableDrawing();
		mFilters.get("none")->use();
		mMenuPost.draw();
	}
}

void Game::resize(unsigned int width, unsigned int height) {
	// Presumably OpenGL won't let me resize it to something impossible, so there's no need to check the dimensions
	mWindow->resize(width, height);
	for (auto a : mCurrentLevel->getCameraList())
		a.second->resize(width, height);
	mGameObjectsPost.resize(width, height);
	mMenuPost.resize(width, height);
	if (mCurrentMenu) mCurrentMenu->layout(width, height);
}

void Game::reloadAll() {
	ServiceLocator::getLoggingService().log("======== RELOADING ALL ========");
	cleanup();
	load();
}

void Game::handle(Event event) {
	switch (event.mType) {
	case EventType::KEY_PRESSED:
		switch (event.mData.keyboard.key) {
		case 'r':
			reloadAll();
			break;
		case 'b':
			mDebugMode = !mDebugMode;
			if (mDebugMode)
				ServiceLocator::getLoggingService().log("======== DEBUG ON ========");
			else
				ServiceLocator::getLoggingService().log("======== DEBUG OFF ========");
			break;
		case 27:
			// Pause / unpause
			// In a comment around here somewhere I have the code snippet to push a menu on the stack
			break;
		}
		break;
	}
}
