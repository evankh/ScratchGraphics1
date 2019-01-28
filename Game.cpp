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

#include <thread>	// An experiment

#include "GL\glew.h"	// I literally just need this for GL_VERTEX_SHADER, which is a bit annoying
NamedContainer<State*> State::sBaseStateLibrary;	// This goes here for some reason

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
	// Load certain very important objects
	mCommonLibraries.standard.geometries.add("plane", Geometry::getNewQuad());
	// Load everything else from file
	FileService baseIndex(mAssetBasePath + mIndexFilename);
	if (!baseIndex.good()) throw "Asset file could not be opened.";
	struct { char *name, *path; } levelData;
	struct { int w, h; } resolution;
	char* commonFolder;
	while (baseIndex.good()) {
		if (baseIndex.extract("//`S`L", NULL));	// Comment
		else if (baseIndex.extract("Level \"`S\" \"`S\"`L", &levelData)) {
			mLevelDirectory.add(levelData.name, levelData.path);
			delete levelData.name;
			delete levelData.path;
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
					delete levelData.name;
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
						parseMenuIndex(commonPath + workingDirectory, mCommonLibraries.menus);
						delete workingDirectory;
					}
					else if (commonIndex.extract("obj: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load objs into common obj directory
						parseGeometryIndex(commonPath + workingDirectory, mCommonLibraries.standard.geometries);
						delete workingDirectory;
					}
					else if (commonIndex.extract("sound: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load sounds into common sound directory
						parseSoundIndex(commonPath + workingDirectory, mCommonLibraries.standard.sounds);
						delete workingDirectory;
					}
					else if (commonIndex.extract("tex: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load textures into common texture directory
						parseTextureIndex(commonPath + workingDirectory, mCommonLibraries.standard.textures);
						delete workingDirectory;
					}
					else if (commonIndex.extract("glsl: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load shaders into common shader directory
						parseShaderIndex(commonPath + workingDirectory, mCommonLibraries.standard.shaders, mCommonLibraries.standard.programs);
						delete workingDirectory;
					}
					else if (commonIndex.extract("post: \"`S\"`L", &workingDirectory)) {
						// Open the index file in the new path, then use it to load postprocessing things into common postprocessing directory
						parsePostprocessingIndex(commonPath + workingDirectory, mCommonLibraries.post.shaders, mCommonLibraries.post.filters, mCommonLibraries.post.kernels, mCommonLibraries.post.pipelines);
						delete workingDirectory;
					}
					else if (commonIndex.extract("`S`L", &workingDirectory)) {
						ServiceLocator::getLoggingService().error("Unexpected line in common index file", workingDirectory);
						delete workingDirectory;
					}
				}
				catch (std::exception e) {
					ServiceLocator::getLoggingService().badFileError(e.what());
				}
			}
		}
		else if (baseIndex.extract("`S`L", &levelData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in root index file", levelData.name);
			delete levelData.name;
		}
	}
}

void Game::cleanup() {
	delete mCurrentLevel;
	mCurrentLevel = NULL;
	mLevelDirectory.clear();

	while (mCurrentMenu) {
		auto top = mCurrentMenu;
		mCurrentMenu = top->mParent;
		delete top;
	}
	mCurrentMenu = NULL;
	mCurrentPostProcessing = NULL;
	mCurrentMenuPost = NULL;

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
		KeyboardHandler::getInstance().dispatchAll();
		MouseHandler::getInstance().dispatchAll();
		if (!mPaused) {
			// Object updates
			for (auto object : mCurrentLevel->getObjectList()) {
				object.second->update(dt);	// A compelling reason to separate further into Components which can be updated individually
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

	// Set the active framebuffer to the appropriate target
	if (mCurrentPostProcessing)
		mCurrentPostProcessing->enableDrawing();
	else if (mCurrentMenu) mCurrentMenuPost->enableDrawing();
	else mWindow->enableDrawing();
	if (mCurrentLevel) {
		for (auto object : mCurrentLevel->getObjectList())
			object.second->render(mCurrentLevel->getCurrentCamera());

		float debug_collision[]{ 0.0f,1.0f,0.0f,0.5f }, debug_nocollision[]{ 0.0f,0.0f,1.0f,0.5f };
		if (mDebugMode) {
			auto program = mCommonLibraries.standard.programs.get("debug_bbs");
			program->use();
			program->sendUniform("uVP", glm::value_ptr(mCurrentLevel->getCurrentCamera()->getViewProjectionMatrix()));
			for (auto object : mCurrentLevel->getObjectList()) {
				if (object.second->hasCollision())
					program->sendUniform("uDebugColor", 4, debug_collision);
				else
					program->sendUniform("uDebugColor", 4, debug_nocollision);
				program->sendUniform("uM", glm::value_ptr(object.second->getPhysicsComponent()->getModelMatrix()));
				object.second->debugDraw();
			}

			program = mCommonLibraries.standard.programs.get("debug_axes");
			program->use();
			program->sendUniform("uVP", glm::value_ptr(mCurrentLevel->getCurrentCamera()->getViewProjectionMatrix()));
			program->sendUniform("uM", glm::value_ptr(glm::mat4(1.0f)));
			Geometry::drawAxes();
			for (auto object : mCurrentLevel->getObjectList()) {
				program->sendUniform("uM", glm::value_ptr(object.second->getPhysicsComponent()->getModelMatrix()));
				Geometry::drawAxes();
			}
		}
	}
	// Need to set a null filter here
	if (mCurrentMenu) mCurrentMenuPost->enableDrawing();
	else mWindow->enableDrawing();
	if (mCurrentPostProcessing) {
		mCurrentPostProcessing->process();
		mCurrentPostProcessing->draw();
	}
	if (mHUD) mHUD->draw();
	//try { mCommonLibraries.post.filters.get("none")->use(); }
	//catch (std::exception e) { ServiceLocator::getLoggingService().error("No such postprocessor", e.what()); }
	if (mCurrentMenu) {
		mWindow->enableDrawing();
		mCurrentMenuPost->process();
		mCurrentMenuPost->draw();
		// Needs a default program to use
		mCurrentMenu->draw();
	}
}

void Game::resize(unsigned int width, unsigned int height) {
	// Presumably OpenGL won't let me resize it to something impossible, so there's no need to check the dimensions
	mWindow->resize(width, height);
	if (mCurrentLevel)
		for (auto a : mCurrentLevel->getCameraList())
			a.second->resize(width, height);
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
		case 'r': {
			mPaused = true;
			//auto loadingthread = std::thread(&Game::reloadAll, this);	// Well that doesn't work for shit
			reloadAll();
			mPaused = false;
			break;
		}
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
				delete workingDirectory;
			}*/
			else if (index.extract("obj: \"`S\"`L", &workingDirectory)) {
				parseGeometryIndex(path + workingDirectory, levelLibraries.geometries);
				delete workingDirectory;
			}
			else if (index.extract("sound: \"`S\"`L", &workingDirectory)) {
				parseSoundIndex(path + workingDirectory, levelLibraries.sounds);
				delete workingDirectory;
			}
			else if (index.extract("tex: \"`S\"`L", &workingDirectory)) {
				parseTextureIndex(path + workingDirectory, levelLibraries.textures);
				delete workingDirectory;
			}
			else if (index.extract("glsl: \"`S\"`L", &workingDirectory)) {
				parseShaderIndex(path + workingDirectory, levelLibraries.shaders, levelLibraries.programs);
				delete workingDirectory;
			}
			/*else if (index.extract("post: \"`S\"`L", &workingDirectory)) {
				parsePostprocessingIndex(path + workingDirectory, postShaderLibrary, postFilterLibrary, postKernelLibrary, postPipelineLibrary);
				delete workingDirectory;
			}*/
			else if (index.extract("`S`L", &err)) {
				ServiceLocator::getLoggingService().error("Unexpected line in level index file", err);
				delete err;
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
		}
		else if (menuIndex.extract("`S`L", &menuData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in menu index file", menuData.name);
			delete menuData.name;
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
		}
		else if (objIndex.extract("`S`L", &objData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in obj index file", objData.name);
			delete objData.name;
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
		}
		else if (soundIndex.extract("File \"`S\" \"`S\"`L", &soundData)) {
			try {
				soundLibrary.add(soundData.name, new FileSound(path + soundData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + soundData.path);
			}
		}
		else if (soundIndex.extract("`S`L", &soundData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in sound index file", soundData.name);
			delete soundData.name;
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
		}
		else if (texIndex.extract("`S`L", &texData.name)) {
			ServiceLocator::getLoggingService().error("Unexpected line in texture index file", texData.name);
			delete texData.name;
		}
	}
}

void Game::parseShaderIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &progLibrary) {
	FileService glslIndex(path + mIndexFilename);
	if (!glslIndex.good()) throw std::exception(glslIndex.getPath().data());
	struct { char* name; int version; char* path, *extra = NULL; } glslData;
	struct { char *name = NULL, *vert = NULL, *frag = NULL, *geom = NULL, *extra = NULL; } progData;
	int type;
	while (glslIndex.good()) {
		if (glslIndex.extract("//`S`L", NULL)) type = NULL;
		else if (glslIndex.extract("Vertex ", NULL)) type = GL_VERTEX_SHADER;
		else if (glslIndex.extract("Fragment ", NULL)) type = GL_FRAGMENT_SHADER;
		else if (glslIndex.extract("Geometry ", NULL)) type = GL_GEOMETRY_SHADER;
		else if (glslIndex.extract("Program \"`S\"", &progData.name)) {
			type = NULL;
			while (!glslIndex.extract("`L", NULL)) {
				if (glslIndex.extract(" Vertex:\"`S\"", &progData.vert));
				else if (glslIndex.extract(" Fragment:\"`S\"", &progData.frag));
				else if (glslIndex.extract(" Geometry:\"`S\"", &progData.geom));
				else if (glslIndex.extract("`S`L", progData.extra)) {
					ServiceLocator::getLoggingService().error("Unexpected data in program definition", progData.extra);
					delete progData.extra;
					progData.extra = NULL;
					break;
				}
			}
			Shader *vert = NULL, *frag = NULL, *geom = NULL;
			bool badvert = false, badfrag = false, badgeom = false;
			try { vert = shaderLibrary.get(progData.vert); }
			catch (std::exception) { badvert = true; }
			try { frag = shaderLibrary.get(progData.frag); }
			catch (std::exception) { badfrag = true; }
			if (progData.geom) try { geom = shaderLibrary.get(progData.geom); }
			catch (std::exception) { badgeom = true; }
			if (progData.vert && !badvert && progData.frag && !badfrag) {
				if (progData.geom && !badgeom) progLibrary.add(progData.name, new Program(vert, geom, frag));
				else progLibrary.add(progData.name, new Program(vert, frag));
			}
			else {
				// Report whatever errors
				if (progData.vert && badvert) ServiceLocator::getLoggingService().error("Vertex shader named but not present", progData.vert);
				if (progData.frag && badfrag) ServiceLocator::getLoggingService().error("Fragment shader named but not present", progData.frag);
				if (progData.geom && badgeom) ServiceLocator::getLoggingService().error("Geometry shader named but not present", progData.geom);
				if (!progData.vert) ServiceLocator::getLoggingService().error("Vertex shader not named", progData.name);
				if (!progData.frag) ServiceLocator::getLoggingService().error("Fragment shader not named", progData.frag);
			}
			// Cleanup
			if (progData.vert) { delete progData.vert; progData.vert = NULL; }
			if (progData.frag) { delete progData.frag; progData.frag = NULL; }
			if (progData.geom) { delete progData.geom; progData.geom = NULL; }
			delete progData.name;
		}
		else if (glslIndex.extract("`S`L", &glslData.name)) {
			type = NULL;
			ServiceLocator::getLoggingService().error("Unexpected line in glsl index file", glslData.name);
			delete glslData.name;
		}
		if (type && glslIndex.extract("\"`S\" `I:\"`S\"`?S`L", &glslData)) {
			try {
				shaderLibrary.add(glslData.name, glslData.version, new Shader(path + glslData.path, type));
				if (glslData.extra) ServiceLocator::getLoggingService().error("Extra data at end of shader definition", glslData.extra);
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + glslData.path);
			}
			delete glslData.name;
			delete glslData.path;
			if (glslData.extra) {
				delete glslData.extra;
				glslData.extra = NULL;
			}
		}
	}
}

void Game::parsePostprocessingIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &filterLibrary, KernelManager &kernelLibrary, NamedContainer<PostProcessingPipeline*> &pipelineLibrary) {
	FileService postIndex(path + mIndexFilename);
	if (!postIndex.good()) throw std::exception(postIndex.getPath().data());
	struct { char* name; int samples; char *path, *extra; } shaderData;
	struct { char* name; Kernel kernel; } kernelData;
	struct { char* name, *processor, *sampler, *kernel; } filterData;
	struct { char* name; } pipelineData;
	struct { char* name; float scale = 1.0f; char* kernel = NULL; } stageData;
	int type;
	while (postIndex.good()) {
		if (postIndex.extract("//`S`L", NULL)) type = NULL;
		else if (postIndex.extract("Sampler ", NULL)) type = GL_VERTEX_SHADER;
		else if (postIndex.extract("Processor ", NULL)) type = GL_FRAGMENT_SHADER;
		else if (postIndex.extract("Kernel \"`S\" `I [", &kernelData)) {
			type = NULL;
			kernelData.kernel.weights = new float[kernelData.kernel.samples];
			bool valid = true;
			for (int i = 0; i < kernelData.kernel.samples; i++) {
				if (!postIndex.extract("`F", kernelData.kernel.weights + i)) {
					valid = false;
					break;
				}
				else if (postIndex.extract(", ", NULL) || postIndex.extract("]", NULL));
				else {
					valid = false;
					break;
				}
			}
			if (valid && postIndex.extract("`L", NULL)) {
				kernelLibrary.add(kernelData.name, kernelData.kernel);
				delete kernelData.name;
				// Can't delete valuee array because kernel owns it now
				continue;
			}
			else {
				char* err;
				postIndex.extract("`S`L", &err);
				ServiceLocator::getLoggingService().error("Unexpected line in kernel definition", err);
				delete err;
				delete kernelData.name;
				delete[] kernelData.kernel.weights;
			}
		}
		else if (postIndex.extract("Filter \"`S\" Sampler:\"`S\" Processor:\"`S\"`L", &filterData)) {
			type = NULL;
			// Do error checking, then construct the filter program
			Shader* sampler = NULL, *processor = NULL;
			bool valid = true;
			try { sampler = shaderLibrary.get(filterData.sampler); }
			catch (std::exception e) { ServiceLocator::getLoggingService().error("Sampler not found", e.what()); valid = false; }
			try { processor = shaderLibrary.get(filterData.processor); }
			catch (std::exception e) { ServiceLocator::getLoggingService().error("Processor not found", e.what()); valid = false; }
			if (valid) filterLibrary.add(filterData.name, new Program(sampler, processor));
		}
		else if (postIndex.extract("Pipeline \"`S\" [", &pipelineData)) {
			type = NULL;
			// Read filter names or filter/kernel pairs from the list, check their existence, then add them to the pipeline
			// Needs to handle:
			//  - Just a name ("name")
			//  - A name and a scale ("name":1.0)
			//  - A name and a kernel ("name":"kernel")
			//  - A name, a scale and a kernel ("name":1.0:"kernel")
			PostProcessingPipeline* temp = new PostProcessingPipeline();
			temp->init(mWindow->getWidth(), mWindow->getHeight());
			bool valid = true;
			while (!postIndex.extract("]", NULL)) {
				if (postIndex.extract("\"`S\"", &stageData.name)) {
					if (postIndex.extract(":`F", &stageData.scale));
					if (postIndex.extract(":\"`S\"", &stageData.kernel));
				}
				else {
					char* err;
					postIndex.extract("`S`L", &err);
					ServiceLocator::getLoggingService().error("Malformed filter name", err);
					delete err;
					valid = false;
					break;
				}
				// Add stage to pipeline
				Program* filter = NULL;
				Kernel kernel{ 0, NULL };
				try {
					filter = filterLibrary.get(stageData.name);
				}
				catch (std::exception e) {
					ServiceLocator::getLoggingService().error("Filter not found", e.what());
					valid = false;
				}
				if (stageData.kernel) try {
					kernel = kernelLibrary.get(stageData.kernel);
				}
				catch (std::exception e) {
					ServiceLocator::getLoggingService().error("Kernel not found", e.what());
					valid = false;
				}
				if (valid) temp->attach(filter, kernel, stageData.scale);
				else break;
				if (postIndex.extract(", ", NULL)) continue;
				else if (postIndex.extract("]", NULL)) break;
				else {
					// Wrong separator
					valid = false;
					break;
				}
			}
			if (valid) {
				pipelineLibrary.add(pipelineData.name, temp);
				// No need to delete temp, the library owns it now
				char* err;
				postIndex.extract("`?S`L", &err);
				if (err) {
					ServiceLocator::getLoggingService().error("Extra data found at end of pipeline", err);
					delete err;
				}
			}
			else {
				delete temp;
			}
		}
		else if (postIndex.extract("`S`L", &pipelineData.name)) {
			type = NULL;
			ServiceLocator::getLoggingService().error("Unexpected line in postprocessing index file", pipelineData.name);
			delete pipelineData.name;
		}
		if (type && postIndex.extract("\"`S\" `I \"`S\"`?S`L", &shaderData)) {
			try {
				shaderLibrary.add(shaderData.name, shaderData.samples, new Shader(path + shaderData.path, type));
				if (shaderData.extra) ServiceLocator::getLoggingService().error("Extra data at end of shader definition", shaderData.extra);
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + shaderData.path);
			}
			delete shaderData.name;
			delete shaderData.path;
			if (shaderData.extra) {
				delete shaderData.extra;
				shaderData.extra = NULL;
			}
		}
	}
}