#ifndef XBOT_CMD_H
#define XBOT_CMD_H 1

#include "libxbot/util.h"

#define XB_CMD_MAX_ARGS 8

typedef enum {
	CMD_UNKNOWN = 0,	// Unknown command
	CMD_HELP,		// Show help

	CMD_CAPTURE,		// Start capturing
	CMD_RUN,		// Run capture-file

	CMD_LIST,		// List all windows
	CMD_FOCUS,		// Set focus window
	CMD_WNAME,		// Get window name

	CMD_MOUSE_MOVE,		// Move mouse relative
	CMD_MOUSE_MOVETO,	// Move mouse absolute
	CMD_MOUSE_CLICK,	// Click mouse button
	CMD_MOUSE_POS,		// Get mouse position

	CMD_KEY,		// Send fake key

	CMD_PX_COLOR,		// Get pixel color at mouse position
	CMD_SLEEP,		// Sleep process for some seconds
	CMD_USLEEP,		// Sleep process for some micro seconds

	_CMD_MAX
} xb_cmd_id;

/* Convert cmd id to string and vice versa. */
xb_cmd_id   xb_cmd_id_parse(const char *);
const char *xb_cmd_id_tostr(xb_cmd_id);


/* Command context */
typedef struct xb_cmd
{
	xb_cmd_id id;
	char     *arg[XB_CMD_MAX_ARGS];
	size_t    narg;

} xb_cmd;

/* Init cmd */
void xb_cmd_zero(xb_cmd *);

/* Parses given string to command.
 * NOTE: command context will contain allocated memory!!
 * @return  Zero on success, -1 on error
 */
int xb_cmd_parse(const char *s, xb_cmd *);

/* Parse command context from arguments, passed by command line.
 * NOTE: command context will contain allocated memory!!
 * @return  Zero on success, -1 on error
 */
int xb_cmd_parse_cmdline(int argc, char *argv[], xb_cmd *);


/* Free memory*/
void xb_cmd_free(xb_cmd *);


/* Print command */
void xb_cmd_print(FILE *, const xb_cmd *);

/* Command id to string */
const char *xb_cmd_id_str(xb_cmd_id);
xb_cmd_id xb_cmd_id_from_str(const char *);


/* Array with commands */
typedef struct xb_cmdlist
{
	xb_cmd *cmds;
	size_t  len;
	size_t  max;

} xb_cmdlist;

void xb_cmdlist_init(xb_cmdlist *, size_t max);
void xb_cmdlist_free(xb_cmdlist *);
void xb_cmdlist_print(FILE *, const xb_cmdlist *);


#endif
