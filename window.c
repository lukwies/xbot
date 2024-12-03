#include "xbot.h"

int xb_window_name(Display *D, const xb_cmd *cmd)
{
	char buf[128];
	char *n = xbot_active_window_name(D, buf, sizeof(buf));

	(void)cmd; // TODO

	if (n) printf("%s\n", n);
	return (!n) ? -1 : 0;
}


int xb_window_focus(Display *D, const xb_cmd *cmd)
{
	if (cmd->narg != 1) {
		xb_error("Usage: xbot focus <window-name>\n");
		return -1;
	}

	xbot_focus_window(D, cmd->arg[0]);

	return 0;
}
