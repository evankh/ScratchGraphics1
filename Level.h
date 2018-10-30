#ifndef __EKH_SCRATCH_GRAPHICS_1_LEVEL__
#define __EKH_SCRATCH_GRAPHICS_1_LEVEL__

class Camera;
#include "Game.h"
#include "GameObject.h"
#include "NamedContainer.h"

#include <string>
#include <vector>

class Level {
	friend class Game;
private:
	std::map<std::string, GameObject*> mSceneObjects;
	std::map<std::string, Camera*> mSceneCameras;
	Camera* mCurrentCamera;
	Source* mSceneAudio;	// And then each menu or HUD could have its own Source just like this, because presumably each will only be playing one sound at a time
	Sound* mBackgroundMusic;
	// A set of managers used for validation of level data
	static const NamedContainer<Geometry*>& sGeometryLibrary;
	static const NamedContainer<Program*>& sProgramLibrary;
	static const NamedContainer<Texture*>& sTextureLibrary;
	//static const NamedContainer<InputComponent>& sInputLibrary;
	static const NamedContainer<PhysicsComponent*>& sPhysicsLibrary;
	static const SoundLibrary& sSoundLibrary;
public:
	Level(const char* filepath);
	Level(std::string filepath);
	~Level();
	std::map<std::string, GameObject*>& getObjectList() { return mSceneObjects; };
	std::map<std::string, Camera*>& getCameraList() { return mSceneCameras; };
	Camera* getCurrentCamera() const { return mCurrentCamera; };
	void setBackgroundMusicVolume(float volume);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_LEVEL__