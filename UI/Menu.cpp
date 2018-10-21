#include "Menu.h"
#include "UIElement.h"

Menu::Menu(Menu* parent, RootElement* contents) {
	mParent = parent;
	mRootElement = contents;
}

void Menu::draw() {
	mRootElement->draw();
}

void Menu::layout(int window_width, int window_height) {
	mRootElement->layout(0, window_width, 0, window_height);
}
