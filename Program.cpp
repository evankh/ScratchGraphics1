#include "Program.h"
#include "GL\glew.h"
#include "ServiceLocator.h"
#include "Shader.h"

Program* Program::sScreenDraw = NULL;

Program::Program() {
	mHandle = glCreateProgram();
	mVS = mTCS = mTES = mGS = mFS = mCS = NULL;
}

Program::Program(const char* vs, const char* fs) {
	mHandle = glCreateProgram();
	mVS = new ShaderNode{new Shader(vs, GL_VERTEX_SHADER), NULL };
	mFS = new ShaderNode{new Shader(fs, GL_FRAGMENT_SHADER), NULL };
	if (mVS->shader) glAttachShader(mHandle, mVS->shader->mHandle);
	if (mFS->shader) glAttachShader(mHandle, mFS->shader->mHandle);
	link();
	validate();
	if (mVS->shader) glDetachShader(mHandle, mVS->shader->mHandle);
	if (mFS->shader) glDetachShader(mHandle, mFS->shader->mHandle);
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
	mVS = new ShaderNode{ vs, NULL };
	mFS = new ShaderNode{ fs, NULL };
	if (mFS->shader) glAttachShader(mHandle, mFS->shader->mHandle);
	if (mVS->shader) glAttachShader(mHandle, mVS->shader->mHandle);
	link();
	validate();
	if (mVS->shader) glDetachShader(mHandle, mVS->shader->mHandle);
	if (mFS->shader) glDetachShader(mHandle, mFS->shader->mHandle);
}

Program::Program(Shader* vs, Shader* fs, int in, int out) :Program(vs, fs) {
	mSamplesIn = in;
	mSamplesOut = out;
}

Program::Program(Shader* vs, Shader* gs, Shader* fs) {
	mHandle = glCreateProgram();
	mVS = new ShaderNode{ vs, NULL };
	mGS = new ShaderNode{ gs, NULL };
	mFS = new ShaderNode{ fs, NULL };
	if (mVS->shader) glAttachShader(mHandle, mVS->shader->mHandle);
	if (mGS->shader) glAttachShader(mHandle, mGS->shader->mHandle);
	if (mFS->shader) glAttachShader(mHandle, mFS->shader->mHandle);
	link();
	validate();
	if (mVS->shader) glDetachShader(mHandle, mVS->shader->mHandle);
	if (mGS->shader) glDetachShader(mHandle, mGS->shader->mHandle);
	if (mFS->shader) glDetachShader(mHandle, mFS->shader->mHandle);
}

[[deprecated]]
Program::Program(char* vsFilepath, char* fsFilepath) {
	mHandle = glCreateProgram();
	mVS = new ShaderNode{new Shader(vsFilepath, GL_VERTEX_SHADER), NULL },
	mFS = new ShaderNode{new Shader(fsFilepath, GL_FRAGMENT_SHADER), NULL };
	glAttachShader(mHandle, mVS->shader->mHandle);
	glAttachShader(mHandle, mFS->shader->mHandle);
	link();
	validate();
	glDetachShader(mHandle, mVS->shader->mHandle);
	glDetachShader(mHandle, mFS->shader->mHandle);
}

Program::~Program() {
	glDeleteProgram(mHandle);
}

void Program::attach(Shader* s, unsigned int type) {
	if (type == GL_VERTEX_SHADER)
		mVS = new ShaderNode{ s, mVS };
	else if (type == GL_TESS_CONTROL_SHADER)
		mTCS = new ShaderNode{ s, mTCS };
	else if (type == GL_TESS_EVALUATION_SHADER)
		mTES = new ShaderNode{ s, mTES };
	else if (type == GL_GEOMETRY_SHADER)
		mGS = new ShaderNode{ s, mGS };
	else if (type == GL_FRAGMENT_SHADER)
		mFS = new ShaderNode{ s, mFS };
	else if (type == GL_COMPUTE_SHADER)
		mCS = new ShaderNode{ s, mCS };
	glAttachShader(mHandle, s->mHandle);
	// When to detach?
}

void Program::attach(Shader* vs, Shader* fs) {
	mVS = new ShaderNode{ vs, mVS };
	mFS = new ShaderNode{ fs, mFS };
	glAttachShader(mHandle, mVS->shader->mHandle);
	glAttachShader(mHandle, mFS->shader->mHandle);
}

