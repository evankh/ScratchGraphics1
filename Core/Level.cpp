#include "Core/Level.h"
#include "Components/Collision.h"
#include "Graphics/Camera.h"
#include "Util/ServiceLocator.h"
#include "Sound/Sound.h"
#include "Core/State.h"
#include "glm/gtx/transform.hpp"

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
			struct { float x = 0.0f, y = 0.0f, z = 0.0f; } pos, vel, acc, scl, col, ang, mom;
			scl.x = scl.y = scl.z = 1.0f;
			GeometryComponent* geom = nullptr;
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
					}
				}
				else if (file.extract(" col:(`F,`F,`F)", &col)) { hasCol = true; }
				else if (file.extract(" sounds:(")) {
					while (!file.extract(")")) {
						auto_cstr soundName;
						file.extract("\"`S\"", &soundName);
						sounds.push_back(soundName);
						file.extract(",");	// No checking to make sure it's properly formatted
					}
				}
				else if (file.extract(" pos:(`F,`F,`F)", &pos));
				else if (file.extract(" vel:(`F,`F,`F)", &vel));
				else if (file.extract(" acc:(`F,`F,`F)", &acc));
				else if (file.extract(" ang:(`F,`F,`F)", &ang));
				else if (file.extract(" mom:(`F,`F,`F)", &mom));
				else if (file.extract(" scl:`F", &scl.x)) { scl.z = scl.y = scl.x; }
				else if (file.extract(" scl:(`F,`F,`F)", &scl));
				else if (file.extract(" input:\"`S\"", &inputName)) { input = (char*)inputName; }
				else if (file.extract("`S:", &err)) {
					ServiceLocator::getLoggingService().error("Unexpected keyword in line", err);
					auto_cstr err2;
					if (file.extract("`S`W", &err2)) {
						ServiceLocator::getLoggingService().error("Skipping parameter", err2);
						file.putBack("`W");	// To continue with proper parsing
					}
				}
				else if (file.extract("`?S`L", &err)) {
					if (err) ServiceLocator::getLoggingService().error("Malformed keyword or extra data in line (skipping line)", err);
					file.putBack("\n");	// Not really sure how I should handle this with the new `L, but this works fine for now
					valid = false;
					break;
				}
			}
			if (valid) {
				// Construction of the actual object
				CollisionComponent* bounds = nullptr;
				if (!bound.empty()) {
					if (bound == "Sphere")
						bounds = new BoundingSphere({ 0.0f,0.0f,0.0f }, 1.0f);
					else if (bound == "AABB")
						bounds = geom->getBoundingBox()->copy();
					else if (bound == "Plane")
						bounds = new CollisionPlane(glm::vec3(pos.x, pos.y, pos.z), glm::vec3(-cosf(ang.x)*sinf(ang.y), -sinf(ang.x)*sinf(ang.y), cosf(ang.y)));
					else
						ServiceLocator::getLoggingService().error("Unknown bounding box type", bound);
				}
				PhysicsComponent* physics = new PhysicsComponent(bounds, { vel.x,vel.y,vel.z }, { acc.x,acc.y,acc.z }, { ang.x,ang.y,ang.z }, { mom.x,mom.y,mom.z });
				physics->scale({ scl.x,scl.y,scl.z });
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
						delete physics;
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
				mPCs.push_back(physics);
			}
		}
		else if (file.extract("`?WCamera \"`S\"", &objectName)) {
			std::string parent;
			struct { float x = 0.0f, y = 0.0f, z = 0.0f; } pos, at;
			float fov = 75.0;
			while (!file.extract("`L")) {
				auto_cstr err, err2, parentName;
				if (file.extract(" parent:\"`S\"", &parentName)) {
					if (!mSceneGraph.contains(parentName))
						ServiceLocator::getLoggingService().error("Parent object not found", parentName);
					else
						parent = (char*)parentName;
				}
				else if (file.extract(" pos:(`F,`F,`F)", &pos));
				else if (file.extract(" at:(`F,`F,`F)", &at));
				else if (file.extract(" fov:`F", &fov));
				else if (file.extract("`S:", &err)) {
					ServiceLocator::getLoggingService().warning("Unexpected keyword in line", err);
					if (file.extract("`S`W", &err2)) {
						ServiceLocator::getLoggingService().warning("Skipping parameter", err2);
						file.putBack("`W");	// To continue with proper parsing
					}
				}
				else if (file.extract("`?S`L", &err)) {
					if (err)
						ServiceLocator::getLoggingService().error("Malformed keyword or extra data in line", err);
					file.putBack("`L");
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
				camera_object->setState(state);
			}
			catch (std::out_of_range e) {
				ServiceLocator::getLoggingService().error("Cannot find AI state", e.what());
				delete camera_object;
				delete camera;
				// Does GameObject take care of its PhysicsComponent? Does Camera? Uh oh
				continue;
			}
			if (!parent.empty()) mSceneGraph.add(objectName, parent, camera_object);
			else mSceneGraph.add(objectName, camera_object);
			mSceneCameras.push_back(objectName);
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
		else if (file.extract("Main Camera \"`S\"`L", &objectName))
			mMainCamera = (char*)objectName;
		else if (file.extract("`?S`L", &objectName)) {
			if (objectName)
				ServiceLocator::getLoggingService().error("Unrecognized line in level file", objectName);
		}
	}
}

Level::~Level() {
	mSceneCameras.clear();
	mSceneGraph.clear();
	mOwnLibraries.geometries.clear();
	mOwnLibraries.shaders.clear();
	mOwnLibraries.programs.clear();
	mOwnLibraries.textures.clear();
	mOwnLibraries.sounds.clear();
	delete &mOwnLibraries;	// Haha that's super dangerous
	for (auto i : mPCs)
		delete i;
	mPCs.clear();
}

template<class T>
int index(const std::vector<T>& list, const T& val) {
	for (unsigned int i = 0; i < list.size(); i++)
		if (list[i] == val) return i;
	return -1;
}

void Level::getWorkingSet(GameObject** workingSet, PhysicsComponent** workingPCs) {
	*workingSet = new GameObject[mSceneGraph.count()];
	*workingPCs = new PhysicsComponent[mSceneGraph.count()];
	std::vector<std::string> objectNames = mSceneGraph.breadthFirstTraversal();
	int i = 0;
	for (std::string name : objectNames) {
		mSceneGraph.get(name)->copyTo(*workingSet + i);
		(*workingSet)[i].setPhysicsComponent(*workingPCs + i);
		(*workingPCs)[i].copyFrom(mSceneGraph.get(name)->getPhysicsComponent());
		if (mSceneGraph.hasParent(name)) {
			std::string parentName = mSceneGraph.getParent(name);
			(*workingPCs)[i].setParent(*workingPCs + index(objectNames, parentName));
		}
		i++;
	}
}

std::vector<int> Level::getAllCameras() const {
	std::vector<int> result;
	std::vector<std::string> list = mSceneGraph.breadthFirstTraversal();
	for (auto i : mSceneCameras)
		result.push_back(index(list, i));
	return result;
}

int Level::getMainCameraIndex() const {
	return index(mSceneGraph.breadthFirstTraversal(), mMainCamera);
}