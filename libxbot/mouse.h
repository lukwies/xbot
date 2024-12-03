#ifndef LIBXBOT_MOUSE_H
#define LIBXBOT_MOUSE_H 1

#include "libxbot.h"

/* Get current mouse coordinates. */
void xbot_mouse_position(Display *, int *y, int *x);

/* Move mouse relative. */
void xbot_mouse_move(Display *, int y, int x);

/* Move mouse to absolute position. */
void xbot_mouse_move_to(Display *, int y, int x);

/* Press mouse button */
void xbot_mouse_press(Display *, int button);
void xbot_mouse_press_at(Display *, int button, int y, int x);

/* Release mouse button */
void xbot_mouse_release(Display *, int button);
void xbot_mouse_release_at(Display *, int button, int y, int x);

/* Click mouse button */
void xbot_mouse_click(Display *, int button);
void xbot_mouse_click_at(Display *, int button, int y, int x);

#endif
