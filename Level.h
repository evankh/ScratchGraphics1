#ifndef __EKH_SCRATCH_GRAPHICS_1_LEVEL__
#define __EKH_SCRATCH_GRAPHICS_1_LEVEL__

#include "Game.h"
#include "GameObject.h"
#include "NamedContainer.h"

#include <vector>

class Level {
	friend class Game;
private:
	std::vector<GameObject*> mGameObjects;
	// A set of managers used for validation of level data
	static const NamedContainer<Geometry*>& sGeometryLibrary;
	static const NamedContainer<Program*>& sProgramLibrary;
	static const NamedContainer<Texture*>& sTextureLibrary;
	static const NamedContainer<InputComponent*>& sInputLibrary;
	static const NamedContainer<PhysicsComponent*>& sPhysicsLibrary;
	static const SoundLibrary& sSoundLibrary;
public:
	Level(const char* filepath);
	Level(std::string filepath);
	~Level();
	std::vector<GameObject*>& getObjectList() { return mGameObjects; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_LEVEL__