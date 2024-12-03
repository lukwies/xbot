#include "key.h"

#define isprintable(k) ((k)>=' '&&(k)<='~')

/* Special keys */
struct special_key {
	const char *s;
	int         c;
} special_keys[] = {
	{"space",	0x20},
	{"return",	XK_Return},
	{"backspace",	XK_BackSpace},
	{"tab",		XK_Tab},

	{"up",		XK_Up},
	{"down",	XK_Down},
	{"left",	XK_Left},
	{"right",	XK_Right},

	{"shift",	XK_Shift_L},
	{"ctrl",	XK_Control_L},
	{"alt",		XK_Alt_L},
	{"alt-gr",	XK_AltGr},
	{"del",		XK_Delete},
	{"esc",		XK_Escape},

	{"page-down",	XK_Page_Down},
	{"page-up",	XK_Page_Up},

	{"shift-l",	XK_Shift_L},
	{"shift-r",	XK_Shift_R},
	{"ctrl-l",	XK_Control_L},
	{"ctrl-r",	XK_Control_R},

	{"caps-lock",	XK_Caps_Lock},
	{"clear",	XK_Clear},
	{"end",		XK_End},
	{"F1",		XK_F1},
	{"F2",		XK_F2},
	{"F3",		XK_F3},
	{"F4",		XK_F4},
	{"F5",		XK_F5},
	{"F6",		XK_F6},
	{"F7",		XK_F7},
	{"F8",		XK_F8},
	{"F9",		XK_F9},
	{"F10",		XK_F10},
	{"F11",		XK_F11},
	{"F12",		XK_F12},
	{"home",	XK_Home},
	{"paste",	XK_Insert},
	{"pause",	XK_Pause},
	{"print",	XK_Print}
//	{"shift-lock",	XK_Shift_Lock},
};
size_t n_special_keys = sizeof(special_keys)/sizeof(special_keys[0]);

static int special_key_code(const char *, size_t);
static const char *parse_next_key(const char *, int *);
static XKeyEvent create_key_event(Display *, Window, Window);
static int is_shift_char(int c);
static int is_altgr_char(int c);
static void print_key(FILE *, const xbot_key *);
static void print_keycode(FILE *, int);
static void printn(const char *pre, const char *s, size_t n, const char *suf);

void xbot_key_zero(xbot_key *k)
{
	k->k     = 0x00;
	k->state = 0x00;
}

void xbot_key_set(xbot_key *k, int xkey)
{
	xbot_key_zero(k);
	if (is_shift_char(xkey))
		k->state |= XBOT_IS_SHIFT;
	else if (is_altgr_char(xkey))
		k->state |= XBOT_IS_ALTGR;
	k->k = xkey;
}

size_t xbot_key_parse(const char *str, xbot_key *keys, size_t max)
{
	const char *p = str;
	size_t      nkeys = 0;
	int         kc = 0; // Keycode
	xbot_key   *key;

	while (*p && nkeys < max) {

		key = &keys[nkeys];
		xbot_key_zero(key);

		while (*p) {

//			printn("  parse shortcut [", p, 3, "]\n");

			p = parse_next_key(p, &kc);
			if (p == NULL) return 0;

			key->k = kc;

			if (kc == XK_Shift_L || kc == XK_Shift_R)
				key->state |= XBOT_IS_SHIFT;
			else if (kc == XK_Control_L || kc == XK_Control_R)
				key->state |= XBOT_IS_CTRL;
			else if (kc == XK_Alt_L)
				key->state |= XBOT_IS_ALT;
			else if (kc == XK_AltGr)
				key->state |= XBOT_IS_ALTGR;
			else {
				if (is_shift_char(kc))
					key->state |= XBOT_IS_SHIFT;
				else if (is_altgr_char(kc))
					key->state |= XBOT_IS_ALTGR;
			}

			// If stringkey does not end with '+' we
			// quit here
			if (*p != '+') break;
			else p++;
		}
		nkeys++;
	}
	return nkeys;
}

int xbot_key_send(Display *D, const xbot_key *keys, size_t len)
{
	register size_t i;
	const xbot_key *k;
	XKeyEvent ev;
	Window    wfocus;
	int       revert;

	Window root = XDefaultRootWindow(D);
	XGetInputFocus(D, &wfocus, &revert);

	ev = create_key_event(D, wfocus, root);

	for (i = 0; i < len; i++) {
		k = &keys[i];

		// Press all keycodes in k
		ev.type  = KeyPress;
		ev.state = k->state;
		ev.time = CurrentTime;
		ev.keycode = XKeysymToKeycode(D, k->k);
		XSendEvent(ev.display, ev.window, True,
				KeyPressMask, (XEvent*)&ev);

		// Release all keycodes in k
		ev.type = KeyRelease;
		XSendEvent(ev.display, ev.window, True,
				KeyPressMask, (XEvent*)&ev);
	}
	return 0;
}