void Program::detachAll() {
	for (ShaderNode* iter : { mVS,mTCS,mTES,mGS,mFS,mCS }) {
		while (iter) {
			glDetachShader(mHandle, iter->shader->mHandle);
			iter = iter->next;
		}
	}
}

void Program::removeAll() {
	while (mVS) {
		glDetachShader(mHandle, mVS->shader->mHandle);
		auto temp = mVS;
		mVS = mVS->next;
		delete temp;
	};
	while (mTCS) {
		glDetachShader(mHandle, mTCS->shader->mHandle);
		auto temp = mVS;
		mVS = mVS->next;
		delete temp;
	};
	while (mTES) {
		glDetachShader(mHandle, mTES->shader->mHandle);
		auto temp = mVS;
		mVS = mVS->next;
		delete temp;
	};
	while (mGS) {
		glDetachShader(mHandle, mGS->shader->mHandle);
		auto temp = mVS;
		mVS = mVS->next;
		delete temp;
	};
	while (mFS) {
		glDetachShader(mHandle, mFS->shader->mHandle);
		auto temp = mVS;
		mVS = mVS->next;
		delete temp;
	};
	while (mCS) {
		glDetachShader(mHandle, mCS->shader->mHandle);
		auto temp = mVS;
		mVS = mVS->next;
		delete temp;
	};
}

void Program::removeAll(unsigned int type) {
	if (type == GL_VERTEX_SHADER) {
		while (mVS) {
			glDetachShader(mHandle, mVS->shader->mHandle);
			auto temp = mVS;
			mVS = mVS->next;
			delete temp;
		};
	}
	if (type == GL_TESS_CONTROL_SHADER) {
		while (mTCS) {
			glDetachShader(mHandle, mTCS->shader->mHandle);
			auto temp = mVS;
			mVS = mVS->next;
			delete temp;
		};
	}
	if (type == GL_TESS_EVALUATION_SHADER) {
		while (mTES) {
			glDetachShader(mHandle, mTES->shader->mHandle);
			auto temp = mVS;
			mVS = mVS->next;
			delete temp;
		};
	}
	if (type == GL_GEOMETRY_SHADER) {
		while (mGS) {
			glDetachShader(mHandle, mGS->shader->mHandle);
			auto temp = mVS;
			mVS = mVS->next;
			delete temp;
		};
	}
	if (type == GL_FRAGMENT_SHADER) {
		while (mFS) {
			glDetachShader(mHandle, mFS->shader->mHandle);
			auto temp = mVS;
			mVS = mVS->next;
			delete temp;
		};
	}
	if (type == GL_COMPUTE_SHADER) {
		while (mCS) {
			glDetachShader(mHandle, mCS->shader->mHandle);
			auto temp = mVS;
			mVS = mVS->next;
			delete temp;
		};
	}
	// default: throw std::invalid_argument(type);
}

bool Program::link(bool allow_output) {
	glLinkProgram(mHandle);
	int status;
	glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
	if (!status) {
		int length;
		glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);
		if (length && allow_output) {
			char* log = new char[length];
			glGetProgramInfoLog(mHandle, length, &length, log);
			ServiceLocator::getLoggingService().error("Program linking failed", log);
			delete[] log;
		}
		return false;
	}
	return true;
}

bool Program::validate(bool allow_output) {
	glValidateProgram(mHandle);
	int status;
	glGetProgramiv(mHandle, GL_VALIDATE_STATUS, &status);
	if (!status) {
		int length;
		glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);
		if (length && allow_output) {
			char* log = new char[length];
			glGetProgramInfoLog(mHandle, length, &length, log);
			ServiceLocator::getLoggingService().error("Shader validation failed", log);
			delete[] log;
		}
		return false;
	}
	return true;
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

void Program::sendUniform(const char* name, const int size, const int count, const float* value) const {
	// This is inconsistent with how the function is being used in PostProcessingPipeline::process; it will cause kernels to fail if size > 4
	int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) {
		if (size == 1) glUniform1fv(handle, count, value);
		else if (size == 2) glUniform2fv(handle, count, value);
		else if (size == 3) glUniform3fv(handle, count, value);
		else if (size == 4) glUniform4fv(handle, count, value);
	}
}

void Program::sendUniform(const char* name, const int value) const {
	int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) glUniform1i(handle, value);
}

void Program::sendUniform(const char* name, const int count, const unsigned int* values) const {
	int handle = glGetUniformLocation(mHandle, name);
	if (handle != -1) glUniform1uiv(handle, count, values);
}