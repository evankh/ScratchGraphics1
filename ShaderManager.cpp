#include "ShaderManager.h"

void ShaderManager::setGLVersion(int version) {
	// Todo: Check validity
	mGLVersion = version;
}

void ShaderManager::add(std::string name, int version, Shader* shader) {
	if (mShaderLibrary.count(name) && mShaderLibrary[name].count(version))
		delete mShaderLibrary[name][version];
	mShaderLibrary[name][version] = shader;
}

Shader* ShaderManager::get(std::string name) {
	if (mShaderLibrary.count(name)) {
		// Get the highest version less than or equal to the stored version
		int highestversion = 0;
		for (auto version : mShaderLibrary[name]) {
			if (version.first <= mGLVersion)
				highestversion = version.first;
		}
		if (highestversion)
			return mShaderLibrary[name][highestversion];
		else
			return NULL;	// All stored shader versions are too high
	}
	else
		return NULL;
}

void ShaderManager::clear() {
	for (auto name : mShaderLibrary) {
		for (auto shader : name.second)
			delete shader.second;
		name.second.clear();
	}
	mShaderLibrary.clear();
}
