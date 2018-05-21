#include "ServiceLocator.h"
#include "Shader.h"
#include "GL\glew.h"
#include <fstream>

Shader::Shader(char* filepath, unsigned int type) {
	mFilepath = filepath;
	mType = type;
	// Read file
	std::ifstream in(mFilepath);
	if (in.good()) {
		int count = 0;
		while (!in.eof()) {
			count++;
			in.get();
		}
		char* allText = new char[count];
		in.clear();
		in.seekg(0);
		in.read(allText, count);
		in.close();
		allText[count - 1] = '\0';
		// Move to GPU
		mHandle = glCreateShader(mType);
		glShaderSource(mHandle, 1, &allText, &count);
		glCompileShader(mHandle);
		int status;
		glGetShaderiv(mHandle, GL_COMPILE_STATUS, &status);
		if (!status) {
			int length;
			glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &length);
			if (length) {
				char* log = new char[length];
				glGetShaderInfoLog(mHandle, length, &length, log);
				ServiceLocator::getLoggingService().error("Shader compilation failed", log);
				delete[] log;
			}
			glDeleteShader(mHandle);
			mHandle = 0;
		}
		delete[] allText;
	}
	else {
		ServiceLocator::getLoggingService().error("Failed to open file", filepath);
	}
}

Shader::~Shader() {
	// Maybe delete the filepath
	glDeleteShader(mHandle);
}