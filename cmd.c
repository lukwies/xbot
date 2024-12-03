#include "cmd.h"

const char *cmd_names[] = {
	"unknown", "help", "capture", "run", "list", "focus",
	"wname", "move", "moveto", "click", "mousepos",
	"key", "pxcolor", "sleep", "usleep"
};

static int parse_cmd_args(const char *, xb_cmd *);
static const char *read_until_char(const char *, char, int, char *, size_t);


const char *xb_cmd_id_tostr(xb_cmd_id id)
{
	return cmd_names[(int)id];
}

xb_cmd_id xb_cmd_id_parse(const char *s)
{
	register int i;
	for (i = 0; i < _CMD_MAX; i++) {
		if (!strcmp(s, cmd_names[i])) {
			return (xb_cmd_id)i;
		}
	}
	return CMD_UNKNOWN;
}

void xb_cmd_zero(xb_cmd *cmd)
{
	cmd->id = CMD_UNKNOWN;
	cmd->narg = 0;
}

int xb_cmd_parse(const char *s, xb_cmd *cmd)
{
	const char *p;
	char cmd_name[32];

	xb_cmd_zero(cmd);

	p = read_until_char(s, ' ', 0, cmd_name, sizeof(cmd_name));
	if (!p) return -1;

	cmd->id  = xb_cmd_id_parse(cmd_name);
	if (cmd->id == CMD_UNKNOWN) {
		xb_error("! Unknown command '%s'\n", cmd_name);
		return -1;
	}
	return (!*p) ? 0 : parse_cmd_args(p, cmd);
}

int xb_cmd_parse_cmdline(int argc, char *argv[], xb_cmd *cmd)
{
	int i;

	if (argc < 2) return -1;
	xb_cmd_zero(cmd);

	cmd->id = xb_cmd_id_parse(argv[1]);

	if (cmd->id == CMD_UNKNOWN) {
		xb_error("! Unknown command '%s'\n", argv[1]);
		return -1;
	}
	for (i = 2; i < argc && cmd->narg < XB_CMD_MAX_ARGS; i++) {
		cmd->arg[cmd->narg++] = xb_strdup(argv[i]);
	}
	return 0;
}

void xb_cmd_free(xb_cmd *cmd)
{
	register size_t i;
	for (i = 0; i < cmd->narg; i++)
		free(cmd->arg[i]);
	cmd->narg = 0;
}

void xb_cmd_print(FILE *f, const xb_cmd *cmd)
{
	register size_t i;
	if (!f) f=stdout;
	printf("%s ", xb_cmd_id_tostr(cmd->id));
	for (i = 0; i < cmd->narg; i++) {
		printf("'%s'%s", cmd->arg[i],
			(i == cmd->narg-1 ? "" : ", "));
	}
	putchar('\n');
}


void xb_cmdlist_init(xb_cmdlist *l, size_t max)
{
	l->cmds = (xb_cmd*)malloc(sizeof(xb_cmd)*max);
	if (!l->cmds) {
		xb_error("! malloc failed\n");
		exit(EXIT_FAILURE);
	}
	l->max = max;
	l->len = 0;
}
void xb_cmdlist_free(xb_cmdlist *l)
{
	free(l->cmds);
	l->max = l->len = 0;
	l->cmds = NULL;
}
void xb_cmdlist_print(FILE *f, const xb_cmdlist *l)
{
	register size_t i;
	for (i = 0; i < l->len; i++)
		xb_cmd_print(f, &l->cmds[i]);
}

/////////// STATIC /////////////////////
static int parse_cmd_args(const char *s, xb_cmd *cmd)
{
	const char *p = s;
	char argbuf[1024];
	char quot;

	while (p && *p) {

		// Argument in quotes ('|") must be handled ...
		if (*p == '"' || *p == '\'') {
			quot = *p;
			p = read_until_char(p+1, quot, 1,
				argbuf, sizeof(argbuf));
			if (!p) {
				xb_error("! parse cmd (%d): Missing "
					"closing %c\n", (int)cmd->id,
					quot);
				return -1;
			}
			cmd->arg[cmd->narg++] = xb_strdup(argbuf);
		}
		else {
			p = read_until_char(p, ' ', 0,
				argbuf, sizeof(argbuf));
			if (!p) {
				xb_error("! parse cmd (%d): failed\n",
					(int)cmd->id);
				return -1;
			}
			cmd->arg[cmd->narg++] = xb_strdup(argbuf);
		}
	}
	return 0;
}

static const char *read_until_char(const char *s, char c,
		int char_must_exist, char *buf, size_t n)
{
	const char *end;
	if (!s || !*s) return NULL;

	end = strchr(s, c);
	if (!end) {
		if (char_must_exist)
			return NULL;
		snprintf(buf, n, "%s", s);
		return "\0";
	}
	if (end-s >= (int)n)
		return NULL;
	memset(buf, 0, n);
	strncpy(buf, s, end-s);
	return end+1;
}
