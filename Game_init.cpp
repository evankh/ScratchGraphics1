#include "Game.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "ServiceLocator.h"
#include "Window.h"

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "IL/ilut.h"

// All initialization functions for OpenGL, OpenAL, and the Game object go here

void game_display_wrapper() {}

void game_resize_wrapper(int width, int height) {
	Game::getInstance().resize(width, height);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Game::getInstance().render(0.0f);
	glutSwapBuffers();
}

void game_keydown_wrapper(unsigned char key, int mouse_x, int mouse_y) {
	// Temporary hard break
	if (key == 27)
		glutLeaveMainLoop();
	else
		KeyboardHandler::getInstance().handlePress(key, mouse_x, mouse_y);
}

void game_keyup_wrapper(unsigned char key, int mouse_x, int mouse_y) {
	KeyboardHandler::getInstance().handleRelease(key, mouse_x, mouse_y);
}

void game_mouse_wrapper(int button, int edge, int mouse_x, int mouse_y) {
	MouseHandler::getInstance().handleButton((MouseButton)button, edge, mouse_x, mouse_y);
}

void game_movement_wrapper(int mouse_x, int mouse_y) {
	// Mouse position can quite happily be outside the window, will have to rework some code to accommodate that
	MouseHandler::getInstance().handleMove(mouse_x, mouse_y);
}

GLint64 getCurrentTime() {
	GLint64 time;
	glGetInteger64v(GL_TIMESTAMP, &time);
	return time;
}

void gameLoop(int value) {
	static GLint64 current, prev = getCurrentTime();
	static float msPerFrame = 10.0f, lag = 0.0f;
	glutTimerFunc((int)msPerFrame, gameLoop, 0);
	current = getCurrentTime();
	GLint64 elapsed = current - prev;
	prev = current;
	lag += elapsed / 1000000.0f;
	while (lag > msPerFrame) {
		Game::getInstance().update(msPerFrame / 1000.0f);
		lag -= msPerFrame;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Game::getInstance().render(lag);
	glutSwapBuffers();
}

void Game::init(int argc, char* argv[]) {
	glutInit(&argc, argv);

	ServiceLocator::provideLoggingService(new ConsoleLoggingService);

	mWindow = new Window(800, 600, "Game owns this window");
	// Set up the MouseHandler textures, probably
	// Really probably ought to move GLEW initialization (and other inits) in here, so I know where they're being initialized
	// And in what order instead of aving them spread across several different files
	mWindow->printGLVersion();
	if (glewInit() == GLEW_OK) {
		mWindow->printGLVersion();
		Game::getInstance().setGLVersion(mWindow->getGLVersion());
		// Set callback functions
		glutDisplayFunc(game_display_wrapper);
		glutTimerFunc(42, gameLoop, 0);
		glutReshapeFunc(game_resize_wrapper);
		glutKeyboardFunc(game_keydown_wrapper);
		glutKeyboardUpFunc(game_keyup_wrapper);
		glutMouseFunc(game_mouse_wrapper);
		glutMotionFunc(game_movement_wrapper);
		glutPassiveMotionFunc(game_movement_wrapper);
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