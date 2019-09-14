#include "Graphics/Lamp.h"
#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"

Lamp::Lamp() {
	mLocation = glm::vec3(0.0f, 0.0f, 3.0f);
	mType = EKH_POINT_LAMP;
	mColor = glm::vec3(1.0f);
	mIntensity = 1.0f;
}

Lamp::~Lamp() {
}

void Lamp::transfer(unsigned int location) {
	// Must avtivate program before calling this function!
	// Argument is the location in shader of the data for this particular light, e.g. glGetUniformLocation("lights") + ((4 floats + 1 int ) * index of this light)
	glUniform3fv(location, 1, glm::value_ptr(mLocation));
	glUniform3fv(location + 3 * sizeof(float), 1, glm::value_ptr(mColor));
	glUniform1f(location + 6 * sizeof(float), mIntensity);
	glUniform1i(location + 7 * sizeof(float), (int)mType);

}
