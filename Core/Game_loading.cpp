#include "Core/Game.h"
#include "Graphics/PostProcessingPipeline.h"
#include "Util/ServiceLocator.h"
#include "Sound/Sound.h"
#include "Core/Window.h"
#include <string>

// All the boilerplate code that involves loading things from files.

void Game::load() {
	// Load certain very important objects (Shaders?)
	mCommonLibraries.standard.geometries.add("plane", GeometryComponent::getNewQuad());
	// Load everything else from file
	FileService baseIndex(mAssetBasePath + mIndexFilename);
	if (!baseIndex.good()) throw "Asset file could not be opened.";
	struct { int w, h; } resolution;
	while (baseIndex.good()) {
		struct { auto_cstr name, path; } levelData;
		auto_cstr commonFolder, ppp;
		if (baseIndex.extract("//`S`L"));	// Comment
		else if (baseIndex.extract("Level \"`S\" \"`S\"`L", &levelData)) {
			mLevelDirectory.add(levelData.name, levelData.path);
		}
		else if (baseIndex.extract("start: ")) {
			if (baseIndex.extract("Level \"`S\"`L", &levelData.name)) {
				std::string levelPath;
				try {
					levelPath = mLevelDirectory.get(levelData.name);
				}
				catch (std::exception) {
					throw std::exception((std::string("Starting level has not been defined: ") + levelData.name + mIndexFilename).data());
				}
				try {
					mCurrentLevel = loadLevel(mAssetBasePath + levelPath);
				}
				catch (std::exception e) {
					ServiceLocator::getLoggingService().error("Starting level", e.what());
					throw e;
				}
			}
			else if (baseIndex.extract("Menu \"`S\"`L", &levelData.name)) {
				try {
					mCurrentMenu = new Menu(NULL, mCommonLibraries.menus.get(levelData.name));
				}
				catch (std::exception) {
					throw std::exception((std::string("Starting menu has not been defined: ") + levelData.name).data());
				}
			}
			else {
				throw std::exception("Malformed start location.");
			}
		}
		else if (baseIndex.extract("resolution: [`I, `I]`L", &resolution)) {
			resize(resolution.w, resolution.h);
		}
		else if (baseIndex.extract("title: \"`S\"`L", &levelData.name)) {
			mWindow->rename(levelData.name);
		}
		else if (baseIndex.extract("common: \"`S\"`L", &commonFolder)) {
			std::string commonPath = mAssetBasePath + commonFolder;
			FileService commonIndex(commonPath + mIndexFilename);
			if (!commonIndex.good()) throw std::exception("Common asset file could not be opened.");
			while (commonIndex.good()) {
				auto_cstr workingDirectory;
				try {
					if (commonIndex.extract("//`S`L"));
					else if (commonIndex.extract("menus: \"`S\"`L", &workingDirectory));
						//parseMenuIndex(commonPath + workingDirectory, mCommonLibraries.menus);
					else if (commonIndex.extract("obj: \"`S\"`L", &workingDirectory))
						// Open the index file in the new path, then use it to load objs into common obj directory
						parseGeometryIndex(commonPath + workingDirectory, mCommonLibraries.standard.geometries);
					else if (commonIndex.extract("sound: \"`S\"`L", &workingDirectory));
						// Open the index file in the new path, then use it to load sounds into common sound directory
						//parseSoundIndex(commonPath + workingDirectory, mCommonLibraries.standard.sounds);
					else if (commonIndex.extract("tex: \"`S\"`L", &workingDirectory))
						// Open the index file in the new path, then use it to load textures into common texture directory
						parseTextureIndex(commonPath + workingDirectory, mCommonLibraries.standard.textures);
					else if (commonIndex.extract("glsl: \"`S\"`L", &workingDirectory))
						// Open the index file in the new path, then use it to load shaders into common shader directory
						parseShaderIndex(commonPath + workingDirectory, mCommonLibraries.standard.shaders, mCommonLibraries.standard.programs);
					else if (commonIndex.extract("post: \"`S\"`L", &workingDirectory))
						// Open the index file in the new path, then use it to load postprocessing things into common postprocessing directory
						parsePostprocessingIndex(commonPath + workingDirectory, mCommonLibraries.post.shaders, mCommonLibraries.post.filters, mCommonLibraries.post.kernels, mCommonLibraries.post.pipelines);
					else if (commonIndex.extract("`S`L", &workingDirectory))
						ServiceLocator::getLoggingService().error("Unexpected line in common index file", workingDirectory);
				}
				catch (std::exception e) {
					ServiceLocator::getLoggingService().badFileError(e.what());
				}
			}
		}
		else if (baseIndex.extract("ppp: \"`S\"`L", &ppp)) {
			try {
				mCurrentPostProcessing = mCommonLibraries.post.pipelines.get(ppp);
			}
			catch (std::out_of_range) {
				ServiceLocator::getLoggingService().error("Not a valid default postprocessor", ppp);
				mCurrentPostProcessing = nullptr;
			}
		}
		else if (baseIndex.extract("`S`L", &levelData.name))
			ServiceLocator::getLoggingService().error("Unexpected line in root index file", levelData.name);
	}
	softReload();
}

