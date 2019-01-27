#define GL_TEST_MAIN main

#include "Game.h"
#include "ServiceLocator.h"
#include "KeyboardHandler.h"
#include "MouseHandler.h"
#include "Window.h"

#include <iostream>
#include <vector>
#include <cassert>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "IL/ilut.h"

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

int GL_TEST_MAIN(int argc, char* argv[]) {
	glutInit(&argc, argv);

	ServiceLocator::provideLoggingService(new ConsoleLoggingService);
	Game::getInstance().init();	// Oh gosh, I hope I can move this down without breaking everything... Oh gosh, I can't move this anywhere without breaking everything...

	int version[2];
	glGetIntegerv(GL_MAJOR_VERSION, version);
	glGetIntegerv(GL_MINOR_VERSION, version + 1);
	ServiceLocator::getLoggingService().log("Version: " + std::to_string(version[0]) + "." + std::to_string(version[1]));

	if (glewInit() == GLEW_OK) {
		glGetIntegerv(GL_MAJOR_VERSION, version);
		glGetIntegerv(GL_MINOR_VERSION, version + 1);
		ServiceLocator::getLoggingService().log("Version: " + std::to_string(version[0]) + "." + std::to_string(version[1]));
		Game::getInstance().setGLVersion(100 * version[0] + 10 * version[1]);
#pragma region callbacks
		glutDisplayFunc(game_display_wrapper);
		glutTimerFunc(42, gameLoop, 0);
		glutReshapeFunc(game_resize_wrapper);
		glutKeyboardFunc(game_keydown_wrapper);
		glutKeyboardUpFunc(game_keyup_wrapper);
		glutMouseFunc(game_mouse_wrapper);
		glutMotionFunc(game_movement_wrapper);
#pragma endregion
		//glEnable(GL_CULL_FACE);
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
		return 1;
	}
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	Game::getInstance().load();
	glutMainLoop();
	ServiceLocator::getLoggingService().log("After the main loop");
	system("pause");
	return 0;
}