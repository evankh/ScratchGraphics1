#ifndef __EKH_SCRATCH_GRAPHICS_1_LAMP__
#define __EKH_SCRATCH_GRAPHICS_1_LAMP__

#include "Components/Physics.h"
#include "glm\vec3.hpp"

enum LampType {
	EKH_SUN_LAMP,
	EKH_POINT_LAMP
};

class Lamp {
private:
	glm::vec3 mLocation;
	LampType mType;
	glm::vec3 mColor;
	float mIntensity;
public:
	Lamp();
	~Lamp();
	void transfer(unsigned int location);
};

#endif//__EKH_SCRATCH_GRAPHICS_1_LAMP__