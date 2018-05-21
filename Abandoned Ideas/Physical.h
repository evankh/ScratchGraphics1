#ifndef __EKH_SCRATCH_GRAPHICS_1_PHYSICAL__
#define __EKH_SCRATCH_GRAPHICS_1_PHYSICAL__

#include "glm/mat4x4.hpp"
#include "../Bounds.h"

class Physical {
private:
	glm::mat4 mModelMatrix;
	Bounds* mBounds = NULL;
public:
	void update(float dt);
	void translate(glm::vec3 dxyz);
	void scale(glm::vec3 scale);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_PHYSICAL__