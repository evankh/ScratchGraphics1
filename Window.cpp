#include "Window.h"
#include "GL\glew.h"
#include "GL\freeglut.h"

Window::Window() {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 600);
	int window_handle = glutCreateWindow("Test");
	glViewport(0, 0, 800, 600);
}

Window::Window(int width, int height, char* title) {
	mWidth = width;
	mHeight = height;
	mTitle = title;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(mWidth, mHeight);
	mHandle = glutCreateWindow(mTitle);
	glViewport(0, 0, mWidth, mHeight);
	// Probably should make sure it worked or something...
}

Window::~Window() {
	glutDestroyWindow(mHandle);
}

void Window::enableDrawing() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mWidth, mHeight);
}

void Window::resize(unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;
	glutReshapeWindow(width, height);
	glViewport(0, 0, mWidth, mHeight);
}

void Window::rename(const char* title) {
	mTitle = title;
	glutSetWindowTitle(mTitle);
}