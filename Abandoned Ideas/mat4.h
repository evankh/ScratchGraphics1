#ifndef __EKH_SCRATCH_GRAPHICS_1_MAT4__
#define __EKH_SCRATCH_GRAPHICS_1_MAT4__

#include <iostream>

class vec4 {
	friend std::ostream& operator<<(std::ostream&, vec4);
private:
	float values[4];
public:
	vec4();
	vec4(float x, float y, float z, float w);
	vec4(float* values);
	float& operator[](unsigned int index);
	vec4 operator+(vec4 &other);
	vec4 operator*(vec4 &other);
	float dot(vec4 &other);
	vec4 operator*(float c);
};

std::ostream& operator<<(std::ostream& stream, vec4 vector);

class mat4 {
	friend mat4 makeTranslationMatrix(float, float, float);
	friend mat4 makeTranslationMatrix(vec4);
	friend mat4 makeScaleMatrix(float);
	friend mat4 makeScaleMatrix(float, float, float);
	friend mat4 makeScaleMatrix(vec4);
	friend std::ostream& operator<<(std::ostream&, mat4);
private:
	union Data {
		float values[16];
		vec4 rows[4];
		Data() {};
	} data;
public:
	mat4();
	vec4& operator[](unsigned int index);
	mat4 operator+(mat4 &other);
	mat4 operator*(mat4 &other);
	vec4 operator*(vec4 &other);
	mat4 operator*(float c);
};

std::ostream& operator<<(std::ostream& stream, mat4 matrix);

mat4 makeTranslationMatrix(float x, float y, float z);
mat4 makeTranslationMatrix(vec4 pos);
mat4 makeRotationMatrix(float theta, vec4 angle);
mat4 makeScaleMatrix(float scale);
mat4 makeScaleMatrix(float xscale, float yscale, float zscale);
mat4 makeScaleMatrix(vec4 scale);

#endif//__EKH_SCRATCH_GRAPHICS_1_MAT4__