Level* Game::loadLevel(std::string path) {
	StandardLibraries* levelLibraries = new StandardLibraries();
	FileService index(path + mIndexFilename);
	// Load any additional data into the new libraries
	// Level index files can add new geometry, shaders, programs, textures, and sounds.
	auto_cstr levelFile;
	index.extract("Level: \"`S\"`L", &levelFile);
	while (index.good()) {
		auto_cstr err, workingDirectory;
		try {
			if (index.extract("//`S`L"));
			/*else if (index.extract("menus: \"`S\"`L", &workingDirectory))
				parseMenuIndex(path + workingDirectory, mCommonLibraries.menus);*/
			else if (index.extract("obj: \"`S\"`L", &workingDirectory))
				parseGeometryIndex(path + workingDirectory, levelLibraries->geometries);
			else if (index.extract("sound: \"`S\"`L", &workingDirectory))
				parseSoundIndex(path + workingDirectory, levelLibraries->sounds);
			else if (index.extract("tex: \"`S\"`L", &workingDirectory))
				parseTextureIndex(path + workingDirectory, levelLibraries->textures);
			else if (index.extract("glsl: \"`S\"`L", &workingDirectory))
				parseShaderIndex(path + workingDirectory, levelLibraries->shaders, levelLibraries->programs);
			/*else if (index.extract("post: \"`S\"`L", &workingDirectory))
				parsePostprocessingIndex(path + workingDirectory, postShaderLibrary, postFilterLibrary, postKernelLibrary, postPipelineLibrary);*/
			else if (index.extract("`S`L", &err))
				ServiceLocator::getLoggingService().error("Unexpected line in level index file", err);
		}
		catch (std::exception e) {
			ServiceLocator::getLoggingService().badFileError(e.what());
		}
	}
	return new Level(path + levelFile, mCommonLibraries.standard, *levelLibraries);
}

