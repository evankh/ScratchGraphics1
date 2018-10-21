#ifndef __EKH_SCRATCH_GRAPHICS_1_MENU__
#define __EKH_SCRATCH_GRAPHICS_1_MENU__

#include <vector>
class RootElement;

class Menu {
	RootElement* mRootElement;
public:
	Menu(Menu* parent, RootElement* contents);
	void draw();
	void layout(int window_width, int window_height);
	Menu* mParent;	// Used for building a stack of Menus
};

#endif//__EKH_SCRATCH_GRAPHICS_1_MENU__