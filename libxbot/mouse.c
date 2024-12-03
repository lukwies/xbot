#include "libxbot.h"

static void make_mouse_button_event(Display *, XEvent *, int btn);


void xbot_mouse_position(Display *d, int *y, int *x)
{
	XEvent e;
	XQueryPointer(d, DefaultRootWindow(d),
		&e.xbutton.root, &e.xbutton.window,
		&e.xbutton.x_root, &e.xbutton.y_root,
		&e.xbutton.x, &e.xbutton.y,
		&e.xbutton.state);
	*y = e.xbutton.y;
	*x = e.xbutton.x;
}

void xbot_mouse_move(Display *D, int y, int x)
{
	XWarpPointer(D, None, None, 0,0,0,0, x, y);
	XFlush(D);
	usleep(2);
}

void xbot_mouse_move_to(Display *D, int y, int x)
{
	int cy, cx;
	xbot_mouse_position(D, &cy, &cx);
	xbot_mouse_move(D, -cy, -cx);
	xbot_mouse_position(D, &cy, &cx); // Don't ask me... \_o_/
	xbot_mouse_move(D, y, x);
}

void xbot_mouse_press(Display *D, int button)
{
	XEvent e;
	make_mouse_button_event(D, &e, button);
	e.type = ButtonPress;

	if (XSendEvent(D, PointerWindow, True, ButtonPressMask,
			&e) == 0) {
		fprintf(stderr, "! Failed to send mouse press event: %s\n",
				strerror(errno));
	}
	XFlush(D);
	usleep(1);
}
void xbot_mouse_press_at(Display *D, int button, int y, int x)
{
	xbot_mouse_move_to(D, y, x);
	xbot_mouse_press(D, button);
}

// Release mouse button
void xbot_mouse_release(Display *D, int button)
{
	XEvent e;
	make_mouse_button_event(D, &e, button);
	e.type = ButtonRelease;

	if (XSendEvent(D, PointerWindow, True, ButtonReleaseMask,
			&e) == 0) {
		fprintf(stderr, "! Failed to send mouse release event: %s\n",
				strerror(errno));
	}
	XFlush(D);
	usleep(1);
}
void xbot_mouse_release_at(Display *D, int button, int y, int x)
{
	xbot_mouse_move_to(D, y, x);
	xbot_mouse_release(D, button);
}


void xbot_mouse_click(Display *D, int button)
{
	xbot_mouse_press(D, button);
	xbot_mouse_release(D, button);
}

void xbot_mouse_click_at(Display *D, int button, int y, int x)
{
	xbot_mouse_move_to(D, y, x);
	xbot_mouse_click(D, button);
}


///////////////////// STATIC ///////////////////////////////////

static void make_mouse_button_event(Display *D, XEvent *e, int button)
{
	memset(e, 0, sizeof(XEvent));
	e->xbutton.button = button;
	e->xbutton.same_screen = True;
	e->xbutton.subwindow = DefaultRootWindow(D);

	while (e->xbutton.subwindow) {
		e->xbutton.window = e->xbutton.subwindow;
		XQueryPointer(D, e->xbutton.window,
			&e->xbutton.root, &e->xbutton.subwindow,
			&e->xbutton.x_root, &e->xbutton.y_root,
			&e->xbutton.x, &e->xbutton.y,
			&e->xbutton.state);
	}
}
