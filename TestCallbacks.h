#ifndef __EKH_SCRATCH_GRAPHICS_1_TEST_CALLBACKS__
#define __EKH_SCRATCH_GRAPHICS_1_TEST_CALLBACKS__

#include "ServiceLocator.h"

void testDisplay() {}
void testTimer(int) {
	ServiceLocator::getLoggingService().log("Timer");
	glutTimerFunc(2000, testTimer, 0);
}
void testCloseWindow() {
	ServiceLocator::getLoggingService().log("Close Window");
}
void testResizeWindow(int width, int height) {
	ServiceLocator::getLoggingService().log("Window resized to (" + std::to_string(width) + ", " + std::to_string(height) + ")");
}
void testPositionWindow(int x, int y) {
	ServiceLocator::getLoggingService().log("Window moved to (" + std::to_string(x) + ", " + std::to_string(y) + ")");
}
void testKeyboard(unsigned char key, int mouse_x, int mouse_y) {
	ServiceLocator::getLoggingService().log("Key " + std::string(1, key) + " down at (" + std::to_string(mouse_x) + ", " + std::to_string(mouse_y) + ")");
}
void testKeyboardUp(unsigned char key, int mouse_x, int mouse_y) {
	ServiceLocator::getLoggingService().log("Key " + std::string(1, key) + " up at (" + std::to_string(mouse_x) + ", " + std::to_string(mouse_y) + ")");
}
void testMouseButton(int button, int edge, int mouse_x, int mouse_y) {
	char* names[13] = { "Left", "Middle", "Right", "Scroll up","Scroll down","3","4","5","6","7","8","9","0" };
	ServiceLocator::getLoggingService().log(std::string(names[button]) + " mouse button " + ((edge == 0) ? "pressed" : "lifted") + " at (" + std::to_string(mouse_x) + ", " + std::to_string(mouse_y) + ")");
}
void testMouseMove(int, int) {}
void testMouseEntry(int edge) {
	ServiceLocator::getLoggingService().log("Mouse " + std::string(edge == 1 ? "enters" : "exits") + " window");
}

#endif//__EKH_SCRATCH_GRAPHICS_1_TEST_CALLBACKS__