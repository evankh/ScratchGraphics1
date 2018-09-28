#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME__
#define __EKH_SCRATCH_GRAPHICS_1_GAME__

#include "Camera.h"
class GameObject;
class Level;
#include "NamedContainer.h"
#include "PostProcessingPipeline.h"
class Program;
#include "ShaderManager.h"
#include "Texture.h"
#include "Event System\Receiver.h"
class Window;

#include <map>

enum GameState {
	EKH_PLAYING_STATE,
	EKH_PAUSE_STATE,
	EKH_MENU_STATE
};

class Game :public Receiver {
	friend class Level;
private:
	Game();
	~Game();
	Window* mWindow;
	OrthoCamera* mScreenCamera;
	PerspCamera* mSceneCamera;
	// I guess I could also do a std::vector<Camera> & a Camera* active...
	ShaderManager mShaders;
	NamedContainer<Program*> mPrograms = NamedContainer<Program*>(NULL);
	NamedContainer<Geometry*> mGeometries = NamedContainer<Geometry*>(NULL);
	NamedContainer<Level*> mLevels = NamedContainer<Level*>(NULL);
	Level* mCurrentLevel;
	NamedContainer<InputComponent*> mInputs = NamedContainer<InputComponent*>(NULL);
	NamedContainer<Texture*> mTextures = NamedContainer<Texture*>(NULL);
	PostProcessingPipeline mGameObjectsPost;
	std::vector<GameObject*> mHUDItems;
	PostProcessingPipeline mMenuPost;
	bool mIsMenuActive = false;
	const std::string mAssetBasePath = "assets/";
	const std::string mIndexFilename = "index.txt";
public:
	static Game& getInstance();
	void init();
	void load();
	void cleanup();
	void update(float dt);
	void render(float dt);
	void resize(unsigned int width, unsigned int height);
	void reloadAll();
	void setGLVersion(unsigned int GLVersion) { mShaders.setGLVersion(GLVersion); };
	virtual void handle(const Event event);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME__