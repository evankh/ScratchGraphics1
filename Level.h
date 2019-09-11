#ifndef __EKH_SCRATCH_GRAPHICS_1_LEVEL__
#define __EKH_SCRATCH_GRAPHICS_1_LEVEL__

#include "GameObject.h"
#include "Geometry.h"
#include "NamedContainer.h"
#include "NamedTree.h"
class PhysicsComponent;
#include "Program.h"
#include "ShaderManager.h"
#include "SoundLibrary.h"
class Source;
#include "Texture.h"

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
	std::vector<std::string> mSceneCameras;
	NamedTree<GameObject*> mSceneGraph;
	std::string mMainCamera;
	//Source* mSceneAudio;	// And then each menu or HUD could have its own Source just like this, because presumably each will only be playing one sound at a time
	Sound* mBackgroundMusic;
	StandardLibraries& mSharedLibraries;
	StandardLibraries& mOwnLibraries;
	std::vector<PhysicsComponent*> mPCs;
public:
	Level(std::string filepath, StandardLibraries& sharedLibraries, StandardLibraries& ownLibraries);
	~Level();
	int getMainCameraIndex() const;
	std::vector<int> getAllCameras() const;
	unsigned int getNumObjects() const { return mSceneGraph.count(); };
	void getWorkingSet(GameObject** workingSet, PhysicsComponent** workingPCs);
	Sound* getBackgroundMusic() const { return mBackgroundMusic; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_LEVEL__