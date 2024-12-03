#include "xbot.h"

#define HELP_KEY "\n"\
 " Usage: xbot key <string>\n"\
 "        xbot key <key>\n"\
 "        xbot key <key1+key2+...>\n"\
 "\n"\
 " The argument(s) of the option 'key' can either be a string,\n"\
 " single key or shortcut (list of keys).\n"\
 "\n"\
 " Examples:\n"\
 "\n"\
 " xbot key 'Hello World!'\n"\
 " xbot key 'Hello World!'[return]\n"\
 " xbot key [F1]\n"\
 " xbot key [ctrl]+k\n"\
 " xbot key [altgr]+[shift]+X\n"\
 "\n"\
 " Supported keys: \n"\
 "\n"\
 " alt, alt-gr, backspace, caps-lock, clear, ctrl, ctrl-r,\n"\
 " del, down, end, esc, F1, F2, F3, F4, F5, F6, F7, F8,\n"\
 " F9, F10, F11, F12, home, left, page-down, page-up, paste,\n"\
 " pause, print, return, right, shift, shift-l, shift-r,\n"\
 " space, tab, up\n"\
 "\n"

int xb_keypress(Display *D, const xb_cmd *cmd)
{
	register size_t i;
	xbot_key keys[512];
	size_t   len;
	int      res;

	if (cmd->narg == 0) {
		xb_error("! key: Missing argumtens\n");
		return -1;
	} else if (!strcmp(cmd->arg[0], "help")) {
		xb_info(HELP_KEY);
		return -1;
	}

	for (i = 0; i < cmd->narg; i++) {

		len = xbot_key_parse(cmd->arg[i], keys, 512);
		if (len == 0) return -1;
		// Add a trailing whitespace
		if (i < cmd->narg-1 && len < 512) {
			keys[len].k = ' ';
			keys[len++].state = 0x00;
		}

		res = xbot_key_send(D, keys, len);
		if (res != 0) return -1;

		xbot_key_print(NULL, keys, len);
	}
	return 0;
}
