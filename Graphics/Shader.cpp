#include "Util/ServiceLocator.h"
#include "Graphics/Shader.h"
#include "GL/glew.h"
#include <fstream>

Shader::Shader(const char* shader, unsigned int type) {
	mType = type;
	mHandle = glCreateShader(mType);
	int length;
	if (mType == GL_VERTEX_SHADER) length = 145;	// This is dumb and terrible, but it probably won't work anyway!
	else length = 140;
	glShaderSource(mHandle, 1, &shader, &length);
	glCompileShader(mHandle);
	int status;
	glGetShaderiv(mHandle, GL_COMPILE_STATUS, &status);
	if (!status) {
		int length;
		glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &length);
		if (length) {
			char* log = new char[length];
			glGetShaderInfoLog(mHandle, length, &length, log);
			ServiceLocator::getLoggingService().fileError(mFilepath, 0, "Shader compilation failed", log);
			delete[] log;
		}
		glDeleteShader(mHandle);
		mHandle = 0;
	}
}

Shader::Shader(std::string filepath, unsigned int type) {
	mFilepath = filepath;
	mType = type;
	// Read file
	/*std::ifstream in(mFilepath);
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
		allText[count - 1] = '\0';*/
	FileService file(mFilepath);
	if (file.good()) {
		std::string allText = file.getAll();
		const char* paininmyass = allText.data();
		int size = allText.size();
		// Move to GPU
		mHandle = glCreateShader(mType);
		glShaderSource(mHandle, 1, &paininmyass, &size);
		glCompileShader(mHandle);
		int status;
		glGetShaderiv(mHandle, GL_COMPILE_STATUS, &status);
		if (!status) {
			int length;
			glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &length);
			if (length) {
				char* log = new char[length];
				glGetShaderInfoLog(mHandle, length, &length, log);
				ServiceLocator::getLoggingService().fileError(mFilepath, 0, "Shader compilation failed", log);
				delete[] log;
			}
			glDeleteShader(mHandle);
			mHandle = 0;
		}
	}
	else {
		throw std::exception(filepath.data());
	}
}

Shader::~Shader() {
	// Maybe delete the filepath
	glDeleteShader(mHandle);
}
