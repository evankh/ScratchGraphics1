#include "Level.h"
#include "Bounds.h"
#include "Camera.h"
#include "ServiceLocator.h"
#include "Source.h"
#include "State.h"
#include "glm/gtx/transform.hpp"

Level::Level(std::string filepath, StandardLibraries& sharedLibraries, StandardLibraries ownLibraries) :mSharedLibraries(sharedLibraries), mOwnLibraries(ownLibraries) {
	FileService file(filepath);
	char* objectName;
	while (file.good()) {
		if (file.extract("//`S`L", NULL));
		else if (file.extract("Object \"`S\"", &objectName)) {
			if (mSceneObjects.count(objectName)) {
				ServiceLocator::getLoggingService().error("Attempted redefinition of object", objectName);
				delete[] objectName;
				file.extract("`S`L", NULL);
				continue;
			}
			// Set up variables and structs to read into
			char* geomName, *progName, *texName, *inputName = NULL, *soundName, *boundsName = NULL, *err;	// Not sure why it only cares about inputName
			struct { float x, y, z; } pos{ 0.0f,0.0f,0.0f }, vel{ 0.0f,0.0f,0.0f }, acc{ 0.0f,0.0f,0.0f }, rot{ 0.0f,0.0f,1.0f }, scl{ 1.0f,1.0f,1.0f }, axis{ 0.0f,0.0f,1.0f }, col{ 0.0f,0.0f,0.0f };
			float ang = 0.0f, mom = 0.0f;
			Geometry* geom = NULL;
			Program* program = NULL;
			Bounds* bounds = NULL;
			Texture* texture = NULL;
			std::vector<std::string> sounds;
			bool valid = true, hasCol = false;
			while (!file.extract("`L", NULL)) {
				if (file.extract(" bounds:\"`S\"", &boundsName));
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
						delete[] geomName;
					}
					catch (std::out_of_range e) {
						ServiceLocator::getLoggingService().error("Geometry not found", e.what());
						delete[] geomName;
						valid = false;
						file.extract("`?S`L", NULL);
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
						delete[] progName;
					}
					catch (std::out_of_range e) {
						ServiceLocator::getLoggingService().error("Program not found", e.what());
						delete[] progName;
						valid = false;
						file.extract("`?S`L", NULL);
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
						delete[] texName;
					}
					catch (std::out_of_range e) {
						ServiceLocator::getLoggingService().error("Texture not found", e.what());
						delete[] texName;
						valid = false;
						file.extract("`?S`L", NULL);
						break;
					}
				}
				else if (file.extract(" col:(`F,`F,`F)", &col)) { hasCol = true; }
				else if (file.extract(" sounds:(", NULL)) {
					while (file.extract("\"`S\"", &soundName)) {
						sounds.push_back(soundName);
						delete[] soundName;
						file.extract(",", NULL);	// No checking to make sure it's properly formatted
					}
					file.extract(")", NULL);
				}
				else if (file.extract(" pos:(`F,`F,`F)", &pos));
				else if (file.extract(" vel:(`F,`F,`F)", &vel));
				else if (file.extract(" acc:(`F,`F,`F)", &acc));
				else if (file.extract(" rot:(`F,`F,`F)", &rot));
				else if (file.extract(" ang:`F", &ang));
				else if (file.extract(" axis:(`F,`F,`F)", &axis));
				else if (file.extract(" mom:`F", &mom));
				else if (file.extract(" scl:`F", &scl.x)) {
					scl.z = scl.y = scl.x;
				}
				else if (file.extract(" scl:(`F,`F,`F)", &scl));
				else if (file.extract(" input:\"`S\"", &inputName));
				else if (file.extract("`S:", &err)) {
					ServiceLocator::getLoggingService().error("Unexpected keyword in line", err);
					delete[] err;
					if (file.extract("`S ", &err)) {
						ServiceLocator::getLoggingService().error("Skipping parameter", err);
						delete[] err;
						file.putBack(" ");	// To continue with proper parsing
					}
				}
				else if (file.extract("`?S`L", &err)) {
					if (err) {
						ServiceLocator::getLoggingService().error("Malformed keyword or extra data in line (skipping line)", err);
						delete[] err;
					}
					file.putBack("\n");	// Not really sure how I should handle this with the new `L, but this works fine for now
					valid = false;
					break;
				}
			}
			if (valid) {
				// Construction of the actual object
				// It actually looks safe for any of the components to be NULL so the validity checking may not be necessary
				if (boundsName) {
					if (strcmp(boundsName, "Sphere") == 0)
						bounds = new BoundingSphere({ 0.0f,0.0f,0.0f }, 1.0f);
					else if (strcmp(boundsName, "AABB") == 0)
						bounds = geom->getBoundingBox()->copy();
					else if (strcmp(boundsName, "Plane") == 0) {
						glm::mat4 rotation = glm::rotate(ang, glm::vec3(rot.x, rot.y, rot.z));
						auto n = rotation * glm::vec4(0.0, 0.0, 1.0, 0.0);
						bounds = new CollisionPlane(glm::vec3(pos.x, pos.y, pos.z), glm::vec3(n.x, n.y, n.z));
					}
					else
						ServiceLocator::getLoggingService().error("Unknown bounding box type", boundsName);
				}
				PhysicsComponent* physics = new PhysicsComponent(bounds, { vel.x,vel.y,vel.z }, { acc.x,acc.y,acc.z }, { axis.x, axis.y, axis.z }, mom);
				physics->scale({ scl.x,scl.y,scl.z });
				physics->rotate({ rot.x,rot.y,rot.z }, ang);
				physics->translate({ pos.x,pos.y,pos.z });
				glm::vec3* color = NULL;
				if (hasCol) color = new glm::vec3(col.x, col.y, col.z);
				// Also store velocity
				GameObject* object = new GameObject(geom, program, physics, texture);
				if (inputName) try {
					State* state = State::getNewEntryState(inputName, object);
					object->setState(state);
				}
				catch (std::out_of_range e) {
					ServiceLocator::getLoggingService().error("Cannot find AI state", inputName);
					// Cleanup
					delete object;
					continue;
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
				mSceneObjects[objectName] = object;
			}
			// Cleanup
			delete[] objectName;
		}
		else if (file.extract("Camera \"`S\"", &objectName)) {
			if (mSceneCameras.count(objectName)) {
				ServiceLocator::getLoggingService().error("Attempted redefinition of camera", objectName);
				delete[] objectName;
				file.extract("`S`L", NULL);
				continue;
			}
			struct { float x, y, z; } pos{ 0.0f,0.0f,0.0f }, at{ 0.0f,1.0f,0.0f };
			float fov = 75.0;
			char* err;
			while (!file.extract("`L", NULL)) {
				if (file.extract(" pos:(`F,`F,`F)", &pos));
				else if (file.extract(" at:(`F,`F,`F)", &at));
				else if (file.extract(" fov:`F", &fov));
				else if (file.extract("`S:", &err)) {
					ServiceLocator::getLoggingService().warning("Unexpected keyword in line", err);
					delete[] err;
					if (file.extract("`S ", &err)) {
						ServiceLocator::getLoggingService().warning("Skipping parameter", err);
						delete[] err;
						file.putBack(" ");	// To continue with proper parsing
					}
				}
				else if (file.extract("`?S`L", &err)) {
					if (err) {
						ServiceLocator::getLoggingService().error("Malformed keyword or extra data in line", err);
						delete[] err;
					}
					file.putBack("\n");
					break;
				}
			}
			PhysicsComponent* phys = new PhysicsComponent({ pos.x,pos.y,pos.z }, { at.x,at.y,at.z });
			PerspCamera* camera = new PerspCamera(800, 600, fov);	// How do I get these values in here?
			camera->update(phys);
			GameObject* camera_object = new GameObject(NULL, NULL, phys, NULL);
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
				delete[] objectName;
				continue;
			}
			mSceneCameras[objectName] = camera_object;
			delete[] objectName;
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
			delete[] objectName;
		}
		else if (file.extract("`?S`L", &objectName)) {
			if (objectName) {
				ServiceLocator::getLoggingService().error("Unrecognized line in level file", objectName);
				delete[] objectName;
			}
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
	for (auto object : mSceneObjects)
		delete object.second;
	mSceneObjects.clear();
	for (auto camera : mSceneCameras)
		delete camera.second;
	mSceneCameras.clear();
	mOwnLibraries.geometries.clear();
	mOwnLibraries.shaders.clear();
	mOwnLibraries.programs.clear();
	mOwnLibraries.textures.clear();
	mOwnLibraries.sounds.clear();
	if (mSceneAudio) delete mSceneAudio;
}

void Level::setBackgroundMusicVolume(float volume) {
	mSceneAudio->setVolume(volume);
}