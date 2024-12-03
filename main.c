#include "xbot.h"
#include "cmd.h"

#define HELP "\n"\
 " xbot-" VERSION "\n"\
 " Raise x11 events from the command-line.\n"\
 "\n"\
 " Usage: xbot OPTION (ARGS ...)\n"\
 "\n"\
 " list                      List all open window ids and titles\n"\
 " capture                   Capture mouse and keypress events and write\n"\
 "                           results to terminal (stdout).\n"\
 " capture <file>            Capture mouse and keypress events and write\n"\
 "                           results to given file.\n"\
 " run <file>                Execute all commands from given file, created\n"\
 "                           by capture cmd or manually. Type 'run help'\n"\
 "                           for file format description.\n"\
 "\n"\
 " move <Y> <X>              Move mouse pointer to relative y/x position.\n"\
 " moveto <Y> <X>            Move mouse pointer to absolute y/x position.\n"\
 "\n"\
 " click                     Click left mouse button at current postion.\n"\
 " click <Y> <X>             Click left mouse button at y/x postion.\n"\
 " click <button>            Click mouse button: 1=Left,2=Center,3=Right,\n"\
 "                           4=ScrollUp,5=ScrollDown at current position.\n"\
 " click <Y> <X> <button>    Click mouse button at y/x position.\n"\
 "\n"\
 " key <keys>                Send one or more key events to x11 server.\n"\
 "                           Type 'key help' for more information.\n"\
 "\n"\
 " mousepos                  Prints the current mouse position.\n"\
 " pxcolor                   Get pixel color at current mouse position.\n"\
 " pxcolor <Y> <X>           Get pixel color at given y/x position.\n"\
 "\n"\
 " wname                     Get title of currently selected window\n"\
 "\n"
/*
 " wname <id|name> <name>    Set title of currently selected window\n"\
 " focus <id>                Set focus window by window id\n"\
 " focus <window-name>       Set focus window by window name\n"\
*/
int main(int argc, char *argv[])
{
	Display *D;
	xb_cmd cmd;
	int    res;

	res = xb_cmd_parse_cmdline(argc, argv, &cmd);

	if (res || cmd.id == CMD_HELP || cmd.id == CMD_UNKNOWN) {
		fputs(HELP, stderr);
		return 1;
	}

	D = xbot_open(NULL);

	switch (cmd.id) {
	case CMD_LIST:
		xbot_list_windows(D);
		break;
	case CMD_CAPTURE:
		res = xb_capture(D, &cmd);
		break;
	case CMD_RUN:
		res = xb_runfile(D, &cmd);
		break;
	case CMD_FOCUS:
		res = xb_window_focus(D, &cmd);
		break;
	case CMD_WNAME:
		res = xb_window_name(D, &cmd);
		break;
	case CMD_MOUSE_MOVE:
		res = xb_mouse_move(D, &cmd);
		break;
	case CMD_MOUSE_MOVETO:
		res = xb_mouse_moveto(D, &cmd);
		break;
	case CMD_MOUSE_CLICK:
		res = xb_mouse_click(D, &cmd);
		break;
	case CMD_MOUSE_POS:
		res = xb_mouse_position(D);
		break;
	case CMD_KEY:
		res = xb_keypress(D, &cmd);
		break;
	case CMD_PX_COLOR:
		res = xb_mouse_pixel_color(D, &cmd);
		break;
	default:
		xb_error("! Unknown command '%s'\n", argv[1]);
		res = -1;
		break;
	}
	xb_cmd_free(&cmd);
	xbot_close(D);
	return res ? 1 : 0;
}
