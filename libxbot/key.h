#ifndef LIBXBOT_KEY_H
#define LIBXBOT_KEY_H 1

#include "libxbot.h"

/*
 * Common keys:
 *
 *   All ASCII characters [0-9][a-z][A-Z][...]
 *
 * Special keys:
 *
 *   [alt] [alt-gr]
 *   [backspace]
 *   [ctrl] [ctrl-l] [ctrl-r]
 *   [del]
 *   [down]
 *   [end]
 *   [esc]
 *   [F1] [F2] ... [F12]
 *   [home]
 *   [left]
 *   [page-up] [page-down]
 *   [paste]
 *   [pause]
 *   [print]
 *   [return]
 *   [right]
 *   [shift] [shift-l] [shift-r] [shift-lock]
 *   [space] ???	
 *   [tab]
 *   [up]
 *
 * Keys can be concatenated with '+'
 *
 *   [shift]+[ctrl]+A
 *   [alt]+[F4]
 */

/* Keyvalue for [alt-gr] ([alt-r]) */
#define XK_AltGr 0xFE03


/* These flags are used to detect if a special key is pressed.
 * Used for value of XKeyEvent->state. */
enum {
	XBOT_IS_SHIFT = 0x01,
	XBOT_IS_CTRL  = 0x04,
	XBOT_IS_ALT   = 0x08,
	XBOT_IS_ALTGR = 0x80
};

/* This is the central key definition.
 * It hold a keycode (c) and a state for storing if key
 * is combined with special key(s).
 */
typedef struct xbot_key
{
	int k;		// Key
	int state;	// State (See enum)

} xbot_key;


/* Initialize xbot_key. */
void xbot_key_zero(xbot_key *);

/* Initialize xbot_key by x11-keycode. This will resolve keys
 * like 'H' (using shift) or '|' (using alt-gr). */
void xbot_key_set(xbot_key *, int xkey);

#define xbot_key_isshift(k) ((k)->state & XBOT_IS_SHIFT)
#define xbot_key_isctrl(k)  ((k)->state & XBOT_IS_CTRL)
#define xbot_key_isalt(k)   ((k)->state & XBOT_IS_ALT)
#define xbot_key_isaltgr(k) ((k)->state & XBOT_IS_ALTGR)


/* Parse string with multiple (special) keys to key array.
 * The string might be formatted as follows:
 *
 *  'Hello World!'
 *  'Hello[tab]World!'
 *  '[ctrl]+[shift]+C'
 *  '[alt]+[F4]'
 *
 * @param   str		The key string
 * @param   keys	Array with x11 keycodes
 * @param   max_keys    Max length of array
 *
 * @return  Number of keys parsed on success, 0 on error
 */
size_t xbot_key_parse(const char *str, xbot_key *keys, size_t max_keys);


/* Send the given keys as fake-keys to the default root window of
 * the x11 server.
 * @return  Zero on success, -1 on error
 */
int xbot_key_send(Display *, const xbot_key *keys, size_t len);


/* Print given keys as formatted line.
 * Format "key <k1><k2>...<kn>"
 */
void xbot_key_print(FILE *, const xbot_key *, size_t n);


/* Returns name of given special key. See description on top
 * of this page for what a special key is.
 */
const char *xbot_special_key_name(int key);

#endif
