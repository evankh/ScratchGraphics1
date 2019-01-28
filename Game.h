#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME__
#define __EKH_SCRATCH_GRAPHICS_1_GAME__

#include "Camera.h"
class GameObject;
class Geometry;
#include "KernelManager.h"
#include "Level.h"
class Menu;
class RootElement;
#include "NamedContainer.h"
#include "PostProcessingPipeline.h"
class Program;
#include "ShaderManager.h"
#include "SoundLibrary.h"
#include "SoundHandler.h"
class Texture;
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
	// Managers & Libraries
	NamedContainer<std::string> mLevelDirectory;
	struct {
		NamedContainer<RootElement*> menus;
		StandardLibraries standard;
		struct {
			ShaderManager shaders;	// Do I need a new container type to handle matching sample sizes? Probably.
			NamedContainer<Program*> filters;
			KernelManager kernels;
			NamedContainer<PostProcessingPipeline*> pipelines;
		} post;
	} mCommonLibraries;
	// Actual game information
	Window* mWindow;
	Level* mCurrentLevel;
	PostProcessingPipeline* mCurrentPostProcessing;
	Menu* mCurrentMenu = NULL;
	Menu* mHUD = NULL;
	PostProcessingPipeline* mCurrentMenuPost;
	SoundHandler& mSoundSystem = SoundHandler::getInstance();
	const std::string mAssetBasePath = "assets2/";
	const std::string mIndexFilename = "index.txt";
	bool mDebugMode = false, mPaused = false;
	Level* loadLevel(std::string path);
	void parseGeometryIndex(std::string path, NamedContainer<Geometry*> &geomLibrary);
	void parseShaderIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &progLibrary);
	void parsePostprocessingIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &filterLibrary, KernelManager &kernelLibrary, NamedContainer<PostProcessingPipeline*> &pipelineLibrary);
	void parseSoundIndex(std::string path, SoundLibrary &soundLibrary);
	void parseMenuIndex(std::string path, NamedContainer<RootElement*> &menuLibrary);
	void parseTextureIndex(std::string path, NamedContainer<Texture*> &texLibrary);
public:
	static Game& getInstance();
	void init();
	void load();
	void cleanup();
	void update(float dt);
	void render(float dt);
	void resize(unsigned int width, unsigned int height);
	void reloadAll();
	void setGLVersion(unsigned int GLVersion) { mCommonLibraries.standard.shaders.setGLVersion(GLVersion); };
	//bool isPlaying() const { return mPaused == false; };
	virtual void handle(const Event event);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GAME__