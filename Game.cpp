#include "Game.h"
#include "Bounds.h"
#include "Geometry.h"
#include "KeyboardHandler.h"
#include "Level.h"
#include "UI/Menu.h"
#include "UI/UIElement.h"
#include "MouseHandler.h"
#include "Program.h"
#include "ServiceLocator.h"
#include "Shader.h"
#include "Sound.h"
#include "State.h"
#include "SoundHandler.h"
#include "Texture.h"
#include "Window.h"

NamedContainer<State*> State::sBaseStateLibrary;	// This goes here for some reason

Game::Game() {
	// ?
	// I guess this would be the spot for the initialization of everything: Loading assets, starting services, initializing rendering, etc.
//	mWindow = new Window(800, 600, "Game owns this window");
	KeyboardHandler::getInstance().registerReceiver("rRbp+", this);
	KeyboardHandler::getInstance().registerReceiver(27, this);
}

void Game::init() {
	mWindow = new Window(800, 600, "Game owns this window");
	// Set up the MouseHandler textures, probably
	// Really probably ought to move GLEW initialization (and other inits) in here, so I know where they're being initialized
	// And in what order instead of aving them spread across several different files
}

void Game::load() {
	// Load certain very important objects (Shaders?)
	mCommonLibraries.standard.geometries.add("plane", Geometry::getNewQuad());
	// Load everything else from file
	FileService baseIndex(mAssetBasePath + mIndexFilename);
	if (!baseIndex.good()) throw "Asset file could not be opened.";
	struct { char *name, *path; } levelData;
	struct { int w, h; } resolution;
	char* commonFolder, *ppp;
	while (baseIndex.good()) {
		if (baseIndex.extract("//`S`L", NULL));	// Comment
		else if (baseIndex.extract("Level \"`S\" \"`S\"`L", &levelData)) {
			mLevelDirectory.add(levelData.name, levelData.path);
			delete[] levelData.name;
			delete[] levelData.path;
		}
		else if (baseIndex.extract("start: ", NULL)) {
			if (baseIndex.extract("Level \"`S\"`L", &levelData.name)) {
				std::string levelPath;
				try {
					levelPath = mLevelDirectory.get(levelData.name);
				}
				catch (std::exception) {
					throw std::exception((std::string("Starting level has not been defined: ") + levelData.name + mIndexFilename).data());
				}
				try {
					mCurrentLevel = loadLevel(mAssetBasePath + levelPath);
				}
				catch (std::exception e) {
					ServiceLocator::getLoggingService().error("Starting level", e.what());
					throw e;
				}
			}
			else if (baseIndex.extract("Menu \"`S\"`L", &levelData.name)) {
				try {
					mCurrentMenu = new Menu(NULL, mCommonLibraries.menus.get(levelData.name));
					delete[] levelData.name;
				}
				catch (std::exception) {
					throw std::exception((std::string("Starting menu has not been defined: ") + levelData.name).data());
				}
			}
			else {
				throw std::exception("Malformed start location.");
			}
		}
		else if (baseIndex.extract("resolution: [`I, `I]`L", &resolution)) {
			resize(resolution.w, resolution.h);
		}
		else if (baseIndex.extract("title: \"`S\"`L", &levelData.name)) {
			mWindow->rename(levelData.name);
		}
		else if (baseIndex.extract("common: \"`S\"`L", &commonFolder)) {
			std::string commonPath = mAssetBasePath + commonFolder;
			FileService commonIndex(commonPath + mIndexFilename);
			if (!commonIndex.good()) throw std::exception("Common asset file could not be opened.");
			char* workingDirectory = NULL;
			while (commonIndex.good()) {
				try {
					if (commonIndex.extract("//`S`L", NULL));
					else if (commonIndex.extract("menus: \"`S\"`L", &workingDirectory)) {
						//parseMenuIndex(commonPath + workingDirectory, mCommonLibraries.menus);
						delete[] workingDirectory;
					}
					else if (commonIndex.extract("obj: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load objs into common obj directory
						parseGeometryIndex(commonPath + workingDirectory, mCommonLibraries.standard.geometries);
						delete[] workingDirectory;
					}
					else if (commonIndex.extract("sound: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load sounds into common sound directory
						//parseSoundIndex(commonPath + workingDirectory, mCommonLibraries.standard.sounds);
						delete[] workingDirectory;
					}
					else if (commonIndex.extract("tex: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load textures into common texture directory
						parseTextureIndex(commonPath + workingDirectory, mCommonLibraries.standard.textures);
						delete[] workingDirectory;
					}
					else if (commonIndex.extract("glsl: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load shaders into common shader directory
						parseShaderIndex(commonPath + workingDirectory, mCommonLibraries.standard.shaders, mCommonLibraries.standard.programs);
						delete[] workingDirectory;
					}
					else if (commonIndex.extract("post: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load postprocessing things into common postprocessing directory
						parsePostprocessingIndex(commonPath + workingDirectory, mCommonLibraries.post.shaders, mCommonLibraries.post.filters, mCommonLibraries.post.kernels, mCommonLibraries.post.pipelines);
						delete[] workingDirectory;
					}
					else if (commonIndex.extract("`S`L", &workingDirectory)) {
						ServiceLocator::getLoggingService().error("Unexpected line in common index file", workingDirectory);
						delete[] workingDirectory;
					}
				}
				catch (std::exception e) {
					ServiceLocator::getLoggingService().badFileError(e.what());
				}
			}
		}
		else if (baseIndex.extract("ppp: \"`S\"`L", &ppp)) {
			try {
				mCurrentPostProcessing = mCommonLibraries.post.pipelines.get(ppp);
			}
			catch (std::out_of_range) {
				ServiceLocator::getLoggingService().error("Not a valid default postprocessor", ppp);
				mCurrentPostProcessing = nullptr;
			}
			delete[] ppp;
		}
		else if (baseIndex.extract("`S`L", &levelData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in root index file", levelData.name);
			delete[] levelData.name;
		}
	}
	softReload();
}

void Game::softReload() {
	ServiceLocator::getLoggingService().log("===== LEVEL RESET =====");
	// Empty everything out
	if (mWorkingActiveCamera) delete mWorkingActiveCamera;
	mWorkingActiveCamera = NULL;
	for (unsigned int i = 0; i < mWorkingObjectList.size(); i++) delete mWorkingObjectList[i];
	mWorkingObjectList.clear();
	// Fill the working list of objects
	auto objectMap = mCurrentLevel->getObjectList();
	for (auto i : objectMap)
		mWorkingObjectList.push_back(i.second->copy());	// Possibly a good opportunity to create an array of actual GameObjects instead of their pointers
	mWorkingActiveCamera = mCurrentLevel->getCurrentCamera()->copy();
}

void Game::cleanup() {
	delete mCurrentLevel;
	mCurrentLevel = nullptr;
	mLevelDirectory.clear();

	while (mCurrentMenu) {
		auto top = mCurrentMenu;
		mCurrentMenu = top->mParent;
		delete top;
	}
	mCurrentMenu = nullptr;
	mCurrentPostProcessing = nullptr;
	mCurrentMenuPost = nullptr;

	mCommonLibraries.menus.clear();
	mCommonLibraries.standard.geometries.clear();
	mCommonLibraries.standard.sounds.clear();
	mCommonLibraries.standard.programs.clear();
	mCommonLibraries.standard.shaders.clear();
	mCommonLibraries.standard.textures.clear();
	mCommonLibraries.post.filters.clear();
	mCommonLibraries.post.kernels.clear();
	mCommonLibraries.post.pipelines.clear();
	mCommonLibraries.post.shaders.clear();
}

Game::~Game() {
	cleanup();
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
	if (mCurrentLevel) {
		// A compelling reason to separate out PhysicsComponents, and possibly Bounds
		for (unsigned int i = 0; i < mWorkingObjectList.size(); i++) {
			auto I = mWorkingObjectList[i];
			if (I->getBounds()) {
				for (unsigned int j = i + 1; j < mWorkingObjectList.size(); j++) {
					auto J = mWorkingObjectList[j];
					if (J->getBounds()) {
						if (collides(I->getBounds(), J->getBounds())) {
							CollisionData collision;
							collision.first = I->getPhysicsComponent();
							collision.second = J->getPhysicsComponent();
							I->handle(Event(collision));
							J->handle(Event(collision));
						}
					}
				}
			}
		}
		// If there's a lag spike, all inputs from any frames will be processed on the first frame... Not sure yet if that's a problem (it probably is)
		//KeyboardHandler::getInstance().step();
		KeyboardHandler::getInstance().dispatchAll();
		//MouseHandler::getInstance().step();
		MouseHandler::getInstance().dispatchAll();
		mWorkingActiveCamera->update(dt);	// Hmm
		if (!mPaused) {
			// Object updates
			for (auto object : mWorkingObjectList) {
				object->update(dt);	// A compelling reason to separate further into Components which can be updated individually
			}
		}
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

	float debug_true[]{ 0.0f,1.0f,0.0f,0.5f }, debug_false[]{ 0.0f,0.0f,1.0f,0.5f };

	// Set the active framebuffer to the appropriate target:
	if (mCurrentPostProcessing) mCurrentPostProcessing->setAsDrawTarget();	// If we're doing any scene PP, draw on that
	else if (mCurrentMenu) mCurrentMenuPost->setAsDrawTarget();				// If not, and we're doing any menu PP, draw on that
	else mWindow->enableDrawing();											// If not, draw directly on the window backbuffer

	if (mWorkingActiveCamera) {
		// Draw each object in the scene
		for (auto object : mWorkingObjectList)
			object->render(mWorkingActiveCamera->getCameraComponent());
		// Draw any debug information
		if (mDebugMode != DEBUG_NONE) {
			// Draw object axes
			auto program = mCommonLibraries.standard.programs.get("debug_axes");
			program->use();
			program->sendUniform("uVP", glm::value_ptr(mWorkingActiveCamera->getCameraComponent()->getViewProjectionMatrix()));
			program->sendUniform("uM", glm::value_ptr(glm::mat4(1.0f)));
			Geometry::drawAxes();
			for (auto object : mWorkingObjectList) {
				program->sendUniform("uM", glm::value_ptr(object->getPhysicsComponent()->getModelMatrix()));
				Geometry::drawAxes();
			}
			// Draw a debug value with solid colors
			program = mCommonLibraries.standard.programs.get("debug_bbs");
			program->use();
			program->sendUniform("uVP", glm::value_ptr(mWorkingActiveCamera->getCameraComponent()->getViewProjectionMatrix()));
			switch (mDebugMode) {
			case DEBUG_COLLISION:
				for (auto object : mWorkingObjectList) {
					if (object->hasCollision())
						program->sendUniform("uDebugColor", 4, 1, debug_true);
					else
						program->sendUniform("uDebugColor", 4, 1, debug_false);
					program->sendUniform("uM", glm::value_ptr(glm::mat4(1.0f)));	// AABB's and spheres, at least, will be tracking their world coordinates
					object->debugDraw();
				}
				break;
			case DEBUG_MOUSE:
				for (auto object : mWorkingObjectList) {
					if (object->hasMouseOver())
						program->sendUniform("uDebugColor", 4, 1, debug_true);
					else
						program->sendUniform("uDebugColor", 4, 1, debug_false);
					program->sendUniform("uM", glm::value_ptr(glm::mat4(1.0f)));
					object->debugDraw();
				}
				break;
			}
		}
	}

	if (mCurrentPostProcessing) {
		mCurrentPostProcessing->process();
		mCommonLibraries.post.filters.get("none")->use();
		mCurrentPostProcessing->setAsTextureSource();
		if (mCurrentMenu) mCurrentMenuPost->setAsDrawTarget();	// If we're doing any menu PP, draw on that
		else mWindow->enableDrawing();							// If not, draw on the window
		Geometry::getScreenQuad()->render();
	}

	if (mHUD) mHUD->draw();	// If there was any PP, this will be drawn onto the same target that the PP was; if not, it will be drawn onto the same target as the scene

	if (mCurrentMenu) {
		mCurrentMenuPost->process();
		mCommonLibraries.post.filters.get("none")->use();
		mCurrentMenuPost->setAsTextureSource();
		mWindow->enableDrawing();	// There are no more layers, any further drawing is directly on the window
		Geometry::getScreenQuad()->render();
		mCurrentMenu->draw();
	}

	// Mouse picking stuff
	MouseHandler::getInstance().setAsDrawingTarget();
	//mWindow->enableDrawing();
	Program* prog = mCommonLibraries.standard.programs.get("mouse_selection");
	prog->use();
	if (mWorkingActiveCamera) {
		prog->sendUniform("uVP", glm::value_ptr(mWorkingActiveCamera->getCameraComponent()->getViewProjectionMatrix()));
		prog->sendUniform("uCamera", 3, 1, glm::value_ptr(mWorkingActiveCamera->getCameraComponent()->getPosition()));
		for (auto object : mWorkingObjectList) {
			prog->sendUniform("uObjectID", object->getIndex());
			object->render(prog);
		}
	}
	/*prog = mCommonLibraries.post.filters.get("none");
	prog->use();
	MouseHandler::getInstance().setAsTextureSource();
	mWindow->enableDrawing();
	Geometry::getScreenQuad()->render();*/
}

void Game::resize(unsigned int width, unsigned int height) {
	// Presumably OpenGL won't let me resize it to something impossible, so there's no need to check the dimensions
	mWindow->resize(width, height);
	MouseHandler::getInstance().resize(width, height);
	if (mCurrentLevel)
		for (auto a : mCurrentLevel->getCameraList())
			a.second->getCameraComponent()->resize(width, height);
	if (mWorkingActiveCamera)
		mWorkingActiveCamera->getCameraComponent()->resize(width, height);
	for (auto processor : mCommonLibraries.post.pipelines)
		processor.second->resize(width, height);
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
		case 'R': {
			mPaused = true;
			//auto loadingthread = std::thread(&Game::reloadAll, this);	// Well that doesn't work for shit
			reloadAll();
			mPaused = false;
			break;
		}
		case 'r':
			softReload();
			break;
		case 'b':
			switch (mDebugMode) {
			case DEBUG_NONE:
				mDebugMode = DEBUG_COLLISION;
				ServiceLocator::getLoggingService().log("======== DEBUG: COLLISION ========");
				break;
			case DEBUG_COLLISION:
				mDebugMode = DEBUG_MOUSE;
				ServiceLocator::getLoggingService().log("======== DEBUG: MOUSE ========");
				break;
			case DEBUG_MOUSE:
				mDebugMode = DEBUG_NONE;
				ServiceLocator::getLoggingService().log("======== DEBUG: NONE ========");
				break;
			}
			break;
		case 'p':
			if (mCommonLibraries.post.pipelines.contains("none") && mCurrentPostProcessing == mCommonLibraries.post.pipelines.get("none")) {
				ServiceLocator::getLoggingService().log("======== POSTPROCESSING: Sobel ========");
				mCurrentPostProcessing = mCommonLibraries.post.pipelines.get("Sobel");
			}
			else if (mCommonLibraries.post.pipelines.contains("Sobel") && mCurrentPostProcessing==mCommonLibraries.post.pipelines.get("Sobel")) {
				ServiceLocator::getLoggingService().log("======== POSTPROCESSING: bloom ========");
				mCurrentPostProcessing = mCommonLibraries.post.pipelines.get("bloom");
			}
			else if (mCommonLibraries.post.pipelines.contains("bloom") && mCurrentPostProcessing == mCommonLibraries.post.pipelines.get("bloom")) {
				ServiceLocator::getLoggingService().log("======== POSTPROCESSING: none ========");
				mCurrentPostProcessing = mCommonLibraries.post.pipelines.get("none");
			}
			break;
		case '+':
			mDebugStageSelection++;
			mDebugStageSelection %= 3;
			break;
		case 27:
			// Pause / unpause
			// In a comment around here somewhere I have the code snippet to push a menu on the stack
			break;
		}
		break;
	}
}

Level* Game::loadLevel(std::string path) {
	StandardLibraries levelLibraries;
	FileService index(path + mIndexFilename);
	// Load any additional data into the new libraries
	// Level index files can add new geometry, shaders, programs, textures, and sounds.
	char* levelFile;
	index.extract("Level: \"`S\"`L", &levelFile);
	char* err, *workingDirectory;
	while (index.good()) {
		try {
			if (index.extract("//`S`L", NULL));
			/*else if (index.extract("menus: \"`S\"`L", &workingDirectory)) {
				parseMenuIndex(path + workingDirectory, mCommonLibraries.menus);
				delete[] workingDirectory;
			}*/
			else if (index.extract("obj: \"`S\"`L", &workingDirectory)) {
				parseGeometryIndex(path + workingDirectory, levelLibraries.geometries);
				delete[] workingDirectory;
			}
			else if (index.extract("sound: \"`S\"`L", &workingDirectory)) {
				parseSoundIndex(path + workingDirectory, levelLibraries.sounds);
				delete[] workingDirectory;
			}
			else if (index.extract("tex: \"`S\"`L", &workingDirectory)) {
				parseTextureIndex(path + workingDirectory, levelLibraries.textures);
				delete[] workingDirectory;
			}
			else if (index.extract("glsl: \"`S\"`L", &workingDirectory)) {
				parseShaderIndex(path + workingDirectory, levelLibraries.shaders, levelLibraries.programs);
				delete[] workingDirectory;
			}
			/*else if (index.extract("post: \"`S\"`L", &workingDirectory)) {
				parsePostprocessingIndex(path + workingDirectory, postShaderLibrary, postFilterLibrary, postKernelLibrary, postPipelineLibrary);
				delete[] workingDirectory;
			}*/
			else if (index.extract("`S`L", &err)) {
				ServiceLocator::getLoggingService().error("Unexpected line in level index file", err);
				delete[] err;
			}
		}
		catch (std::exception e) {
			ServiceLocator::getLoggingService().badFileError(e.what());
		}
	}
	return new Level(path + levelFile, mCommonLibraries.standard, levelLibraries);
}

void Game::parseMenuIndex(std::string path, NamedContainer<RootElement*> &menuLibrary) {
	FileService menuIndex(path + mIndexFilename);
	if (!menuIndex.good()) throw std::exception(menuIndex.getPath().data());
	struct { char*name, *path; } menuData;
	while (menuIndex.good()) {
		if (menuIndex.extract("//`S`L", NULL));
		else if (menuIndex.extract("\"`S\":\"`S\"`L", &menuData)) {
			try {
				menuLibrary.add(menuData.name, new RootElement(path + menuData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + menuData.path);
			}
			delete[] menuData.name;
			delete[] menuData.path;
		}
		else if (menuIndex.extract("`S`L", &menuData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in menu index file", menuData.name);
			delete[] menuData.name;
		}
	}
}

void Game::parseGeometryIndex(std::string path, NamedContainer<Geometry*> &geomLibrary) {
	FileService objIndex(path + mIndexFilename);
	if (!objIndex.good()) throw std::exception(objIndex.getPath().data());
	struct { char*name, *path; } objData;
	while (objIndex.good()) {
		if (objIndex.extract("//`S`L", NULL));
		else if (objIndex.extract("\"`S\":\"`S\"`L", &objData)) {
			try {
				geomLibrary.add(objData.name, new Geometry(path + objData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + objData.path);
			}
			delete[] objData.name;
			delete[] objData.path;
		}
		else if (objIndex.extract("`S`L", &objData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in obj index file", objData.name);
			delete[] objData.name;
		}
	}
}

void Game::parseSoundIndex(std::string path, SoundLibrary &soundLibrary) {
	FileService soundIndex(path + mIndexFilename);
	if (!soundIndex.good()) throw std::exception(soundIndex.getPath().data());
	struct { char*name, *path; } soundData;
	while (soundIndex.good()) {
		if (soundIndex.extract("//`S`L", NULL));
		else if (soundIndex.extract("Procedural \"`S\" \"`S\"`L", &soundData)) {
			try {
				soundLibrary.add(soundData.name, new ProceduralSound(path + soundData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + soundData.path);
			}
			delete[] soundData.name;
			delete[] soundData.path;
		}
		else if (soundIndex.extract("File \"`S\" \"`S\"`L", &soundData)) {
			try {
				soundLibrary.add(soundData.name, new FileSound(path + soundData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + soundData.path);
			}
			delete[] soundData.name;
			delete[] soundData.path;
		}
		else if (soundIndex.extract("`S`L", &soundData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in sound index file", soundData.name);
			delete[] soundData.name;
		}
	}
}

void Game::parseTextureIndex(std::string path, NamedContainer<Texture*> &texLibrary) {
	FileService texIndex(path + mIndexFilename);
	if (!texIndex.good()) throw std::exception(texIndex.getPath().data());
	struct { char*name, *path; } texData;
	while (texIndex.good()) {
		if (texIndex.extract("//`S`L", NULL));
		else if (texIndex.extract("\"`S\":\"`S\"`L", &texData)) {
			try {
				texLibrary.add(texData.name, new Texture(path + texData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + texData.path);
			}
			delete[] texData.name;
			delete[] texData.path;
		}
		else if (texIndex.extract("`S`L", &texData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in texture index file", texData.name);
			delete[] texData.name;
		}
	}
}

#include "GL\glew.h"	// I literally just need this for GL_VERTEX_SHADER, which is a bit annoying

void Game::parseShaderIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &progLibrary) {
	FileService glslIndex(path + mIndexFilename);
	if (!glslIndex.good()) throw FileException(glslIndex.getPath().data());
	struct { char* name; int version; char* path, *extra = NULL; } glslData;
	struct { char* name = NULL, *vert = NULL, *tesc = NULL, *tese = NULL, *geom = NULL, *frag = NULL, *comp = NULL, *extra = NULL; } progData;
	int type;
	while (glslIndex.good()) {
		if (glslIndex.extract("//`S`L", NULL)) type = NULL;
		else if (glslIndex.extract("Vertex ", NULL)) type = GL_VERTEX_SHADER;
		else if (glslIndex.extract("Tessellation Control ", NULL)) type = GL_TESS_CONTROL_SHADER;
		else if (glslIndex.extract("Tessellation Evaluation ", NULL)) type = GL_TESS_EVALUATION_SHADER;
		else if (glslIndex.extract("Geometry ", NULL)) type = GL_GEOMETRY_SHADER;
		else if (glslIndex.extract("Fragment ", NULL)) type = GL_FRAGMENT_SHADER;
		else if (glslIndex.extract("Compute ", NULL)) type = GL_COMPUTE_SHADER;
		else if (glslIndex.extract("Program \"`S\"", &progData.name)) {
			type = NULL;
			std::vector<char*> vert, tesc, tese, geom, frag, comp;
			while (!glslIndex.extract("`L", NULL)) {
				if (glslIndex.extract(" Vertex:\"`S\"", &progData.vert)) { vert.push_back(progData.vert); }
				else if (glslIndex.extract(" Geometry:\"`S\"", &progData.geom)) { geom.push_back(progData.geom); }
				else if (glslIndex.extract(" Tessellation Control:\"`S\"", &progData.tesc)) { tesc.push_back(progData.tesc); }
				else if (glslIndex.extract(" Tessellation Evaluation:\"`S\"", &progData.tese)) { tese.push_back(progData.tese); }
				else if (glslIndex.extract(" Fragment:\"`S\"", &progData.frag)) { frag.push_back(progData.frag); }
				else if (glslIndex.extract(" Compute:\"`S\"", &progData.comp)) { comp.push_back(progData.comp); }
				else if (glslIndex.extract(" `S:\"`S\"", &progData.comp)) {
					ServiceLocator::getLoggingService().error("Unknown shader type", progData.comp);
					ServiceLocator::getLoggingService().error(" ... with name", progData.extra);
					delete[] progData.comp;
					delete[] progData.extra;
					progData.comp = NULL;
					progData.extra = NULL;
				}
				else if (glslIndex.extract("`S`L", &progData.extra)) {
					ServiceLocator::getLoggingService().error("Unexpected data in program definition", progData.extra);
					delete[] progData.extra;
					progData.extra = NULL;
					break;
				}
			}
			Program* prog = new Program();
			for (auto i : vert) {
				try { prog->attach(shaderLibrary.get(i), GL_VERTEX_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Vertex shader named but not present", i); }
			}
			for (auto i : tesc) {
				try { prog->attach(shaderLibrary.get(i), GL_TESS_CONTROL_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Tessellation Control shader named but not present", i); }
			}
			for (auto i : tese) {
				try { prog->attach(shaderLibrary.get(i), GL_TESS_EVALUATION_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Tessellation Evaluation shader named but not present", i); }
			}
			for (auto i : geom) {
				try { prog->attach(shaderLibrary.get(i), GL_GEOMETRY_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Geometry shader named but not present", i); }
			}
			for (auto i : frag) {
				try { prog->attach(shaderLibrary.get(i), GL_FRAGMENT_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Fragment shader named but not present", i); }
			}
			for (auto i : comp) {
				try { prog->attach(shaderLibrary.get(i), GL_COMPUTE_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Compute shader named but not present", i); }
			}
			if (prog->link() && prog->validate()) {
				progLibrary.add(progData.name, prog);
				prog->detachAll();
			}
			else {
				prog->removeAll(GL_COMPUTE_SHADER);
				if (prog->link(false) && prog->validate(false)) {
					prog->detachAll();
					ServiceLocator::getLoggingService().log(std::string("Recovered by removing compute shaders. ") + progData.name + " may not function as expected.");
					progLibrary.add(progData.name, prog);
				}
				else {
					prog->removeAll(GL_TESS_CONTROL_SHADER);
					prog->removeAll(GL_TESS_EVALUATION_SHADER);
					if (prog->link(false) && prog->validate(false)) {
						prog->detachAll();
						ServiceLocator::getLoggingService().log(std::string("Recovered by removing tesselation shaders. ") + progData.name + " may not function as expected.");
						progLibrary.add(progData.name, prog);
					}
					else {
						prog->removeAll(GL_GEOMETRY_SHADER);
						if (prog->link(false) && prog->validate(false)) {
							prog->detachAll();
							ServiceLocator::getLoggingService().log(std::string("Recovered by removing geometry shaders. ") + progData.name + " may not function as expected.");
							progLibrary.add(progData.name, prog);
						}
						else {
							ServiceLocator::getLoggingService().log(std::string("Could not recover ") + progData.name + ".");
							delete prog;
						}
					}
				}
			}
			// Cleanup
			for (auto i : vert) delete[] i;
			for (auto i : tesc) delete[] i;
			for (auto i : tese) delete[] i;
			for (auto i : geom) delete[] i;
			for (auto i : frag) delete[] i;
			for (auto i : comp) delete[] i;
			delete[] progData.name;
		}
		else if (glslIndex.extract("`S`L", &glslData.name)) {
			type = NULL;
			ServiceLocator::getLoggingService().error("Unexpected line in glsl index file", glslData.name);
			delete[] glslData.name;
		}
		if (type && glslIndex.extract("\"`S\" `I:\"`S\"`?S`L", &glslData)) {
			try {
				Shader* temp = new Shader(path + glslData.path, type);
				try {
					shaderLibrary.get(glslData.name);
					ServiceLocator::getLoggingService().error("Attempted redefintion of shader", glslData.name);
				}
				catch (std::out_of_range e) {
					shaderLibrary.add(glslData.name, glslData.version, temp);
				}
				if (glslData.extra) ServiceLocator::getLoggingService().error("Extra data at end of shader definition", glslData.extra);
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + glslData.path);
			}
			delete[] glslData.name;
			delete[] glslData.path;
			if (glslData.extra) {
				delete[] glslData.extra;
				glslData.extra = NULL;
			}
		}
	}
}

void Game::parsePostprocessingIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &filterLibrary, KernelManager &kernelLibrary, NamedContainer<PostprocessingPipeline*> &pipelineLibrary) {
	FileService postIndex(path + mIndexFilename);
	if (!postIndex.good()) throw std::exception(postIndex.getPath().data());
	struct { char* name; int samples; char* path, *extra = nullptr; } shaderData;
	struct { char* name; Kernel kernel; } kernelData;
	struct { char* name, *sampler, *processor; int in = 0, out = 0; } filterData;
	struct { char* name; } pipelineData;
	while (postIndex.good()) {
		if (postIndex.extract("//`S`L")) {}
		else if (postIndex.extract("Sampler \"`S\" `I \"`S\"", &shaderData)) {
			postIndex.extract("`?S`L", &shaderData.extra);
			if (shaderData.extra) {
				ServiceLocator::getLoggingService().error("Extra data at end of shader definition", shaderData.extra);
				delete[] shaderData.extra;
				shaderData.extra = nullptr;
			}
			shaderLibrary.add(shaderData.name, shaderData.samples, new Shader(path + shaderData.path, GL_VERTEX_SHADER));
			delete[] shaderData.name;
			delete[] shaderData.path;
		}
		else if (postIndex.extract("Processor \"`S\" `I \"`S\"", &shaderData)) {
			postIndex.extract("`?S`L", &shaderData.extra);
			if (shaderData.extra) {
				ServiceLocator::getLoggingService().error("Extra data at end of shader definition", shaderData.extra);
				delete[] shaderData.extra;
				shaderData.extra = nullptr;
			}
			shaderLibrary.add(shaderData.name, shaderData.samples, new Shader(path + shaderData.path, GL_FRAGMENT_SHADER));
			delete[] shaderData.name;
			delete[] shaderData.path;
		}
		else if (postIndex.extract("Kernel \"`S\" `I [", &kernelData)) {
			kernelData.kernel.weights = new float[kernelData.kernel.samples];
			bool valid = true;
			for (int i = 0; i < kernelData.kernel.samples; i++) {
				if (!postIndex.extract("`F", kernelData.kernel.weights + i)) {
					valid = false;
					break;
				}
				else if (postIndex.extract(", ") || postIndex.extract("]"));
				else {
					valid = false;
					break;
				}
			}
			if (valid && postIndex.extract("`L")) {
				kernelLibrary.add(kernelData.name, kernelData.kernel);
				delete[] kernelData.name;
				// Can't delete value array because kernel owns it now
				continue;
			}
			else {
				char* err;
				postIndex.extract("`S`L", &err);
				ServiceLocator::getLoggingService().error("Unexpected line in kernel definition", err);
				delete[] err;
				delete[] kernelData.name;
				delete[] kernelData.kernel.weights;
			}
		}
		else if (postIndex.extract("Filter \"`S\" Sampler:\"`S\" Processor:\"`S\" in:`I out:`I`L", &filterData)) {
			// Do error checking, then construct the filter program
			Shader* sampler = nullptr, *processor = nullptr;
			bool valid = true;
			try { sampler = shaderLibrary.get(filterData.sampler); }
			catch (std::exception e) { ServiceLocator::getLoggingService().error("Sampler not found", e.what()); valid = false; }
			try { processor = shaderLibrary.get(filterData.processor); }
			catch (std::exception e) { ServiceLocator::getLoggingService().error("Processor not found", e.what()); valid = false; }
			if (filterData.in < 0) { ServiceLocator::getLoggingService().error("Invalid number of input samples", std::to_string(filterData.in)); valid = false; };
			if (filterData.out == 0) { ServiceLocator::getLoggingService().error("Invalid number of output samples", std::to_string(filterData.out)); valid = false; };
			if (valid) {
				Program* filter = new Program(sampler, processor, filterData.in, filterData.out);
				// There should be a smarter way to set these, maybe by listing them all out in the index file
				filter->sendUniform("uColorBuffer", 0);
				filter->sendUniform("uDepthBuffer", 1);
				for (int i = 0; i < 10; i++) {
					std::string name("uTexture");
					name += std::to_string(i);
					filter->sendUniform(name.c_str(), i);
				}
				filter->sendUniform("uTexture", 0);
				filterLibrary.add(filterData.name, filter);
			}
		}
		else if (postIndex.extract("Pipeline \"`S\" [", &pipelineData)) {
			PostprocessingPipeline* temp = new PostprocessingPipeline(mWindow->getWidth(), mWindow->getHeight());
			bool valid = true;
			while (true) {
				// Read
				struct { char* name; float scale = 1.0f; char* kernel = nullptr; } stageData;
				bool stageValid = true, compositingInput = false;
				std::vector<char*> kernels;
				if (postIndex.extract("\"`S\"", &stageData.name)) {
					compositingInput = postIndex.extract("*");
					if (postIndex.extract(":`F", &stageData.scale));
					if (postIndex.extract(":\"`S\"", &stageData.kernel));
					else if (postIndex.extract(":[")) {
						kernels.push_back(nullptr);
						while (!postIndex.extract("]")) {
							postIndex.extract("\"`S\"", &kernels.back());
							kernels.push_back(nullptr);
							postIndex.extract(",");
						}
						kernels.pop_back();
					}
					// Verify
					StageData stage;
					try {
						stage.filter = filterLibrary.get(stageData.name);
					}
					catch (std::invalid_argument e) {
						stageValid = false;
						ServiceLocator::getLoggingService().error("Not a valid filter", e.what());
					}
					stage.scale = stageData.scale;
					if (stageData.kernel) {
						try {
							stage.kernels.push_back(kernelLibrary.get(stageData.kernel));
						}
						catch (std::invalid_argument e) {
							stageValid = false;
							ServiceLocator::getLoggingService().error("Not a valid kernel", e.what());
						}
					}
					else if (kernels.size()) {
						for (auto kernel : kernels) {
							try {
								stage.kernels.push_back(kernelLibrary.get(kernel));
							}
							catch (std::invalid_argument e) {
								stageValid = false;
								ServiceLocator::getLoggingService().error("Not a valid kernel", e.what());
							}
						}
					}
					// Add
					if (stageValid) {
						try { temp->attach(stage, compositingInput); }
						catch (std::invalid_argument e) {
							ServiceLocator::getLoggingService().error(stageData.name, e.what());
							valid = false;
							delete temp;
						}
					}
					else valid = false;
					// Cleanup
					delete[] stageData.name;
					for (auto i : kernels)
						delete[] i;
					kernels.clear();
					if (stageData.kernel) {
						delete[] stageData.kernel;
						stageData.kernel = nullptr;
					}
				}
				else if (postIndex.extract("compositor:\"`S\"", &stageData.name)) {
					try {
						Program* compositor = filterLibrary.get(stageData.name);
						temp->attachCompositor(compositor);
					}
					catch (std::invalid_argument e) {
						ServiceLocator::getLoggingService().error("Not a valid compositing stage", e.what());
					}
					delete[] stageData.name;
				}
				if (postIndex.extract(", ")) continue;
				if (postIndex.extract("]")) break;
			}
			// Verify
			if (valid)
				pipelineLibrary.add(pipelineData.name, temp);
			else
				delete temp;
			// Cleanup
			char* err = nullptr;
			postIndex.extract("`?S`L", &err);
			if (err) {
				ServiceLocator::getLoggingService().error("Extra data at end of pipeline definition", err);
				delete[] err;
			}
			delete[] pipelineData.name;
		}
		else if (postIndex.extract("`?S`L", &pipelineData.name)) {
			if (pipelineData.name) {
				ServiceLocator::getLoggingService().error("Unexpected line in postprocessing index file", pipelineData.name);
				delete[] pipelineData.name;
				pipelineData.name = nullptr;
			}
		}
	}
}