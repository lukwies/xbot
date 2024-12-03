#ifndef XBOT_H
#define XBOT_H 1

#include "libxbot/libxbot.h"
#include "libxbot/mouse.h"
#include "libxbot/key.h"
#include "libxbot/util.h"

#include "cmd.h"

// Handle command 'capture (<FILE>)'
extern int xb_capture(Display *, const xb_cmd *);

// Handle command 'run <FILE>'
extern int xb_runfile(Display *, const xb_cmd *);

// Handle mouse commands
extern int xb_mouse_move(Display *D, const xb_cmd *);
extern int xb_mouse_moveto(Display *D, const xb_cmd *);
extern int xb_mouse_position(Display *D);
extern int xb_mouse_pixel_color(Display *D, const xb_cmd *);
extern int xb_mouse_click(Display *D, const xb_cmd *);

// Keyboard commands
extern int xb_keypress(Display *D, const xb_cmd *);

// Window commands
extern int xb_window_name(Display *D, const xb_cmd *);
extern int xb_window_focus(Display *D, const xb_cmd *);

#endif
