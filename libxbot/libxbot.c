#include "libxbot.h"

Display *xbot_open(const char *display_name)
{
	Display *d = XOpenDisplay(display_name);
	if (!d) {
		fprintf(stderr, "! Failed to open X-display '%s': %s\n",
			(display_name?display_name:"default"),
			strerror(errno));
		return NULL;
	}
	return d;
}

void xbot_close(Display *d)
{
	if (d) XCloseDisplay(d);
}

void xbot_pixel_color(Display *d, int y, int x, XColor *col)
{
	XImage *img;
	img = XGetImage(d, DefaultRootWindow(d), x, y, 1, 1,
			AllPlanes, XYPixmap);
	col->pixel = XGetPixel(img, 0, 0);
	XDestroyImage(img);
	XQueryColor(d, DefaultColormap(d, DefaultScreen(d)), col);
}

char *xbot_window_name(Display *D, Window w, char *s, size_t n)
{
	char *x = NULL;
	if (XFetchName(D, w, &x)) {
		snprintf(s, n, "%s", x);
		XFree(x);
	} else {
		snprintf(s, n, "???");
	}
	return s;
}
char *xbot_active_window_name(Display *D, char *s, size_t n)
{
	Window focus;
	int    revert;
        XGetInputFocus (D, &focus, &revert);
	return xbot_window_name(D, focus, s, n);
}


Window xbot_get_window_by_name(Display *D, const char *wname)
{
	Window root = DefaultRootWindow(D);
	Window parent;
	Window *children;
	unsigned int num_children, i;
	Window found_window = 0;

	if (XQueryTree(D, root, &root, &parent, &children, &num_children)) {
		for (i = 0; i < num_children; i++) {
			char *name;
			if (XFetchName(D, children[i], &name)) {
				if (name && strcmp(name, wname) == 0) {
					found_window = children[i];
					XFree(name);
					break;
				}
				XFree(name);
			}
		}
		XFree(children);
	}
	return found_window;
}

int xbot_focus_window(Display *D, const char *wname)
{
	Window w = xbot_get_window_by_name(D, wname);
	if (!w) return -1;

	XSetInputFocus(D, w, RevertToParent, CurrentTime);
	XRaiseWindow(D, w);
	XFlush(D);
	return 0;
}

void xbot_list_windows(Display *D)
{
	Window root = DefaultRootWindow(D);
	Window parent;
	Window *children;
	unsigned int num_children, i;

	if (XQueryTree(D, root, &root, &parent, &children, &num_children)) {
		for (i = 0; i < num_children; i++) {
			char *name;
			if (XFetchName(D, children[i], &name)) {
				if (name) {
					printf("Window ID: %lu, Title: %s\n",
						children[i], name);
					XFree(name);
				} else {
					printf("Window ID: %lu, Title: (no title)\n",
						children[i]);
				}
			}
		}
		XFree(children);
	}
}
