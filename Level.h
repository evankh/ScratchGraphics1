#ifndef __EKH_SCRATCH_GRAPHICS_1_LEVEL__
#define __EKH_SCRATCH_GRAPHICS_1_LEVEL__

class Camera;
class GameObject;
class Geometry;
#include "NamedContainer.h"
class PhysicsComponent;
class Program;
#include "ShaderManager.h"
#include "SoundLibrary.h"
class Source;
class Texture;

#include <string>
#include <vector>

struct StandardLibraries {
	NamedContainer<Geometry*> geometries;
	ShaderManager shaders;
	NamedContainer<Program*> programs;
	NamedContainer<Texture*> textures;
	SoundLibrary sounds;
};

class Level {
private:
	std::map<std::string, GameObject*> mSceneObjects;	// Why am I not using my NamedManagers here?
	std::map<std::string, Camera*> mSceneCameras;
	Camera* mCurrentCamera;
	Source* mSceneAudio;	// And then each menu or HUD could have its own Source just like this, because presumably each will only be playing one sound at a time
	Sound* mBackgroundMusic;
	StandardLibraries& mSharedLibraries;
	StandardLibraries mOwnLibraries;
public:
	Level(std::string filepath, StandardLibraries& sharedLibraries, StandardLibraries ownLibraries);
	~Level();
	std::map<std::string, GameObject*>& getObjectList() { return mSceneObjects; };
	std::map<std::string, Camera*>& getCameraList() { return mSceneCameras; };
	Camera* getCurrentCamera() const { return mCurrentCamera; };
	void setBackgroundMusicVolume(float volume);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_LEVEL__