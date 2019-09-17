#ifndef __EKH_SCRATCH_GRAPHICS_1_LEVEL__
#define __EKH_SCRATCH_GRAPHICS_1_LEVEL__

#include "Core/GameObject.h"
#include "Components/Geometry.h"
#include "Util/NamedContainer.h"
#include "Util/NamedTree.h"
class PhysicsComponent;
#include "Graphics/Program.h"
#include "Util/ShaderManager.h"
#include "Util/SoundLibrary.h"
class Source;
#include "Graphics/Texture.h"

#include <string>
#include <vector>

struct StandardLibraries {
	NamedContainer<GeometryComponent*> geometries;
	ShaderManager shaders;
	NamedContainer<Program*> programs;
	NamedContainer<Texture*> textures;
	SoundLibrary sounds;
};
class Level {
private:
	std::vector<std::string> mSceneCameras;	// A list of all the names of Camera GameObjects in the level
	NamedTree<GameObject*> mSceneGraph;		// A hierarchy of all the GameObjects in the level
	std::string mMainCamera;				// The name of the camera object to start viewing through
	//Source* mSceneAudio;	// And then each menu or HUD could have its own Source just like this, because presumably each will only be playing one sound at a time
	Sound* mBackgroundMusic;				// The primary sound playing all the time
	StandardLibraries& mSharedLibraries;	// Libraries of assets shared between all levels
	StandardLibraries& mOwnLibraries;		// Libraries of assets unique to this level
	std::vector<PhysicsComponent*> mPCs;	// This exists solely to delete the PCs that GameObject is no longer responsible for
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