#include "Level.h"
#include "Bounds.h"
#include "Camera.h"
#include "ServiceLocator.h"
#include "Source.h"
// Need to find a better way to include these - probably a library like the others, containing maybe factory methods for the entry point to each state machine?
#include "Sample States/PlayerStates.h"

Level::Level(std::string filepath, NamedContainer<Geometry*>& geomLibrary, NamedContainer<Program*>& progLibrary, NamedContainer<Texture*>& texLibrary, NamedContainer<PhysicsComponent*>physLibrary, SoundLibrary& soundLibrary)
	:mGeometryLibrary(geomLibrary), mProgramLibrary(progLibrary), mTextureLibrary(texLibrary), mPhysicsLibrary(physLibrary), mSoundLibrary(soundLibrary) {
	// This all needs to get WAY more robust with the new exceptions
	FileService file(filepath);
	if (file.good()) {
		char* objectName;
		while (file.good()) {
			if (file.extract("//`S`L", NULL));
			else if (file.extract("Object \"`S\"", &objectName)) {
				if (mSceneObjects.count(objectName)) {
					ServiceLocator::getLoggingService().error("Attempted redefinition of object", objectName);
					delete objectName;
					file.extract("`S`L", NULL);
					continue;
				}
				// Set up variables and structs to read into
				char* geomName, *progName, *texName, *inputName, *soundName, *boundsName, *err;
				struct { float x, y, z; } pos{ 0.0f,0.0f,0.0f }, vel{ 0.0f,0.0f,0.0f }, acc{ 0.0f,0.0f,0.0f }, rot{ 0.0f,0.0f,1.0f }, scl{ 1.0f,1.0f,1.0f }, axis{ 0.0f,0.0f,1.0f }, col{ 0.0f,0.0f,0.0f };
				float ang = 0.0f, mom = 0.0f;
				Geometry* geom = NULL;
				Program* program = NULL;
				Bounds* bounds = NULL;
				bool isPlayer = false;	// Weird
				Texture* texture = NULL;
				std::vector<std::string> sounds;
				bool valid = true, hasCol = false;
				while (!file.extract("`L", NULL)) {
					if (file.extract(" bounds:`S ", &boundsName)) {
						if (strcmp(boundsName, "Sphere") == 0)
							bounds = new BoundingSphere({ 0.0f,0.0f,0.0f }, 1.0f);
						else
							ServiceLocator::getLoggingService().error("Unknown bounding box type", boundsName);
						delete[] boundsName;
						file.putBack(' ');
					}
					else if (file.extract(" geom:\"`S\"", &geomName)) {
						// Check if the geometry exists in the geometry library
						// If so, save a reference to it to pass to the GameObject constructor
						// If not, raise an error and break, set a bool so we can skip the construction of the object, and eat the rest of the line
						// (Same general procedure applies to all parameters)
						if (geom = mGeometryLibrary.get(geomName))
							delete geomName;
						else {
							ServiceLocator::getLoggingService().error("Geometry not found", geomName);
							delete geomName;
							valid = false;
							file.extract("`?S`L", NULL);
							break;
						}
					}
					else if (file.extract(" prog:\"`S\"", &progName)) {
						if (program = mProgramLibrary.get(progName))
							delete progName;
						else {
							ServiceLocator::getLoggingService().error("Program not found", progName);
							delete progName;
							valid = false;
							file.extract("`?S`L", NULL);
							break;
						}
					}
					else if (file.extract(" tex:\"`S\"", &texName)) {
						if (texture = mTextureLibrary.get(texName))
							delete texName;
						else {
							ServiceLocator::getLoggingService().error("Texture not found", texName);
							delete texName;
							valid = false;
							file.extract("`?S`L", NULL);
							break;
						}
					}
					else if (file.extract(" col:(`F,`F,`F)", &col)) { hasCol = true; }
					else if (file.extract(" sounds:(", NULL)) {
						while (file.extract("\"`S\"", &soundName)) {
							sounds.push_back(soundName);
							delete soundName;
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
					else if (file.extract(" input:\"`S\"", &inputName)) {
						if (strcmp(inputName, "player") == 0) {
							isPlayer = true;
							delete inputName;
						}
						else {
							ServiceLocator::getLoggingService().error("Input component not found", inputName);
							delete inputName;
							//valid = false;	// Actually it is probably fine to be valid as long as an error mesage is printed
							file.extract("`?S`L", NULL);
							break;
						}
					}
					else if (file.extract("`S:", &err)) {
						ServiceLocator::getLoggingService().error("Unexpected keyword in line", err);
						delete err;
						if (file.extract("`S ", &err)) {
							ServiceLocator::getLoggingService().error("Skipping parameter", err);
							delete err;
							file.putBack(" ");	// To continue with proper parsing
						}
					}
					else if (file.extract("`?S`L", &err)) {
						if (err) {
							ServiceLocator::getLoggingService().error("Malformed keyword or extra data in line (skipping line)", err);
							delete err;
						}
						file.putBack("\n");	// Not really sure how I should handle this with the new `L, but his works fine for now
						valid = false;
						break;
					}
				}
				if (valid) {
					// Construction of the actual object
					// It actually looks safe for any of the components to be NULL so the validity checking may not be necessary
					// I'm really not sure about the whole PhysicsComponent Library concept, it seems perfectly plausible to generate them here on the fly, and wouldn't allow multiple objects to be pointing to the same PhysicsComponent
					PhysicsComponent* physics = new PhysicsComponent(bounds, { vel.x,vel.y,vel.z }, { acc.x,acc.y,acc.z }, { axis.x, axis.y, axis.z }, mom);
					physics->scale({ scl.x,scl.y,scl.z });
					physics->rotate({ rot.x,rot.y,rot.z }, ang);
					physics->translate({ pos.x,pos.y,pos.z });
					glm::vec3* color = NULL;
					if (hasCol) color = new glm::vec3(col.x, col.y, col.z);
					// Also store velocity
					GameObject* object = new GameObject(geom, program, physics, /*input, */texture, color);
					if (isPlayer)
						object->setState(PlayerState::getEntry(object));
					else
						object->setState(PassThruState::getEntry(object));
					for (auto sound : sounds) {
						if (mSoundLibrary.get(sound))
							object->registerSound(sound, mSoundLibrary.get(sound));
						else
							ServiceLocator::getLoggingService().error("Sound not found", sound);
					}
					mSceneObjects[objectName] = object;
				}
				// Cleanup
				delete objectName;
			}
			else if (file.extract("Camera \"`S\"", &objectName)) {
				if (mSceneCameras.count(objectName)) {
					ServiceLocator::getLoggingService().error("Attempted redefinition of camera", objectName);
					delete objectName;
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
						ServiceLocator::getLoggingService().error("Unexpected keyword in line", err);
						delete err;
						if (file.extract("`S ", &err)) {
							ServiceLocator::getLoggingService().error("Skipping parameter", err);
							delete err;
							file.putBack(" ");	// To continue with proper parsing
						}
					}
					else if (file.extract("`?S`L", &err)) {
						if (err) {
							ServiceLocator::getLoggingService().error("Malformed keyword or extra data in line (skipping line)", err);
							delete err;
						}
						file.putBack("\n");
						break;
					}
				}
				PerspCamera* camera = new PerspCamera(new PhysicsComponent({ pos.x,pos.y,pos.z }, { at.x,at.y,at.z }), 800, 600, fov);	// How do I get these values in here?
				mSceneCameras[objectName] = camera;
				delete objectName;
			}
			else if (file.extract("Music \"`S\"", &objectName)) {
				if (mBackgroundMusic) {
					ServiceLocator::getLoggingService().error("Attempted redefinition of background music", objectName);
					delete objectName;
				}
				else if (mBackgroundMusic = mSoundLibrary.get(objectName))
					delete objectName;
				else {
					ServiceLocator::getLoggingService().error("Background music not found", objectName);
					delete objectName;
				}
			}
			else if (file.extract("`?S`L", &objectName)) {
				if (objectName) {
					ServiceLocator::getLoggingService().error("Unrecognized line in level file", objectName);
					delete objectName;
				}
			}
		}
		if (mSceneCameras.count("main"))
			mCurrentCamera = mSceneCameras.at("main");
		else
			ServiceLocator::getLoggingService().error("No camera \"main\" defined in level", filepath);
		if (mBackgroundMusic) {
			mSceneAudio = new Source(mCurrentCamera->getPhysics(), true);
			mSceneAudio->update();
			mSceneAudio->playSound(mBackgroundMusic);
		}
	}
	else {
		throw std::exception(filepath.data());
	}
}

Level::~Level() {
	for (auto object : mSceneObjects)
		delete object.second;
	mSceneObjects.clear();
	for (auto camera : mSceneCameras)
		delete camera.second;
	mSceneCameras.clear();
	if (mSceneAudio) delete mSceneAudio;
}

void Level::setBackgroundMusicVolume(float volume) {
	mSceneAudio->setVolume(volume);
}