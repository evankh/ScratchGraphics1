#include "Components/Graphics.h"
#include "Components/Geometry.h"
#include "Components/Physics.h"
#include "Core/GameObject.h"
#include "Graphics/Camera.h"
#include "Graphics/Program.h"
#include "Graphics/Texture.h"
#include <glm/gtc/type_ptr.hpp>

GraphicsComponent::GraphicsComponent(Program* program) {
	mProgram = program;
}

void GraphicsComponent::activate(GameObject* owner, GameObject* camera) {
	mProgram->use();
	PhysicsComponent* model = owner->getPhysicsComponent();
	if (model) {
		mProgram->sendUniform("uM", glm::value_ptr(model->getWorldTransform()));
		mProgram->sendUniform("uInverseM", glm::value_ptr(model->getInverseWorldTransform()));
	}
	mProgram->sendUniform("uVP", glm::value_ptr(camera->getCameraComponent()->getViewProjectionMatrix()));
	model = camera->getPhysicsComponent();
	mProgram->sendUniform("uCamera", 3, 1, glm::value_ptr(model->getGlobalPosition()));
	for (unsigned int i = 0; i < mTextures.size(); i++)
		mTextures[i]->activate(i);
	if (mHasColor) mProgram->sendUniform("uColor", 3, 1, glm::value_ptr(mColor));
}

void GraphicsComponent::render(GeometryComponent* geometry) const {
	geometry->render();
}

GraphicsComponent* GraphicsComponent::makeCopy() const {
	GraphicsComponent* result = new GraphicsComponent(mProgram);
	if (mHasColor) result->setColor(mColor);
	for (auto i : mTextures)
		result->addTexture(i);
	return result;
}