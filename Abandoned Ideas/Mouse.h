#ifndef __EKH_SCRATCH_GRAPHICS_1_MOUSE__
#define __EKH_SCRATCH_GRAPHICS_1_MOUSE__

enum MouseEventType {
	EKH_MOUSE_EVENT_NONE = 0,
	EKH_MOUSE_EVENT_CLICK,
	EKH_MOUSE_EVENT_DRAG,
	EKH_MOUSE_EVENT_DOUBLE_CLICK,
	EKH_MOUSE_EVENT_DOUBLE_DRAG,
	EKH_MOUSE_EVENT_ENTRY,

	EKH_NUM_MOUSE_EVENTS
};

struct SSBB {
	int x, y, w, h;
	bool inside(int x, int y);
};

struct MouseEvent {
	MouseEventType type;
	int x, y;
	int dx, dy;
	int button;
	int edge;
	MouseEvent* next;
};

class Mouse {
private:
	static MouseEvent* sHead;
	static MouseEvent* sTail;
public:
	~Mouse();
	static void append(MouseEvent* event);
	static MouseEvent* search(SSBB ssbb);
	static void clear();
};

void mouseButtonCallback(int button, int edge, int mouse_x, int mouse_y);
void mouseMoveCallback(int x, int y);
void mouseEntryCallback(int edge);

#endif//__EKH_SCRATCH_GRAPHICS_1_MOUSE__