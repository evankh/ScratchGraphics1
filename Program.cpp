#include "Program.h"
#include "GL\glew.h"
#include "ServiceLocator.h"
#include "Shader.h"

Program* Program::sScreenDraw = NULL;

Program::Program() {
	mHandle = glCreateProgram();
	mVS = NULL;
	mGS = NULL;
	mFS = NULL;
}

Program::Program(const char* vs, const char* fs) {
	mHandle = glCreateProgram();
	mVS = new Shader(vs, GL_VERTEX_SHADER);
	mFS = new Shader(fs, GL_FRAGMENT_SHADER);
	if (mVS) glAttachShader(mHandle, mVS->mHandle);
	if (mFS) glAttachShader(mHandle, mFS->mHandle);
	link();
	validate();
	if (mVS) glDetachShader(mHandle, mVS->mHandle);
	if (mFS) glDetachShader(mHandle, mFS->mHandle);
}

void Program::generateScreenDrawProgram() {
	const char* screenVS = "#version 330\nlayout (location = 8) in vec2 iTexCoord;out vec2 pTexCoord;void main() {gl_Position=vec4(iTexCoord,0.0f,1.0f);pTexCoord=iTexCoord;}";
	const char* screenFS = "#version 150\nin vec2 pTexCoord;out vec4 oFragColor;uniform sampler2D uColorBuffer;void main() {oFragColor=texture(uColorBuffer,pTexCoord);}";
	sScreenDraw = new Program(screenVS, screenFS);
}

Program::Program(Shader* vs, Shader* fs) {
	if (!sScreenDraw)
		generateScreenDrawProgram();	// Since this is the constructor we're actually using, let's put it here
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

void Program::attach(Shader* s, unsigned int type) {
	if (type == GL_VERTEX_SHADER)
		mVS = s;
	else if (type == GL_FRAGMENT_SHADER)
		mFS = s;
	else if (type == GL_GEOMETRY_SHADER)
		mGS = s;
	glAttachShader(mHandle, s->mHandle);
	// When to detach?
}

void Program::attach(Shader* vs, Shader* fs) {
	mVS = vs;
	mFS = fs;
	glAttachShader(mHandle, mVS->mHandle);
	glAttachShader(mHandle, mFS->mHandle);
}

void Program::detachAll() {
	if (mVS) glDetachShader(mHandle, mVS->mHandle);
	if (mGS) glDetachShader(mHandle, mGS->mHandle);
	if (mFS) glDetachShader(mHandle, mFS->mHandle);
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

void Program::use() const {
	glUseProgram(mHandle);
}

void Program::sendUniform(const char* name, const float value) const {
	int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) glUniform1f(handle, value);
}

void Program::sendUniform(const char* name, const float* matrix) const {
	int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) glUniformMatrix4fv(handle, 1, GL_FALSE, matrix);
}

void Program::sendUniform(const char* name, const int size, const float* value) const {
	int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) glUniform1fv(handle, size, value);
	bool x = glGetError() == GL_INVALID_VALUE;	// DEBUG: trying to figure out why colors don't work
	bool y = glGetError() == GL_INVALID_OPERATION;
	bool z = glGetError() == GL_NONE;
}

void Program::sendUniform(const char* name, const unsigned int value) const {
	int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) glUniform1i(handle, value);
}