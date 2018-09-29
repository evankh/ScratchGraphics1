#include "Level.h"
#include "ServiceLocator.h"

// I can't believe this actually works
const NamedContainer<Geometry*>& Level::sGeometryLibrary = Game::getInstance().mGeometries;
const NamedContainer<Program*>& Level::sProgramLibrary = Game::getInstance().mPrograms;
const NamedContainer<Texture*>& Level::sTextureLibrary = Game::getInstance().mTextures;
const NamedContainer<InputComponent*>& Level::sInputLibrary = Game::getInstance().mInputs;
/* That thing you just said there? "Oh boy, I really don't want to fuck with this code"? Yeah, that's a good sign you should rethink how you're doing this part. */

Level::Level(const char* filepath) {
	FileService& file = ServiceLocator::getFileService(filepath);
	if (file.good()) {
		char* objectName;
		while (file.good()) {
			if (file.extract("Object \"\\S\"", &objectName)) {
				// Set up variables and structs to read into
				char* geomName, *progName, *inputName, *err;
				struct { float x, y, z; } pos{ 0.0f,0.0f,0.0f }, vel{ 0.0f,0.0f,0.0f }, acc{ 0.0f,0.0f,0.0f }, rot{ 0.0f,0.0f,1.0f }, scl{ 1.0f,1.0f,1.0f };
				float ang = 0.0f;
				Geometry* geom = NULL;
				Program* program = NULL;
				InputComponent* input = NULL;
				bool valid = true;
				while (!file.extract("\\L", NULL)) {
					if (file.extract(" geom:\"\\S\"", &geomName)) {
						// Check if the geometry exists in the geometry library
						// If so, save a reference to it to pass to the GameObject constructor
						// If not, raise an error and break, set a bool so we can skip the construction of the object, and eat the rest of the line
						// (Same general procedure applies to all parameters)
						if (geom = sGeometryLibrary.get(geomName))
							delete geomName;
						else {
							ServiceLocator::getLoggingService().error("Geometry not found", geomName);
							delete geomName;
							valid = false;
							file.extract("\\?S\\L", NULL);
							break;
						}
					}
					else if (file.extract(" prog:\"\\S\"", &progName)) {
						if (program = sProgramLibrary.get(progName))
							delete progName;
						else {
							ServiceLocator::getLoggingService().error("Program not found", progName);
							delete progName;
							valid = false;
							file.extract("\\?S\\L", NULL);
							break;
						}
					}
					else if (file.extract(" pos:(\\F,\\F,\\F)", &pos)) {}	// Not sure I actually need to do anything for the raw data?
					else if (file.extract(" vel:(\\F,\\F,\\F)", &vel)) {}
					else if (file.extract(" acc:(\\F,\\F,\\F)", &acc)) {}
					else if (file.extract(" rot:(\\F,\\F,\\F)", &rot)) {}
					else if (file.extract(" ang:\\F", &ang)) {}
					// Will need to account for angular velocity here as well
					else if (file.extract(" scl:\\F", &scl.x)) {
						scl.z = scl.y = scl.x;
					}
					else if (file.extract(" scl:(\\F,\\F,\\F)", &scl)) {}
					else if (file.extract(" input:\"\\S\"", &inputName)) {
						if (input = sInputLibrary.get(inputName))
							delete inputName;
						else {
							ServiceLocator::getLoggingService().error("Input component not found", inputName);
							delete inputName;
							valid = false;	// Actually it is probably fine to be valid as long as an error mesage is printed
							file.extract("\\?S\\L", NULL);
							break;
						}
					}
					else if (file.extract("\\S:", &err)) {
						ServiceLocator::getLoggingService().error("Unexpected keyword in line", err);
						delete err;
						if (file.extract("\\S ", &err)) {
							ServiceLocator::getLoggingService().error("Skipping parameter", err);
							delete err;
							file.putBack(" ");	// To continue with proper parsing
						}
					}
					else if (file.extract("\\?S\\L", &err)) {
						if (err) {
							ServiceLocator::getLoggingService().error("Malformed keyword or extra data in line (skipping line)", err);
							delete err;
						}
						file.putBack("\n");	// Not really sure how I should handle this with the new \\L, but his works fine for now
						valid = false;
						break;
					}
				}
				if (valid) {
					// Construction of the actual object
					// It actually looks safe for any of the components to be NULL so the validity checking may not be necessary
					// I'm really not sure about the whole PhysicsComponent Library concept, it seems perfectly plausible to generate them here on the fly, and wouldn't allow multiple objects to be pointing to the same PhysicsComponent
					PhysicsComponent* physics = new PhysicsComponent;
					physics->scale({ scl.x,scl.y,scl.z });
					physics->rotate({ rot.x,rot.y,rot.z }, ang);
					physics->translate({ pos.x,pos.y,pos.z });
					// Also store velocity
					mGameObjects.push_back(new GameObject(geom, program, physics, input));
				}
				// Cleanup
				delete objectName;
			}
			else if (file.extract("\\?S\\L", &objectName)) {
				if (objectName) {
					ServiceLocator::getLoggingService().error("Unrecognized line in level file", objectName);
					delete objectName;
				}
			}
		}
	}
	else {
		ServiceLocator::getLoggingService().badFileError(filepath);
	}
	file.close();
}

Level::Level(std::string filepath) :Level(filepath.data()) {}

Level::~Level() {
	for (auto object : mGameObjects)
		delete object;
	mGameObjects.clear();
}
