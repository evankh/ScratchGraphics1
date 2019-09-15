#ifndef __EKH_SCRATCH_GRAPHICS_1_GRAPHICS_COMPONENT__
#define __EKH_SCRATCH_GRAPHICS_1_GRAPHICS_COMPONENT__

class GameObject;
class GeometryComponent;
class Program;
#include "Graphics/Texture.h"

#include <vector>
#include <glm/vec3.hpp>

class GraphicsComponent {
private:
	Program* mProgram = nullptr;
	std::vector<Texture*> mTextures;
	//Texture* mTexture = nullptr;
	bool mHasColor = false;
	glm::vec3 mColor;
public:
	GraphicsComponent(Program* program);
	~GraphicsComponent() { mTextures.clear(); };
	void activate(GameObject* owner, GameObject* camera);
	void render(GeometryComponent* geometry);
	void addTexture(Texture* tex) { mTextures.push_back(tex); };
	void setColor(glm::vec3 col) { mHasColor = true; mColor = col; };
	GraphicsComponent* makeCopy() const;
};

#endif//__EKH_SCRATCH_GRAPHICS_1_GRAPHICS_COMPONENT__