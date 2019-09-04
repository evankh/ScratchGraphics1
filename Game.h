#ifndef __EKH_SCRATCH_GRAPHICS_1_GAME__
#define __EKH_SCRATCH_GRAPHICS_1_GAME__

#include "Level.h"
#include "NamedContainer.h"
#include "PostProcessingPipeline.h"
#include "SoundHandler.h"
#include "UI/Menu.h"
#include "UI/UIElement.h"
#include "Event System/Receiver.h"
class Window;

#include <map>

enum GameState {
	EKH_PLAYING_STATE,
	EKH_PAUSE_STATE,
	EKH_MENU_STATE
};

enum DebugMode {
	DEBUG_NONE,
	DEBUG_COLLISION,
	DEBUG_MOUSE
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
			NamedContainer<PostprocessingPipeline*> pipelines;
		} post;
	} mCommonLibraries;
	// Working set of Level data
	GameObject* mWorkingObjectList = nullptr;
	unsigned int mWorkingListSize = 0;
	GameObject* mWorkingActiveCamera;
	// Actual game information
	Window* mWindow;
	Level* mCurrentLevel;
	PostprocessingPipeline* mCurrentPostProcessing;
	Menu* mCurrentMenu = nullptr;
	Menu* mHUD = nullptr;
	PostprocessingPipeline* mCurrentMenuPost;
	SoundHandler& mSoundSystem = SoundHandler::getInstance();
	const std::string mAssetBasePath = "assets2/";
	const std::string mIndexFilename = "index.txt";
	DebugMode mDebugMode = DEBUG_NONE;
	bool mPaused = false;
	float mMasterVolume = 1.0f;
	Level* loadLevel(std::string path);
	void parseGeometryIndex(std::string path, NamedContainer<Geometry*> &geomLibrary);
	void parseShaderIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &progLibrary);
	//void parsePostprocessingIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &filterLibrary, KernelManager &kernelLibrary, NamedContainer<PostProcessingPipeline*> &pipelineLibrary);
	void parsePostprocessingIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &filterLibrary, KernelManager &kernelLibrary, NamedContainer<PostprocessingPipeline*> &pipelineLibrary);
	void parseSoundIndex(std::string path, SoundLibrary &soundLibrary);
	void parseMenuIndex(std::string path, NamedContainer<RootElement*> &menuLibrary);
	void parseTextureIndex(std::string path, NamedContainer<Texture*> &texLibrary);

	int mDebugStageSelection = 0;
public:
	static Game& getInstance();
	void init(int argc, char* argv[]);
	void load();
	void softReload();
	void run();
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