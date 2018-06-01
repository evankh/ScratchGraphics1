#include "Program.h"
#include "GL\glew.h"
#include "ServiceLocator.h"
#include "Shader.h"

Program::Program() {
	mHandle = glCreateProgram();
	mVS = NULL;
	mGS = NULL;
	mFS = NULL;
}

Program::Program(Shader* vs, Shader* fs) {
	mHandle = glCreateProgram();
	mVS = vs;
	mFS = fs;
	if (mVS) glAttachShader(mHandle, mVS->mHandle);
	if (mFS) glAttachShader(mHandle, mFS->mHandle);
	link();
	validate();
	if (mVS) glDetachShader(mHandle, mVS->mHandle);
	if (mFS) glDetachShader(mHandle, mFS->mHandle);
}

Program::Program(Shader* vs, Shader* gs, Shader* fs) {
	mHandle = glCreateProgram();
	mVS = vs;
	mGS = gs;
	mFS = fs;
	if (mVS) glAttachShader(mHandle, mVS->mHandle);
	if (mGS) glAttachShader(mHandle, mGS->mHandle);
	if (mFS) glAttachShader(mHandle, mFS->mHandle);
	link();
	validate();
	if (mVS) glDetachShader(mHandle, mVS->mHandle);
	if (mGS) glDetachShader(mHandle, mGS->mHandle);
	if (mFS) glDetachShader(mHandle, mFS->mHandle);
}

// Deprecated
Program::Program(char* vsFilepath, char* fsFilepath) {
	mHandle = glCreateProgram();
	mVS = new Shader(vsFilepath, GL_VERTEX_SHADER);
	mFS = new Shader(fsFilepath, GL_FRAGMENT_SHADER);
	glAttachShader(mHandle, mVS->mHandle);
	glAttachShader(mHandle, mFS->mHandle);
	link();
	validate();
	glDetachShader(mHandle, mVS->mHandle);
	glDetachShader(mHandle, mFS->mHandle);
}

Program::~Program() {
	//delete mVS;
	//delete mFS;
	glDeleteProgram(mHandle);
}

void Program::attach(Shader* vs, Shader* fs) {
	mVS = vs;
	mFS = fs;
	glAttachShader(mHandle, mVS->mHandle);
	glAttachShader(mHandle, mFS->mHandle);
}

void Program::link() {
	glLinkProgram(mHandle);
	int status;
	glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
	if (!status) {
		int length;
		glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);
		if (length) {
			char* log = new char[length];
			glGetProgramInfoLog(mHandle, length, &length, log);
			ServiceLocator::getLoggingService().error("Program linking failed", log);
			delete[] log;
		}
	}
}

void Program::validate() {
	glValidateProgram(mHandle);
	int status;
	glGetProgramiv(mHandle, GL_VALIDATE_STATUS, &status);
	if (!status) {
		int length;
		glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);
		if (length) {
			char* log = new char[length];
			glGetProgramInfoLog(mHandle, length, &length, log);
			ServiceLocator::getLoggingService().error("Shader validation failed", log);
			delete[] log;
		}
	}
}

void Program::use() {
	glUseProgram(mHandle);
}

void Program::sendUniform(const char* name, float value) {
	unsigned int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) glUniform1f(handle, value);
}

void Program::sendUniform(const char* name, float* matrix) {
	unsigned int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) glUniformMatrix4fv(handle, 1, GL_FALSE, matrix);
}

void Program::sendUniform(const char* name, int size, float* value) {
	unsigned int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) glUniform1fv(handle, size, value);
	bool error = (glGetError() == GL_INVALID_OPERATION);
}