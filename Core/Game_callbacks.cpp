#include "Core/Game.h"
#include "Input/KeyboardHandler.h"
#include "Input/MouseHandler.h"

#include "GL/freeglut.h"

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

void gameLoop(int value) {
	static long long current, prev = Game::getTime();
	static float msPerFrame = 10.0f, lag = 0.0f;
	glutTimerFunc((int)msPerFrame, gameLoop, 0);
	current = Game::getTime();
	long long elapsed = current - prev;
	prev = current;
	lag += elapsed / 1000000.0f;
	while (lag > msPerFrame) {
		Game::getInstance().update(msPerFrame / 1000.0f);
		lag -= msPerFrame;
	}
	Game::getInstance().render(lag);
}

void Game::setupCallbacks() const {
	glutDisplayFunc(game_display_wrapper);
	glutTimerFunc(42, gameLoop, 0);
	glutReshapeFunc(game_resize_wrapper);
	glutKeyboardFunc(game_keydown_wrapper);
	glutKeyboardUpFunc(game_keyup_wrapper);
	glutMouseFunc(game_mouse_wrapper);
	glutMotionFunc(game_movement_wrapper);
	glutPassiveMotionFunc(game_movement_wrapper);
}