void xbot_key_print(FILE *f, const xbot_key *keys, size_t n)
{
	register size_t i;
	if (!f) f=stdout;

	fputs("key ", f);
        for (i = 0; i < n; i++) {
		print_key(f, &keys[i]);
        }
	fputc('\n', f);
}



const char *xbot_special_key_name(int key)
{
	register size_t i;
	for (i = 0; i < n_special_keys; i++) {
		if (key == special_keys[i].c)
			return special_keys[i].s;
	}
	return "???";
}

/////////////////////// STATIC /////////////////////////////

/* Parses the next (single) keycode. */
static const char *parse_next_key(const char *s, int *keycode)
{
	int code;
	const char *end;

	// If string begins with '[' we check if has a trailing
	// special code.
	if (*s == '[') {
		end = strchr(s, ']');
		if (!end) {
			*keycode = (int)*s;
			return s+1;
		}
		s++;

		// Try to get special-code from string
		// between "[...]"
		code = special_key_code(s, end-s);
		if (code == -1) {
			printn("! Invalid key [", s, end-s, "]\n");
			return NULL;
		}
		*keycode = code;
		return end+1;
	}
	*keycode = (int)*s;
	return s+1;
}

static int special_key_code(const char *s, size_t len)
{
	register size_t i=0;
	if (!*s || !len) return -1;
	for (i = 0; i < n_special_keys; i++)
		if (!strncmp(special_keys[i].s, s, len))
			return special_keys[i].c;
	return -1;
}

// Function to create a keyboard event
static XKeyEvent create_key_event(Display *D, Window focus, Window root)
{
	XKeyEvent event;
	event.display     = D;
	event.window      = focus;
	event.root        = root;
	event.subwindow   = None;
	event.time        = CurrentTime;
	event.x           = 1;
	event.y           = 1;
	event.x_root      = 1;
	event.y_root      = 1;
	event.same_screen = True;
//	event.keycode     = XKeysymToKeycode(D, keycode);
	event.state       = 0;
//	event.type = KeyPress;
	return event;
}

static int is_shift_char(int c)
{
	const char *p;
	static const char *shift_chars = {"!\"$%&/()=?*'_:;>"};

	if (c >= 'A' && c <= 'Z')
		return 1;

	for (p = shift_chars; *p; p++)
		if (*p == (char)c)
			return 1;
	return 0;
}
static int is_altgr_char(int c)
{
	const char *p;
	static const char *altgr_chars = {"{[]}\\~|"};
	for (p = altgr_chars; *p; p++)
		if (*p == (char)c)
			return 1;
	return 0;
}


static void print_key(FILE *f, const xbot_key *k)
{
        int n=0;
        if (!f) f=stdout;

	// Print keys combined with shift or altgr.
	if ((k->state == XBOT_IS_SHIFT || k->state == XBOT_IS_ALTGR)
			 && isprintable(k->k)) {
		print_keycode(f, k->k);
		return;
	}

	// Print shortcut prefixes
        if (k->state & XBOT_IS_CTRL) {
                fprintf(f, "[ctrl]");
                n++;
        }
        if (k->state & XBOT_IS_ALT) {
                fprintf(f, "%s[alt]", (n++ ? "+" : ""));
        }
        if (k->state & XBOT_IS_ALTGR) {
                fprintf(f, "%s[alt-gr]", (n++ ? "+" : ""));
        }
        if (k->state & XBOT_IS_SHIFT) {
                fprintf(f, "%s[shift]", (n++ ? "+" : ""));
        }
        if (n) fputc('+', f);

	print_keycode(f, k->k);
}

static void print_keycode(FILE *f, int code)
{
	if (isprintable(code))
		fprintf(f, "%c", code);
	else	fprintf(f, "[%s]", xbot_special_key_name(code));
}


static void printn(const char *pre, const char *s, size_t n, const char *suf)
{
	register size_t i;
	if (pre) fputs(pre, stdout);
	for (i = 0; i < n && s[i]; i++)
		putchar(s[i]);
	if (suf) fputs(suf, stdout);
}

//#define TEST_KEY
#ifdef TEST_KEY
int main(void)
{
	const char *ks[] = {
		"Hello World[return]",
		"[alt-gr]+[shift]+A",
		"[alt]+l",
		"[shift]+A"
	};
	xbot_key k[128];
	size_t i, len;

	for (i = 0; i < 4; i++) {
		printf("input '%s'\n", ks[i]);
		len = xbot_key_parse(ks[i], k, 128);
		if (len > 0)
			xbot_key_print(NULL, k, len);
	}
	return 0;

}
#endif
