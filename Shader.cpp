#include "ServiceLocator.h"
#include "Shader.h"
#include "GL\glew.h"
#include <fstream>

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
	FileService& file = ServiceLocator::getFileService(mFilepath);
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
		ServiceLocator::getLoggingService().badFileError(filepath);
	}
	file.close();
}

Shader::~Shader() {
	// Maybe delete the filepath
	glDeleteShader(mHandle);
}
