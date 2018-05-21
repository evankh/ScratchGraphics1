#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME__
#define __EKH_SCRATCH_GRAPHICS_1_GAME__

#include "Camera.h"
//#include "GameObject.h"
class GameObject;
#include "NamedContainer.h"
#include "PostProcessingPipeline.h"
class Program;
//#include "StateMachine.h"
//#include "Window.h"
class Window;

#include <map>

enum GameState {
	EKH_PLAYING_STATE,
	EKH_PAUSE_STATE,
	EKH_MENU_STATE
};

class Game {
private:
	Game();
	~Game();
	Window* mWindow;
	OrthoCamera* mScreenCamera;
	PerspCamera* mSceneCamera;
	// I guess I could also do a std::vector<Camera> & a Camera* active...
	//std::map<std::string, Program*> mShaderPrograms;
	//std::map<std::string, Geometry*> mDebugUsefulGeometries;	// And can/should expand on this later to cache OBJ files so as to only read them once...
	NamedContainer<Shader*> mShaders = NamedContainer<Shader*>(NULL);
	NamedContainer<Program*> mPrograms = NamedContainer<Program*>(NULL);
	NamedContainer<Geometry*> mGeometries = NamedContainer<Geometry*>(NULL);
	std::vector<GameObject*> mGameObjects;
	PostProcessingPipeline mGameObjectsPost;
	std::vector<GameObject*> mHUDItems;
	PostProcessingPipeline mMenuPost;
	//StateMachine<GameState> mStateMachine;
	bool mIsMenuActive = false;
public:
	static Game& getInstance();
	void init();
	void cleanup();
	void update(float dt);
	void render();
	void resize(unsigned int width, unsigned int height);
	void loadLevel();
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME__