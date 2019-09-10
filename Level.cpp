#include "Level.h"
#include "Bounds.h"
#include "Camera.h"
#include "ServiceLocator.h"
#include "Source.h"
#include "State.h"
#include "glm/gtx/transform.hpp"
#include <xtree>

Level::Level(std::string filepath, StandardLibraries& sharedLibraries, StandardLibraries& ownLibraries) :mSharedLibraries(sharedLibraries), mOwnLibraries(ownLibraries) {
	FileService file(filepath);
	while (file.good()) {
		auto_cstr objectName;
		if (file.extract("//`S`L"));
		else if (file.extract("`?WObject \"`S\"", &objectName)) {
			if (mSceneGraph.contains(objectName)) {
				ServiceLocator::getLoggingService().error("Attempted redefinition of object", objectName);
				file.extract("`S`L");
				continue;
			}
			// Set up variables and structs to read into
			struct { float x, y, z; } pos{ 0.0f,0.0f,0.0f }, vel{ 0.0f,0.0f,0.0f }, acc{ 0.0f,0.0f,0.0f }, rot{ 0.0f,0.0f,1.0f }, scl{ 1.0f,1.0f,1.0f }, axis{ 0.0f,0.0f,1.0f }, col{ 0.0f,0.0f,0.0f };
			float ang = 0.0f, mom = 0.0f;
			Geometry* geom = nullptr;
			Program* program = nullptr;
			Texture* texture = nullptr;
			std::vector<std::string> sounds;
			bool valid = true, hasCol = false;
			std::string parent, input, bound;
			while (!file.extract("`L")) {
				auto_cstr parentName, geomName, progName, texName, inputName, soundName, boundsName, err;
				if (file.extract(" parent:\"`S\"", &parentName)) {
					if (!mSceneGraph.contains(parentName)) {
						ServiceLocator::getLoggingService().error("Parent object not found", parentName);
						valid = false;
						file.extract("`?S`L");
						break;
					}
					else {
						parent = (char*)parentName;
					}
				}
				else if (file.extract(" bounds:\"`S\"", &boundsName)) { bound = (char*)boundsName; }
				else if (file.extract(" geom:\"`S\"", &geomName)) {
					// Check if the geometry exists in the geometry library
					// If so, save a reference to it to pass to the GameObject constructor
					// If not, raise an error and break, set a bool so we can skip the construction of the object, and eat the rest of the line
					// (Same general procedure applies to all parameters)
					try {
						try {
							geom = mOwnLibraries.geometries.get(geomName);
						}
						catch (std::out_of_range) {
							geom = mSharedLibraries.geometries.get(geomName);
						}
					}
					catch (std::out_of_range e) {
						ServiceLocator::getLoggingService().error("Geometry not found", e.what());
						valid = false;
						file.extract("`?S`L");
						break;
					}
				}
				else if (file.extract(" prog:\"`S\"", &progName)) {
					try {
						try {
							program = mOwnLibraries.programs.get(progName);
						}
						catch (std::out_of_range) {
							program = mSharedLibraries.programs.get(progName);
						}
					}
					catch (std::out_of_range e) {
						ServiceLocator::getLoggingService().error("Program not found", e.what());
						valid = false;
						file.extract("`?S`L");
						break;
					}
				}
				else if (file.extract(" tex:\"`S\"", &texName)) {
					try {
						try {
							texture = mOwnLibraries.textures.get(texName);
						}
						catch (std::out_of_range) {
							texture = mSharedLibraries.textures.get(texName);
						}
					}
					catch (std::out_of_range e) {
						ServiceLocator::getLoggingService().error("Texture not found", e.what());
						valid = false;
						file.extract("`?S`L");
						break;
					}
				}
				else if (file.extract(" col:(`F,`F,`F)", &col)) { hasCol = true; }
				else if (file.extract(" sounds:(")) {
					while (!file.extract(")")){
						auto_cstr soundName;
						file.extract("\"`S\"", &soundName);
						sounds.push_back(soundName);
						file.extract(",");	// No checking to make sure it's properly formatted
					}
				}
				else if (file.extract(" pos:(`F,`F,`F)", &pos));
				else if (file.extract(" vel:(`F,`F,`F)", &vel));
				else if (file.extract(" acc:(`F,`F,`F)", &acc));
				else if (file.extract(" rot:(`F,`F,`F)", &rot));
				else if (file.extract(" ang:`F", &ang));
				else if (file.extract(" axis:(`F,`F,`F)", &axis));
				else if (file.extract(" mom:`F", &mom));
				else if (file.extract(" scl:`F", &scl.x)) { scl.z = scl.y = scl.x; }
				else if (file.extract(" scl:(`F,`F,`F)", &scl));
				else if (file.extract(" input:\"`S\"", &inputName)) { input = (char*)inputName; }
				else if (file.extract("`S:", &err)) {
					ServiceLocator::getLoggingService().error("Unexpected keyword in line", err);
					auto_cstr err2;
					if (file.extract("`S ", &err2)) {
						ServiceLocator::getLoggingService().error("Skipping parameter", err2);
						file.putBack(" ");	// To continue with proper parsing
					}
				}
				else if (file.extract("`?S`L", &err)) {
					if (err)
						ServiceLocator::getLoggingService().error("Malformed keyword or extra data in line (skipping line)", err);
					file.putBack("\n");	// Not really sure how I should handle this with the new `L, but this works fine for now
					valid = false;
					break;
				}
			}
			if (valid) {
				// Construction of the actual object
				Bounds* bounds = nullptr;
				if (!bound.empty()) {
					if (bound == "Sphere")
						bounds = new BoundingSphere({ 0.0f,0.0f,0.0f }, 1.0f);
					else if (bound == "AABB")
						bounds = geom->getBoundingBox()->copy();
					else if (bound == "Plane") {
						glm::mat4 rotation = glm::rotate(ang, glm::vec3(rot.x, rot.y, rot.z));
						auto n = rotation * glm::vec4(0.0, 0.0, 1.0, 0.0);
						bounds = new CollisionPlane(glm::vec3(pos.x, pos.y, pos.z), glm::vec3(n.x, n.y, n.z));
					}
					else
						ServiceLocator::getLoggingService().error("Unknown bounding box type", bound);
				}
				PhysicsComponent* physics = new PhysicsComponent(bounds, { vel.x,vel.y,vel.z }, { acc.x,acc.y,acc.z }, { axis.x, axis.y, axis.z }, mom);
				physics->scale({ scl.x,scl.y,scl.z });
				physics->rotate({ rot.x,rot.y,rot.z }, ang);
				physics->translate({ pos.x,pos.y,pos.z });
				GameObject* object = new GameObject(geom, program, physics);
				if (texture) object->setTexture(texture);
				if (hasCol) object->setColor({ col.x,col.y,col.z });
				if (!input.empty()) {
					try {
						State* state = State::getNewEntryState(input, object);
						object->setState(state);
					}
					catch (std::out_of_range e) {
						ServiceLocator::getLoggingService().error("Cannot find AI state", input);
						// Cleanup
						delete object;
						continue;
					}
				}
				else {
					State* state = State::getNewEntryState("passthru", object);	// Could probably move this into State.h actually, so there's always one valid one just in case
					object->setState(state);
				}
				for (auto sound : sounds) {
					if (mOwnLibraries.sounds.get(sound))
						object->registerSound(sound, mOwnLibraries.sounds.get(sound));
					else if (mSharedLibraries.sounds.get(sound))
						object->registerSound(sound, mSharedLibraries.sounds.get(sound));
					else
						ServiceLocator::getLoggingService().error("Sound not found", sound);
				}
				if (!parent.empty()) mSceneGraph.add(objectName, parent, object);
				else mSceneGraph.add(objectName, object);
			}
		}
		else if (file.extract("Camera \"`S\"", &objectName)) {
			if (mSceneCameras.count(objectName)) {
				ServiceLocator::getLoggingService().error("Attempted redefinition of camera", objectName);
				file.extract("`S`L");
				continue;
			}
			struct { float x, y, z; } pos{ 0.0f,0.0f,0.0f }, at{ 0.0f,1.0f,0.0f };
			float fov = 75.0;
			while (!file.extract("`L")) {
				auto_cstr err, err2;
				if (file.extract(" pos:(`F,`F,`F)", &pos));
				else if (file.extract(" at:(`F,`F,`F)", &at));
				else if (file.extract(" fov:`F", &fov));
				else if (file.extract("`S:", &err)) {
					ServiceLocator::getLoggingService().warning("Unexpected keyword in line", err);
					if (file.extract("`S ", &err2)) {
						ServiceLocator::getLoggingService().warning("Skipping parameter", err2);
						file.putBack(" ");	// To continue with proper parsing
					}
				}
				else if (file.extract("`?S`L", &err)) {
					if (err)
						ServiceLocator::getLoggingService().error("Malformed keyword or extra data in line", err);
					file.putBack("\n");
					break;
				}
			}
			PhysicsComponent* phys = new PhysicsComponent({ pos.x,pos.y,pos.z }, { at.x,at.y,at.z });
			PerspCamera* camera = new PerspCamera(800, 600, fov);	// How do I get these values in here?
			camera->update(phys);
			GameObject* camera_object = new GameObject(nullptr, nullptr, phys);
			camera_object->setCameraComponent(camera);
			try {
				State* state = State::getNewEntryState("camera", camera_object);
				camera_object->setState(state);	// It just occured to me that State could probably do this
			}
			catch (std::out_of_range e) {
				ServiceLocator::getLoggingService().error("Cannot find AI state", e.what());
				delete camera_object;
				delete camera;
				// Does GameObject take care of its PhysicsComponent? Does Camera? Uh oh
				continue;
			}
			mSceneCameras[objectName] = camera_object;
		}
		else if (file.extract("Music \"`S\"", &objectName)) {
			if (mBackgroundMusic) {
				ServiceLocator::getLoggingService().error("Attempted redefinition of background music", objectName);
			}
			else try {
				try {
					mBackgroundMusic = mOwnLibraries.sounds.get(objectName);
				}
				catch (std::out_of_range) {
					mBackgroundMusic = mSharedLibraries.sounds.get(objectName);
				}
			}
			catch (std::out_of_range &e) {
				ServiceLocator::getLoggingService().error("Background music not found", e.what());
			}
		}
		else if (file.extract("`?S`L", &objectName)) {
			if (objectName)
				ServiceLocator::getLoggingService().error("Unrecognized line in level file", objectName);
		}
	}
	if (mSceneCameras.count("main"))
		mCurrentCamera = mSceneCameras.at("main");
	else
		ServiceLocator::getLoggingService().error("No camera \"main\" defined in level", filepath);
	if (mBackgroundMusic) {
		mSceneAudio = new Source(mCurrentCamera->getPhysicsComponent(), true);
		mSceneAudio->update();
		mSceneAudio->playSound(mBackgroundMusic);
	}
}

Level::~Level() {
	for (auto camera : mSceneCameras)
		delete camera.second;
	mSceneCameras.clear();
	mSceneGraph.clear();
	mOwnLibraries.geometries.clear();
	mOwnLibraries.shaders.clear();
	mOwnLibraries.programs.clear();
	mOwnLibraries.textures.clear();
	mOwnLibraries.sounds.clear();
	delete &mOwnLibraries;	// Haha that's super dangerous
	if (mSceneAudio) delete mSceneAudio;
}

GameObject* Level::getWorkingSet() {
	GameObject* workingSet = new GameObject[mSceneGraph.count()];
	std::vector<std::string> objectNames = mSceneGraph.breadthFirstTraversal();
	std::map<std::string, PhysicsComponent*> clonedPCs;
	int i = 0;
	for (std::string name : objectNames) {
		mSceneGraph.get(name)->copy(workingSet + i);
		if (mSceneGraph.hasParent(name))
			workingSet[i].getPhysicsComponent()->setParent(clonedPCs[mSceneGraph.getParent(name)]);
		clonedPCs[name] = workingSet[i].getPhysicsComponent();
		i++;
	}
	return workingSet;
}

void Level::setBackgroundMusicVolume(float volume) {
	mSceneAudio->setVolume(volume);
}