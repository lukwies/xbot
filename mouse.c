#include "xbot.h"

static int move_mouse(Display *D, int move_to, const xb_cmd *cmd);

// Command 'move'
int xb_mouse_move(Display *D, const xb_cmd *cmd)
{
	return move_mouse(D, 0, cmd);
}

// Command 'moveto'
int xb_mouse_moveto(Display *D, const xb_cmd *cmd)
{
	return move_mouse(D, 1, cmd);
}

// Command 'mousepos'
int xb_mouse_position(Display *D)
{
	int y, x;
	xbot_mouse_position(D, &y, &x);
	xb_info("Mouse position y=%d, x=%d\n", y, x);
	return 0;
}

// Map color from XColor (2byte) to 1byte
static void print_color(XColor *c) {
	double pxc = 100.0 / 65535.0;
	double pc  = 256.0 / 100.0;
	int r = c->red * pxc * pc;
	int g = c->green * pxc * pc;
	int b = c->blue * pxc * pc;
	printf("color 0x%02x,0x%02x,0x%02x\n", r, g, b);
}

// Command 'pxcolor'
int xb_mouse_pixel_color(Display *D, const xb_cmd *cmd)
{
	int my, mx;
	XColor c;
	switch (cmd->narg) {
	case 0:
		xbot_mouse_position(D, &my, &mx);
		break;
	case 1: // Left button click at given Y/X pos
		if (xb_str2int(cmd->arg[0], &my) ||
				xb_str2int(cmd->arg[1], &mx)) {
			xb_error("! pxcolor: Y/X position must be numeric\n");
			return -1;
		}
		break;
	default:
		xb_error("! pxcolor: Invalid number of arguments\n");
		return -1;
	}
	xbot_pixel_color(D, my, mx, &c);
	print_color(&c);
	return 0;
}

// Command 'click'
int xb_mouse_click(Display *D, const xb_cmd *cmd)
{
	int btn=0, y, x;

	switch (cmd->narg) {
	case 0:	// Left click at current pos
		xbot_mouse_click(D, 1);
		xb_info("click\n");
		break;
	case 1:	// Button click at current pos
		if (xb_str2int(cmd->arg[0], &btn) || btn < 1 || btn > 3) {
			xb_error("! click: Button id must be numeric (1-3)\n");
			return -1;
		}
		xbot_mouse_click(D, btn);
		xb_info("click %d\n", btn);
		break;
	case 2: // Left button click at given Y/X pos
		if (xb_str2int(cmd->arg[0], &y) ||
				xb_str2int(cmd->arg[1], &x)) {
			xb_error("! click: Y/X position must be numeric\n");
			return -1;
		}
		xbot_mouse_click_at(D, 0, y, x);
		xb_info("click %d %d\n", y, x);
		break;
	case 3: // Button click at given Y/X pos
		if (xb_str2int(cmd->arg[0], &y) ||
				xb_str2int(cmd->arg[1], &x)) {
			xb_error("! click: Y/X position must be numeric\n");
			return -1;
		}
		if (xb_str2int(cmd->arg[2], &btn) || btn < 1 || btn > 3) {
			xb_error("! click: Button id must be numeric (1-3)\n");
			return -1;
		}
		xbot_mouse_click_at(D, btn, y, x);
		xb_info("click %d %d %d\n", y, x, btn);
		break;
	default:
		xb_error("! click: Invalid arguments\n");
		return -1;
	}
	return 0;
}

////////////////////// STATIC ////////////////////////////////////

static int move_mouse(Display *D, int move_to, const xb_cmd *cmd)
{
	int y, x;
	if (cmd->narg != 2) {
		xb_error("Usage: xbot move%s <Y> <X>\n",
				(move_to ? "to" : ""));
		return -1;
	}
	if (xb_str2int(cmd->arg[0], &y) ||
			xb_str2int(cmd->arg[1], &x)) {
		xb_error("! move: Y/X position must be numeric\n");
		return -1;
	}

	if (move_to)
		xbot_mouse_move_to(D, y, x);
	else	xbot_mouse_move(D, y, x);

	xb_info("move%s %d %d\n", (move_to?"to":""), y, x);

	return 0;
}