void Game::parseMenuIndex(std::string path, NamedContainer<RootElement*> &menuLibrary) {
	FileService menuIndex(path + mIndexFilename);
	if (!menuIndex.good()) throw std::exception(menuIndex.getPath().data());
	while (menuIndex.good()) {
		struct { auto_cstr name, path; } menuData;
		if (menuIndex.extract("//`S`L"));
		else if (menuIndex.extract("\"`S\":\"`S\"`L", &menuData)) {
			try {
				menuLibrary.add(menuData.name, new RootElement(path + menuData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + menuData.path);
			}
		}
		else if (menuIndex.extract("`S`L", &menuData.name))
			ServiceLocator::getLoggingService().error("Unexpected line in menu index file", menuData.name);
	}
}

void Game::parseGeometryIndex(std::string path, NamedContainer<GeometryComponent*> &geomLibrary) {
	FileService objIndex(path + mIndexFilename);
	if (!objIndex.good()) throw std::exception(objIndex.getPath().data());
	while (objIndex.good()) {
		struct { auto_cstr name, path; } objData;
		if (objIndex.extract("//`S`L"));
		else if (objIndex.extract("\"`S\":\"`S\"`L", &objData)) {
			try {
				geomLibrary.add(objData.name, new GeometryComponent(path + objData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + objData.path);
			}
		}
		else if (objIndex.extract("`S`L", &objData.name))
			ServiceLocator::getLoggingService().error("Unexpected line in obj index file", objData.name);
	}
}

void Game::parseSoundIndex(std::string path, SoundLibrary &soundLibrary) {
	FileService soundIndex(path + mIndexFilename);
	if (!soundIndex.good()) throw std::exception(soundIndex.getPath().data());
	while (soundIndex.good()) {
		struct { auto_cstr name, path; } soundData;
		if (soundIndex.extract("//`S`L"));
		else if (soundIndex.extract("Procedural \"`S\" \"`S\"`L", &soundData)) {
			try {
				soundLibrary.add(soundData.name, new ProceduralSound(path + soundData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + soundData.path);
			}
		}
		else if (soundIndex.extract("File \"`S\" \"`S\"`L", &soundData)) {
			try {
				soundLibrary.add(soundData.name, new FileSound(path + soundData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + soundData.path);
			}
		}
		else if (soundIndex.extract("`S`L", &soundData.name))
			ServiceLocator::getLoggingService().error("Unexpected line in sound index file", soundData.name);
	}
}

void Game::parseTextureIndex(std::string path, NamedContainer<Texture*> &texLibrary) {
	FileService texIndex(path + mIndexFilename);
	if (!texIndex.good()) throw std::exception(texIndex.getPath().data());
	while (texIndex.good()) {
		struct { auto_cstr name, path; } texData;
		if (texIndex.extract("//`S`L"));
		else if (texIndex.extract("\"`S\":\"`S\"`L", &texData)) {
			try {
				texLibrary.add(texData.name, new Texture(path + texData.path));
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + texData.path);
			}
		}
		else if (texIndex.extract("`S`L", &texData.name))
			ServiceLocator::getLoggingService().error("Unexpected line in texture index file", texData.name);
	}
}

#include "GL\glew.h"	// I literally just need this for GL_VERTEX_SHADER, which is a bit annoying

void Game::parseShaderIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &progLibrary) {
	FileService glslIndex(path + mIndexFilename);
	if (!glslIndex.good()) throw FileException(glslIndex.getPath().data());
	int type;
	while (glslIndex.good()) {
		struct { auto_cstr name; int version; auto_cstr path, extra; } glslData;
		struct { auto_cstr name; } progData;
		if (glslIndex.extract("//`S`L")) type = NULL;
		else if (glslIndex.extract("Vertex ")) type = GL_VERTEX_SHADER;
		else if (glslIndex.extract("Tessellation Control ")) type = GL_TESS_CONTROL_SHADER;
		else if (glslIndex.extract("Tessellation Evaluation ")) type = GL_TESS_EVALUATION_SHADER;
		else if (glslIndex.extract("Geometry ")) type = GL_GEOMETRY_SHADER;
		else if (glslIndex.extract("Fragment ")) type = GL_FRAGMENT_SHADER;
		else if (glslIndex.extract("Compute ")) type = GL_COMPUTE_SHADER;
		else if (glslIndex.extract("Program \"`S\"", &progData.name)) {
			type = NULL;
			std::vector<auto_cstr> vert, tesc, tese, geom, frag, comp;	// These seem to be leaking
			while (!glslIndex.extract("`L")) {
				auto_cstr temp;
				if (glslIndex.extract(" Vertex:\"`S\"", &temp)) { vert.push_back(temp); }
				else if (glslIndex.extract(" Geometry:\"`S\"", &temp)) { geom.push_back(temp); }
				else if (glslIndex.extract(" Tessellation Control:\"`S\"", &temp)) { tesc.push_back(temp); }
				else if (glslIndex.extract(" Tessellation Evaluation:\"`S\"", &temp)) { tese.push_back(temp); }
				else if (glslIndex.extract(" Fragment:\"`S\"", &temp)) { frag.push_back(temp); }
				else if (glslIndex.extract(" Compute:\"`S\"", &temp)) { comp.push_back(temp); }
				else if (glslIndex.extract(" `S:\"", &temp)) {
					auto_cstr temp2;
					glslIndex.extract("`S\"", &temp2);
					ServiceLocator::getLoggingService().error("Unknown shader type", temp);
					ServiceLocator::getLoggingService().error(" ... with name", temp2);
				}
				else if (glslIndex.extract("`S`L", &temp)) {
					ServiceLocator::getLoggingService().error("Unexpected data in program definition", temp);
					break;
				}
			}
			Program* prog = new Program();
			for (auto& i : vert) {
				try { prog->attach(shaderLibrary.get(i), GL_VERTEX_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Vertex shader named but not present", i); }
			}
			for (auto& i : tesc) {
				try { prog->attach(shaderLibrary.get(i), GL_TESS_CONTROL_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Tessellation Control shader named but not present", i); }
			}
			for (auto& i : tese) {
				try { prog->attach(shaderLibrary.get(i), GL_TESS_EVALUATION_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Tessellation Evaluation shader named but not present", i); }
			}
			for (auto& i : geom) {
				try { prog->attach(shaderLibrary.get(i), GL_GEOMETRY_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Geometry shader named but not present", i); }
			}
			for (auto& i : frag) {
				try { prog->attach(shaderLibrary.get(i), GL_FRAGMENT_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Fragment shader named but not present", i); }
			}
			for (auto& i : comp) {
				try { prog->attach(shaderLibrary.get(i), GL_COMPUTE_SHADER); }
				catch (std::out_of_range) { ServiceLocator::getLoggingService().error("Compute shader named but not present", i); }
			}
			if (prog->link() && prog->validate()) {
				progLibrary.add(progData.name, prog);
				prog->detachAll();
			}
			else {
				prog->removeAll(GL_COMPUTE_SHADER);
				if (prog->link(false) && prog->validate(false)) {
					prog->detachAll();
					ServiceLocator::getLoggingService().log(std::string("Recovered by removing compute shaders. ") + progData.name + " may not function as expected.");
					progLibrary.add(progData.name, prog);
				}
				else {
					prog->removeAll(GL_TESS_CONTROL_SHADER);
					prog->removeAll(GL_TESS_EVALUATION_SHADER);
					if (prog->link(false) && prog->validate(false)) {
						prog->detachAll();
						ServiceLocator::getLoggingService().log(std::string("Recovered by removing tesselation shaders. ") + progData.name + " may not function as expected.");
						progLibrary.add(progData.name, prog);
					}
					else {
						prog->removeAll(GL_GEOMETRY_SHADER);
						if (prog->link(false) && prog->validate(false)) {
							prog->detachAll();
							ServiceLocator::getLoggingService().log(std::string("Recovered by removing geometry shaders. ") + progData.name + " may not function as expected.");
							progLibrary.add(progData.name, prog);
						}
						else {
							ServiceLocator::getLoggingService().log(std::string("Could not recover ") + progData.name + ".");
							delete prog;
						}
					}
				}
			}
		}
		else if (glslIndex.extract("`S`L", &glslData.name)) {
			type = NULL;
			ServiceLocator::getLoggingService().error("Unexpected line in glsl index file", glslData.name);
		}
		if (type && glslIndex.extract("\"`S\" `I:\"`S\"`?S`L", &glslData)) {
			try {
				Shader* temp = new Shader(path + glslData.path, type);
				try {
					shaderLibrary.get(glslData.name);
					ServiceLocator::getLoggingService().error("Attempted redefintion of shader", glslData.name);
				}
				catch (std::out_of_range e) {
					shaderLibrary.add(glslData.name, glslData.version, temp);
				}
				if (glslData.extra) ServiceLocator::getLoggingService().error("Extra data at end of shader definition", glslData.extra);
			}
			catch (...) {
				ServiceLocator::getLoggingService().badFileError(path + glslData.path);
			}
		}
	}
}

void Game::parsePostprocessingIndex(std::string path, ShaderManager &shaderLibrary, NamedContainer<Program*> &filterLibrary, KernelManager &kernelLibrary, NamedContainer<PostprocessingPipeline*> &pipelineLibrary) {
	FileService postIndex(path + mIndexFilename);
	if (!postIndex.good()) throw std::exception(postIndex.getPath().data());
	while (postIndex.good()) {
		struct { auto_cstr name; int samples; auto_cstr path, extra; } shaderData;
		struct { auto_cstr name; Kernel kernel; } kernelData;
		struct { auto_cstr name, sampler, processor; int in = 0, out = 0; } filterData;
		struct { auto_cstr name; } pipelineData;
		if (postIndex.extract("//`S`L")) {}
		else if (postIndex.extract("Sampler \"`S\" `I \"`S\"", &shaderData)) {
			postIndex.extract("`?S`L", &shaderData.extra);
			if (shaderData.extra)
				ServiceLocator::getLoggingService().error("Extra data at end of shader definition", shaderData.extra);
			shaderLibrary.add(shaderData.name, shaderData.samples, new Shader(path + shaderData.path, GL_VERTEX_SHADER));
		}
		else if (postIndex.extract("Processor \"`S\" `I \"`S\"", &shaderData)) {
			postIndex.extract("`?S`L", &shaderData.extra);
			if (shaderData.extra)
				ServiceLocator::getLoggingService().error("Extra data at end of shader definition", shaderData.extra);
			shaderLibrary.add(shaderData.name, shaderData.samples, new Shader(path + shaderData.path, GL_FRAGMENT_SHADER));
		}
		else if (postIndex.extract("Kernel \"`S\" `I [", &kernelData)) {
			kernelData.kernel.weights = new float[kernelData.kernel.samples];
			bool valid = true;
			for (int i = 0; i < kernelData.kernel.samples; i++) {
				if (!postIndex.extract("`F", kernelData.kernel.weights + i)) {
					valid = false;
					break;
				}
				else if (postIndex.extract(", ") || postIndex.extract("]"));
				else {
					valid = false;
					break;
				}
			}
			if (valid && postIndex.extract("`L")) {
				kernelLibrary.add(kernelData.name, kernelData.kernel);
				// Can't delete value array because kernel owns it now
				continue;
			}
			else {
				auto_cstr err;
				postIndex.extract("`S`L", &err);
				ServiceLocator::getLoggingService().error("Unexpected line in kernel definition", err);
			}
		}
		else if (postIndex.extract("Filter \"`S\" Sampler:\"`S\" Processor:\"`S\" in:`I out:`I`L", &filterData)) {
			// Do error checking, then construct the filter program
			Shader* sampler = nullptr, *processor = nullptr;
			bool valid = true;
			try { sampler = shaderLibrary.get(filterData.sampler); }
			catch (std::exception e) { ServiceLocator::getLoggingService().error("Sampler not found", e.what()); valid = false; }
			try { processor = shaderLibrary.get(filterData.processor); }
			catch (std::exception e) { ServiceLocator::getLoggingService().error("Processor not found", e.what()); valid = false; }
			if (filterData.in < 0) { ServiceLocator::getLoggingService().error("Invalid number of input samples", std::to_string(filterData.in)); valid = false; };
			if (filterData.out == 0) { ServiceLocator::getLoggingService().error("Invalid number of output samples", std::to_string(filterData.out)); valid = false; };
			if (valid) {
				Program* filter = new Program(sampler, processor, filterData.in, filterData.out);
				// There should be a smarter way to set these, maybe by listing them all out in the index file
				filter->sendUniform("uColorBuffer", 0);
				filter->sendUniform("uDepthBuffer", 1);
				for (int i = 0; i < 10; i++) {
					std::string name("uTexture");
					name += std::to_string(i);
					filter->sendUniform(name.c_str(), i);
				}
				filter->sendUniform("uTexture", 0);
				filterLibrary.add(filterData.name, filter);
			}
		}
		else if (postIndex.extract("Pipeline \"`S\" [", &pipelineData)) {
			PostprocessingPipeline* temp = new PostprocessingPipeline(mWindow->getWidth(), mWindow->getHeight());
			bool valid = true;
			while (true) {
				// Read
				struct { auto_cstr name; float scale = 1.0f; auto_cstr kernel; } stageData;
				bool stageValid = true, compositingInput = false;
				std::vector<auto_cstr> kernels;
				if (postIndex.extract("\"`S\"", &stageData.name)) {
					compositingInput = postIndex.extract("*");
					if (postIndex.extract(":`F", &stageData.scale));
					if (postIndex.extract(":\"`S\"", &stageData.kernel));
					else if (postIndex.extract(":[")) {
						kernels.push_back(nullptr);
						while (!postIndex.extract("]")) {
							postIndex.extract("\"`S\"", &kernels.back());
							kernels.push_back(nullptr);
							postIndex.extract(",");
						}
						kernels.pop_back();
					}
					// Verify
					StageData stage;
					try {
						stage.filter = filterLibrary.get(stageData.name);
					}
					catch (std::invalid_argument e) {
						stageValid = false;
						ServiceLocator::getLoggingService().error("Not a valid filter", e.what());
					}
					stage.scale = stageData.scale;
					if (stageData.kernel) {
						try {
							stage.kernels.push_back(kernelLibrary.get(stageData.kernel));
						}
						catch (std::invalid_argument e) {
							stageValid = false;
							ServiceLocator::getLoggingService().error("Not a valid kernel", e.what());
						}
					}
					else if (kernels.size()) {
						for (auto kernel : kernels) {
							try {
								stage.kernels.push_back(kernelLibrary.get(kernel));
							}
							catch (std::invalid_argument e) {
								stageValid = false;
								ServiceLocator::getLoggingService().error("Not a valid kernel", e.what());
							}
						}
					}
					// Add
					if (stageValid) {
						try { temp->attach(stage, compositingInput); }
						catch (std::invalid_argument e) {
							ServiceLocator::getLoggingService().error(stageData.name, e.what());
							valid = false;
							delete temp;
						}
					}
					else valid = false;
				}
				else if (postIndex.extract("compositor:\"`S\"", &stageData.name)) {
					try {
						Program* compositor = filterLibrary.get(stageData.name);
						temp->attachCompositor(compositor);
					}
					catch (std::invalid_argument e) {
						ServiceLocator::getLoggingService().error("Not a valid compositing stage", e.what());
					}
				}
				if (postIndex.extract(", ")) continue;
				if (postIndex.extract("]")) break;
			}
			// Verify
			if (valid)
				pipelineLibrary.add(pipelineData.name, temp);
			else
				delete temp;
			// Cleanup
			auto_cstr err;
			postIndex.extract("`?S`L", &err);
			if (err)
				ServiceLocator::getLoggingService().error("Extra data at end of pipeline definition", err);
		}
		else if (postIndex.extract("`?S`L", &pipelineData.name)) {
			if (pipelineData.name)
				ServiceLocator::getLoggingService().error("Unexpected line in postprocessing index file", pipelineData.name);
		}
	}
}