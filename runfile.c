#include "xbot.h"

#define HELP_RUNFILE "\n"\
 " click <Y> <X> <btn>   Click at given position\n"\
 " key <keys>            \n"\
 " sleep <s>             Wait some seconds\n"\
 " usleep <us>           Wait some microseconds\n"\
 "\n"

/*
 * Read and execute xbot file
 */

static int exec_cmd(Display *, const xb_cmd *cmd);
static void mysleep(const xb_cmd *cmd, int is_sleep);

int xb_runfile(Display *D, const xb_cmd *cmd)
{
	FILE *f;
	char line[512];
	xb_cmd runcmd;

	if (cmd->narg != 1) {
		xb_error("! Usage: run <FILE>\n");
		return -1;
	} else if (!strcmp(cmd->arg[0], "help")) {
		fputs(HELP_RUNFILE, stdout);
		return 0;
	} else if (!(f = fopen(cmd->arg[0], "r"))) {
		xb_error("! run: Failed to open '%s'\n", cmd->arg[0]);
		return -1;
	}

	while (fgets(line, sizeof(line), f)) {

		line[strlen(line)-1] = '\0';
		if (!*line || line[0] == '#')
			continue;
		if (xb_cmd_parse(line, &runcmd))
			return -1;
		else {
			exec_cmd(D, &runcmd);
			xb_cmd_free(&runcmd);
		}
	}

	fclose(f);
	return 0;
}

static int exec_cmd(Display *D, const xb_cmd *cmd)
{
	switch (cmd->id) {
	case CMD_MOUSE_MOVE:
		xb_mouse_move(D, cmd);
		break;
	case CMD_MOUSE_MOVETO:
		xb_mouse_moveto(D, cmd);
		break;
	case CMD_MOUSE_CLICK:
		xb_mouse_click(D, cmd);
		break;
	case CMD_KEY:
		xb_keypress(D, cmd);
		break;
	case CMD_SLEEP:
		mysleep(cmd, 1);
		break;
	case CMD_USLEEP:
		mysleep(cmd, 0);
		break;
	default:
		break;
	}
	return 0;
}

static void mysleep(const xb_cmd *cmd, int is_sleep)
{
	int val;
	if (cmd->narg != 1) {
		xb_error("! Usage: %ssleep <TIME>\n",
			(is_sleep?"":"u"));
		return;
	}
	else if (xb_str2int(cmd->arg[0], &val) != 0) {
		xb_error("! %ssleep: argument must be numeric\n",
			(is_sleep?"":"u"));
		return;
	}

	if (is_sleep)	xb_sleep(val);
	else		xb_usleep(val);
}
