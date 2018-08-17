#include "Game.h"
#include "Program.h"
#include "KeyboardHandler.h"
#include "Shader.h"
#include "Window.h"
#include "ServiceLocator.h"

#include "cubedata_temp.h"
#include "squaredata_temp.h"

#include "GL\glew.h"
#include "glm\gtc\matrix_transform.hpp"

Game::Game() {
	// ?
	// I guess this would be the spot for the initialization of everything: Loading assets, starting services, initializing rendering, etc.
	mWindow = new Window(800, 600, "Game owns this window");
	KeyboardHandler::getInstance().registerReceiver('r', this);
	KeyboardHandler::getInstance().registerReceiver(27, this);
}

void Game::init() {
	// Set up the window and cameras
	mScreenCamera = new OrthoCamera(mWindow->getWidth(), mWindow->getHeight());
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 2.0f));
	view = glm::rotate(view, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	view = glm::rotate(view, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	mSceneCamera = new PerspCamera(view, mWindow->getWidth(), mWindow->getHeight(), 75.0f);

	FileService& index = ServiceLocator::getFileService(mAssetBasePath + mIndexFilename);
	if (index.good()) {
		// Generic game data
		struct { char* title; int w, h; } window;
		index.extract("\\S\n\\I \\I\n", &window);
		mWindow->resize(window.w, window.h);
		mWindow->rename(window.title);
		// Loading asset directories
		char* workingDirectory;
		while (index.good()) {
			if (index.extract("geometry:\"\\S\"\n", &workingDirectory)) {
				ServiceLocator::getLoggingService().error("Found geometry asset path", mAssetBasePath + workingDirectory);
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the geometry data
				}
				else {
					ServiceLocator::getLoggingService().error("Error opening geometry index", mAssetBasePath + workingDirectory + mIndexFilename);
				}
				delete workingDirectory;
			} else if (index.extract("shaders:\"\\S\"\n", &workingDirectory)) {
				ServiceLocator::getLoggingService().error("Found shader asset path", mAssetBasePath + workingDirectory);
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the shader data
				}
				else {
					ServiceLocator::getLoggingService().error("Error opening shader index", mAssetBasePath + workingDirectory + mIndexFilename);
				}
				delete workingDirectory;
			} else if(index.extract("post:\"\\S\"\n", &workingDirectory)) {
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
			} else if (index.extract("levels:\"\\S\"\n", &workingDirectory)) {
				ServiceLocator::getLoggingService().error("Found level asset path", mAssetBasePath + workingDirectory);
				FileService& workingIndex = ServiceLocator::getFileService(mAssetBasePath + workingDirectory + mIndexFilename);
				if (workingIndex.good()) {
					// Extract the geometry data
				}
				else {
					ServiceLocator::getLoggingService().error("Error opening levels index", mAssetBasePath + workingDirectory + mIndexFilename);
				}
				delete workingDirectory;
			} else if (index.extract("textures:\"\\S\"\n", &workingDirectory)) {
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

void Game::loadShaders() {
	mShaders.add("mvp_position",new Shader("glsl/mvp_position.vert",GL_VERTEX_SHADER));
	mShaders.add("mvp_white",	new Shader("glsl/mvp_white.vert",	GL_VERTEX_SHADER));
	mShaders.add("test_vert",	new Shader("glsl/test.vert",		GL_VERTEX_SHADER));
	mShaders.add("sample_1",	new Shader("glsl/post/single.vert",	GL_VERTEX_SHADER));
	mShaders.add("sample_7h",	new Shader("glsl/post/7x1.vert",	GL_VERTEX_SHADER));
	mShaders.add("sample_7v",	new Shader("glsl/post/1x7.vert",	GL_VERTEX_SHADER));
	mShaders.add("sample_11h",	new Shader("glsl/post/11x1.vert",	GL_VERTEX_SHADER));
	mShaders.add("sample_11v",	new Shader("glsl/post/1x11.vert",	GL_VERTEX_SHADER));
	mShaders.add("sample_3x3",	new Shader("glsl/post/3x3.vert",	GL_VERTEX_SHADER));
	mShaders.add("sample_3h",	new Shader("glsl/post/3x1.vert",	GL_VERTEX_SHADER));
	mShaders.add("sample_3v",	new Shader("glsl/post/1x3.vert",	GL_VERTEX_SHADER));

	mShaders.add("flat_faces",	new Shader("glsl/flat.geom",		GL_GEOMETRY_SHADER));

	mShaders.add("test_frag",	new Shader("glsl/test.frag",		GL_FRAGMENT_SHADER));
	mShaders.add("diffuse",		new Shader("glsl/diffuse.frag",		GL_FRAGMENT_SHADER));
	mShaders.add("none",		new Shader("glsl/post/none.frag",	GL_FRAGMENT_SHADER));
	mShaders.add("invert",		new Shader("glsl/post/invert.frag",	GL_FRAGMENT_SHADER));
	mShaders.add("darken",		new Shader("glsl/post/darken.frag",	GL_FRAGMENT_SHADER));
	mShaders.add("blur_7",		new Shader("glsl/post/blur7.frag",	GL_FRAGMENT_SHADER));
	mShaders.add("blur_11",		new Shader("glsl/post/blur11.frag",	GL_FRAGMENT_SHADER));
	mShaders.add("kernel_3x3",	new Shader("glsl/post/kernel3x3.frag",	GL_FRAGMENT_SHADER));
	mShaders.add("kernel_3",	new Shader("glsl/post/kernel3.frag",GL_FRAGMENT_SHADER));
	mShaders.add("min",			new Shader("glsl/post/min.frag",	GL_FRAGMENT_SHADER));
	mShaders.add("max",			new Shader("glsl/post/max.frag",	GL_FRAGMENT_SHADER));

	mPrograms.add("debug_mvp_position_color",new Program(mShaders.get("mvp_position"),	mShaders.get("test_frag")));
	mPrograms.add("debug_mvp_white",		new Program(mShaders.get("mvp_white"),		mShaders.get("test_frag")));
	mPrograms.add("debug_notransform_white",new Program(mShaders.get("test_vert"),		mShaders.get("test_frag")));
	mPrograms.add("debug_flat",				new Program(mShaders.get("mvp_position"),	mShaders.get("flat_faces"),		mShaders.get("diffuse")));
	mPrograms.add("post_none",				new Program(mShaders.get("sample_1"),		mShaders.get("none")));
	mPrograms.add("post_invert",			new Program(mShaders.get("sample_1"),		mShaders.get("invert")));
	mPrograms.add("post_darken",			new Program(mShaders.get("sample_1"),		mShaders.get("darken")));
	mPrograms.add("post_hblur",				new Program(mShaders.get("sample_11h"),		mShaders.get("blur_11")));
	mPrograms.add("post_vblur",				new Program(mShaders.get("sample_11v"),		mShaders.get("blur_11")));
	mPrograms.add("post_convolution",		new Program(mShaders.get("sample_3x3"),		mShaders.get("kernel_3x3")));
	mPrograms.add("post_conv_3h",			new Program(mShaders.get("sample_3h"),		mShaders.get("kernel_3")));
	mPrograms.add("post_conv_3v",			new Program(mShaders.get("sample_3v"),		mShaders.get("kernel_3")));
	mPrograms.add("post_min",				new Program(mShaders.get("sample_3x3"),		mShaders.get("min")));
	mPrograms.add("post_max",				new Program(mShaders.get("sample_3x3"),		mShaders.get("max")));

	mShaders.clear_delete();
}

void Game::loadGeometry() {
	mGeometries.add("debug_cube", new Geometry(cube_numverts, (float*)cube_vertices, cube_numfaces, cube_faces, { A_POSITION }));
	mGeometries.add("debug_menu_square", new Geometry(square_numverts, (float*)square_vertices, square_numfaces, square_faces, { A_POSITION }));

	// First we would need to load a file containing main menu information, then game saves, then data about the levels, then depending on the save selected, load a particular level...
	// For now, automatically load one level, the debug level
//	loadLevel();
	bool memoryleak[5] = { true, true, false, true, false };
	mGameObjects.push_back(new GameObject(mGeometries.get("debug_cube"), mPrograms.get("debug_flat"), new PhysicsComponent(glm::vec3(0.0f, 0.0f, 1.0f), 90.0f), new MouseInputComponent(memoryleak)));
	mGameObjects.push_back(new GameObject(mGeometries.get("debug_cube"), mPrograms.get("debug_flat"), new PhysicsComponent(glm::vec3(0.0f, 0.0f, 1.0f), 45.0f), new KeyboardInputComponent(4,"wasd")));
	mGameObjects.back()->translate(glm::vec3(0.0f, 0.0f, 1.0f));
	mGameObjects.back()->scale(glm::vec3(0.5f));
}

void Game::loadMenu() {
	/*mHUDItems.push_back(new GameObject(mGeometries.get("debug_menu_square"), mPrograms.get("debug_mvp_white"), new PhysicsComponent(), NULL));
	mHUDItems.back()->translate(glm::vec3(50.0f, 50.0f, 0.0f));
	mHUDItems.back()->scale(glm::vec3(700.0f, 25.0f, 0.0f));//*/
}

void Game::loadPostProcessing() {
	float* debug_square_blur = new float[16] { 0.0625, 0.125, 0.0625, 0.125, 0.25, 0.125, 0.0625, 0.125, 0.0625 };	// Fuck it, it can be a memory leak for now
	float* debug_both_edges = new float[16]{ -2,-1,0,-1,0,1,0,1,2 };
	float* debug_edge = new float[3]{ -1.0f, 0.0f, 1.0f };
	mGameObjectsPost.init(mWindow->getWidth(), mWindow->getHeight());
	//mGameObjectsPost.attach(mPrograms.get("post_conv_3h"), 3, debug_edge, 0.5f);
	//mGameObjectsPost.attach(mPrograms.get("post_conv_3v"), 3, debug_edge, 0.5f);
	//mGameObjectsPost.attach(mPrograms.get("post_convolution"), 9, debug_both_edges, 0.5f);
	/*mGameObjectsPost.attach(mPrograms.get("post_hblur"), 0.5f);
	mGameObjectsPost.attach(mPrograms.get("post_vblur"), 0.5f);
	mGameObjectsPost.attach(mPrograms.get("post_hblur"), 0.2f);
	mGameObjectsPost.attach(mPrograms.get("post_vblur"), 0.2f);//*/
	//mGameObjectsPost.attach(mPrograms.get("post_invert"), 2.0f);
	mMenuPost.init(mWindow->getWidth(), mWindow->getHeight());
	mMenuPost.attach(mPrograms.get("post_darken"));
	mMenuPost.attach(mPrograms.get("post_hblur"));
	mMenuPost.attach(mPrograms.get("post_vblur"));
}

void Game::cleanup() {
	mGameObjectsPost.clear();
	mMenuPost.clear();
	for (auto object : mGameObjects)
		delete object;
	mGameObjects.clear();
	for (auto item : mHUDItems)
		delete item;
	mHUDItems.clear();
	mGeometries.clear_delete();
	mPrograms.clear_delete();
	mShaders.clear_delete();
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
	for (auto object : mGameObjects) {
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
	for (auto object : mGameObjects) {
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
	init();
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
