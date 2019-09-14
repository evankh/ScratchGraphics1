#include "Core/Game.h"
#include "Input/MouseHandler.h"
#include "Util/ServiceLocator.h"
#include "Core/Window.h"

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "IL/ilut.h"

// All initialization functions for OpenGL, other libraries, and the Game object go here

void Game::init(int argc, char* argv[]) {
	glutInit(&argc, argv);

	mWindow = new Window(800, 600, "Game owns this window");
	mWindow->printGLVersion();

	if (glewInit() == GLEW_OK) {
		mWindow->printGLVersion();
		Game::getInstance().setGLVersion(mWindow->getGLVersion());
		// Set callback functions
		setupCallbacks();
		// Set a few universal OpenGL settings
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glPointSize(3.0f);
		//glEnable(GL_FRAMEBUFFER_SRGB);
		//Not so sure about this one
		glActiveTexture(GL_TEXTURE0);
	}
	else {
		ServiceLocator::getLoggingService().error("Error initiating glew", "");
		throw std::exception("Error initializing GLEW");
	}
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	MouseHandler::getInstance().init(800, 600);
}

void Game::run() {
	glutMainLoop();
}

long long Game::getTime() {
	GLint64 time;
	glGetInteger64v(GL_TIMESTAMP, &time);
	return time;
}