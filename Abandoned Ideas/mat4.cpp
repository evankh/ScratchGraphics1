#include "mat4.h"

vec4::vec4() {
	for (int i = 0; i < 4; i++)
		values[i] = 0;
}

vec4::vec4(float x, float y, float z, float w) {
	values[0] = x;
	values[1] = y;
	values[2] = z;
	values[3] = w;
}

vec4::vec4(float* value) {
	*values = *value;
}

float& vec4::operator[](unsigned int index) {
	// vectors are 1-indexed, sorry
	return values[index - 1];
}

vec4 vec4::operator+(vec4 & other) {
	vec4 result;
	for (int i = 0; i < 4; i++)
		result.values[i] = values[i] + other.values[i];
	return result;
}

vec4 vec4::operator*(vec4 &other) {
	vec4 result;
	for (int i = 0; i < 4; i++)
		result.values[i] = values[i] * other.values[i];
	return result;
}

float vec4::dot(vec4 &other) {
	float result = 0.0f;
	for (int i = 0; i < 4; i++)
		result += values[i] * other.values[i];
	return result;
}

vec4 vec4::operator*(float c) {
	vec4 result;
	for (int i = 0; i < 4; i++)
		result.values[i] = values[i] * c;
	return result;
}

std::ostream& operator<<(std::ostream &stream, vec4 vector) {
	stream << "[ ";
	for (int i = 0; i < 4; i++)
		stream << vector.values[i] << " ";
	stream << "]";
	return stream;
}

mat4::mat4() {
	for (int i = 0; i < 4; i++)
		data.rows[i] = vec4();
}

vec4& mat4::operator[](unsigned int index) {
	// matrices are 1-indexed, sorry
	return data.rows[index - 1];
}

mat4 mat4::operator+(mat4 &other) {
	mat4 result;
	for (int i = 0; i < 4; i++)
		result.data.rows[i] = data.rows[i] + other.data.rows[i];
	return result;
}

vec4 mat4::operator*(vec4 &other) {
	vec4 result;
	for (int i = 0; i < 4; i++)
		result[i + 1] = (*this)[i + 1].dot(other);
	return result;
}

mat4 mat4::operator*(float c) {
	mat4 result;
	for (int i = 0; i < 4; i++)
		result.data.rows[i] = data.rows[i] * c;
	return result;
}

std::ostream& operator<<(std::ostream &stream, mat4 matrix) {
	stream << "[ ";
	for (int i = 0; i < 3; i++)
		stream << matrix.data.rows[i] << std::endl << "  ";
	stream << matrix.data.rows[3] << " ]";
	return stream;
}

mat4 makeTranslationMatrix(float x, float y, float z) {
	mat4 result;
	result[1] = vec4(1, 0, 0, x);
	result[2] = vec4(0, 1, 0, y);
	result[3] = vec4(0, 0, 1, z);
	result[4] = vec4(0, 0, 0, 1);
	return result;
}

mat4 makeTranslationMatrix(vec4 pos) {
	mat4 result;
	result[1] = vec4(1, 0, 0, pos[1]);
	result[2] = vec4(0, 1, 0, pos[2]);
	result[3] = vec4(0, 0, 1, pos[3]);
	result[4] = vec4(0, 0, 0, 1);
	return result;
}