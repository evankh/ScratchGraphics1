#include "Physical.h"
#include <glm\gtc\matrix_transform.hpp>

void Physical::update(float dt) {

}

void Physical::translate(glm::vec3 dxyz) {
	mModelMatrix = glm::translate(mModelMatrix, dxyz);
}

void Physical::scale(glm::vec3 scale) {
	mModelMatrix = glm::scale(mModelMatrix, scale);
}
