#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME__
#define __EKH_SCRATCH_GRAPHICS_1_GAME__

#include "Camera.h"
class GameObject;
#include "NamedContainer.h"
#include "PostProcessingPipeline.h"
class Program;
#include "Event System\Receiver.h"
class Window;

#include <map>

enum GameState {
	EKH_PLAYING_STATE,
	EKH_PAUSE_STATE,
	EKH_MENU_STATE
};

class Game :public Receiver {
private:
	Game();
	~Game();
	Window* mWindow;
	OrthoCamera* mScreenCamera;
	PerspCamera* mSceneCamera;
	// I guess I could also do a std::vector<Camera> & a Camera* active...
	NamedContainer<Shader*> mShaders = NamedContainer<Shader*>(NULL);
	NamedContainer<Program*> mPrograms = NamedContainer<Program*>(NULL);
	NamedContainer<Geometry*> mGeometries = NamedContainer<Geometry*>(NULL);
	std::vector<GameObject*> mGameObjects;
	PostProcessingPipeline mGameObjectsPost;
	std::vector<GameObject*> mHUDItems;
	PostProcessingPipeline mMenuPost;
	bool mIsMenuActive = false;
	void loadShaders();
	void loadGeometry();
	void loadMenu();
	void loadPostProcessing();
public:
	static Game& getInstance();
	void init();
	void cleanup();
	void update(float dt);
	void render();
	void resize(unsigned int width, unsigned int height);
	void loadLevel();
	void reloadAll();
	virtual void handle(Event event);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME__