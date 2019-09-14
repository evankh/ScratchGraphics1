#include "Game.h"
#include "Camera.h"
#include "GameObject.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "ServiceLocator.h"
#include "Shader.h"
#include "Source.h"
#include "State.h"
#include "Window.h"

NamedContainer<State*> State::sBaseStateLibrary;	// This goes here for some reason

Game::Game() {
	// ?
	// I guess this would be the spot for the initialization of everything: Loading assets, starting services, initializing rendering, etc.
//	mWindow = new Window(800, 600, "Game owns this window");
	KeyboardHandler::getInstance().registerReceiver("rRbp+m", this);
	KeyboardHandler::getInstance().registerReceiver(27, this);
}

void Game::softReload() {
	ServiceLocator::getLoggingService().log("===== LEVEL RESET =====");
	// Empty everything out: already done by cleanup
	mWorkingListSize = mCurrentLevel->getNumObjects();
	mCurrentLevel->getWorkingSet(&mWorkingObjectList, &mWorkingPCList);
	mWorkingActiveCamera = mWorkingObjectList + mCurrentLevel->getMainCameraIndex();
	mSoundSystem.registerListener(mWorkingActiveCamera->getPhysicsComponent());
	mGlobalAudio = new Source(mWorkingActiveCamera->getPhysicsComponent(), true);
	mGlobalAudio->update();
	mGlobalAudio->playSound(mCurrentLevel->getBackgroundMusic());
}

void Game::cleanup() {
	delete mGlobalAudio;

	delete mCurrentLevel;
	mCurrentLevel = nullptr;
	mLevelDirectory.clear();

	delete[] mWorkingPCList;
	mWorkingPCList = nullptr;
	delete[] mWorkingObjectList;
	mWorkingObjectList = nullptr;
	mWorkingListSize = 0;
	mWorkingActiveCamera = nullptr;

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
	// Collision detection
	// Yo, I just really want to do this in the least efficient way possible
	if (mCurrentLevel) {
		// A compelling reason to separate out PhysicsComponents, and possibly Bounds
		for (unsigned int i = 0; i < mWorkingListSize; i++) {
			PhysicsComponent& I = mWorkingPCList[i];
			if (I.getBounds()) {
				for (unsigned int j = i + 1; j < mWorkingListSize; j++) {
					PhysicsComponent& J = mWorkingPCList[j];
					if (J.getBounds()) {
						if (collides(I.getBounds(), J.getBounds())) {
							CollisionData collision;
							collision.first = &I;
							collision.second = &J;
							mWorkingObjectList[i].handle(Event(collision));
							mWorkingObjectList[j].handle(Event(collision));
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
		if (!mPaused) {
			// Object updates
			for (unsigned int i = 0; i < mWorkingListSize; i++) {
				mWorkingObjectList[i].update(dt);	// A compelling reason to separate further into Components which can be updated individually
			}
		}
		mSoundSystem.update();
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
		for (unsigned int i = 0; i < mWorkingListSize; i++)
			mWorkingObjectList[i].render(mWorkingActiveCamera);
		// Draw any debug information
		if (mDebugMode != DEBUG_NONE) {
			// Draw object axes
			auto program = mCommonLibraries.standard.programs.get("debug_axes");
			program->use();
			program->sendUniform("uVP", glm::value_ptr(mWorkingActiveCamera->getCameraComponent()->getViewProjectionMatrix()));
			program->sendUniform("uM", glm::value_ptr(glm::mat4(1.0f)));
			Geometry::drawAxes();
			for (unsigned int i = 0; i < mWorkingListSize; i++) {
				program->sendUniform("uM", glm::value_ptr(mWorkingObjectList[i].getPhysicsComponent()->getWorldTransform()));
				Geometry::drawAxes();
			}
			// Draw a debug value with solid colors
			program = mCommonLibraries.standard.programs.get("debug_bbs");
			program->use();
			program->sendUniform("uVP", glm::value_ptr(mWorkingActiveCamera->getCameraComponent()->getViewProjectionMatrix()));
			switch (mDebugMode) {
			case DEBUG_COLLISION:
				for (unsigned int i = 0; i < mWorkingListSize; i++) {
					if (mWorkingObjectList[i].hasCollision())
						program->sendUniform("uDebugColor", 4, 1, debug_true);
					else
						program->sendUniform("uDebugColor", 4, 1, debug_false);
					program->sendUniform("uM", glm::value_ptr(glm::mat4(1.0f)));	// AABB's and spheres, at least, will be tracking their world coordinates
					mWorkingObjectList[i].debugDraw();
				}
				break;
			case DEBUG_MOUSE:
				for (unsigned int i = 0; i < mWorkingListSize; i++) {
					if (mWorkingObjectList[i].hasMouseOver())
						program->sendUniform("uDebugColor", 4, 1, debug_true);
					else
						program->sendUniform("uDebugColor", 4, 1, debug_false);
					program->sendUniform("uM", glm::value_ptr(glm::mat4(1.0f)));
					mWorkingObjectList[i].debugDraw();
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

	mWindow->update();

	// Mouse picking stuff
	MouseHandler::getInstance().setAsDrawingTarget();
	Program* prog = mCommonLibraries.standard.programs.get("mouse_selection");
	prog->use();
	if (mWorkingActiveCamera) {
		prog->sendUniform("uVP", glm::value_ptr(mWorkingActiveCamera->getCameraComponent()->getViewProjectionMatrix()));
		prog->sendUniform("uCamera", 3, 1, glm::value_ptr(mWorkingActiveCamera->getPhysicsComponent()->getGlobalPosition()));
		for (unsigned int i = 0; i < mWorkingListSize; i++) {
			prog->sendUniform("uObjectID", mWorkingObjectList[i].getIndex());
			mWorkingObjectList[i].render(prog);
		}
	}
}

void Game::resize(unsigned int width, unsigned int height) {
	// Presumably OpenGL won't let me resize it to something impossible, so there's no need to check the dimensions
	mWindow->resize(width, height);
	MouseHandler::getInstance().resize(width, height);
	if (mCurrentLevel)
		for (auto a : mCurrentLevel->getAllCameras())
			mWorkingObjectList[a].getCameraComponent()->resize(width, height);
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
		case 'm':
			mMasterVolume = 1.0f - mMasterVolume;
			mSoundSystem.getInstance().setVolume(mMasterVolume);
			break;
		case 27:
			// Pause / unpause
			// In a comment around here somewhere I have the code snippet to push a menu on the stack
			break;
		}
		break;
	}
}