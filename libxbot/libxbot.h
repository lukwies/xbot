#ifndef LIBXBOT_H
#define LIBXBOT_H 1

/*
 * compile with -lX11
 */
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define AUTHOR       "peilnix"
#define VERSION      "0.1"
#define RELEASE_DATE "2022-08-30"


/* Open X-display by given name. The display_name parameter can
 * be NULL to use the default X-display.
 */
Display *xbot_open(const char *display_name);

/* Close X-display after using it. */
void xbot_close(Display *);

/* Get pixel color at given position. */
void xbot_pixel_color(Display *, int y, int x, XColor *);

/* Get window title */
char *xbot_window_name(Display *, Window, char *, size_t);
char *xbot_active_window_name(Display *, char *, size_t);

/* Get window by name
 * Returns 0 if window not found.
 */
Window xbot_get_window_by_name(Display *, const char *name);

/* Set window to focus. */
int xbot_focus_window(Display *, const char *name);

/* List all open window ids and titles. */
void xbot_list_windows(Display *);

#endif
