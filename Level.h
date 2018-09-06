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
	static NamedContainer<Geometry*>& sGeometryLibrary;
	static ShaderManager& sShaderLibrary;
	static NamedContainer<Texture*>& sTextureLibrary;
	static NamedContainer<InputComponent*>& sInputLibrary;
	static NamedContainer<PhysicsComponent*>& sPhysicsLibrary;
public:
	Level(const char* filepath);
	~Level();
	std::vector<GameObject*>& getObjectList() { return mGameObjects; };
};

#endif//__EKH_SCRATCH_GRAPHICS_1_LEVEL__