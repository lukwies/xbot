#include <signal.h>

#include "libxbot/libxbot.h"
#include "libxbot/mouse.h"
#include "libxbot/key.h"
#include "libxbot/util.h"
#include "cmd.h"

/* How many seconds to wait before start capturing */
#define WAIT_BEFORE_CAPTURE 3

/* These (special) keys are ignored while capturing, since
 * we can use XKeyEvent.state to detect them... */
#define is_ignore_key(k) (\
        (k) == XK_Shift_L || (k) == XK_Shift_R || \
        (k) == XK_Control_L || (k) == XK_Control_R || \
        (k) == XK_Alt_L || (k) == XK_AltGr \
        )

/* Returns true if c is a printable ASCII char. */
#define isprintable(c) ((c)>=' '&&(c)<='~')

static FILE *prepare_capturing(const xb_cmd *);
static void  print_key(FILE *, int key, int state);
static void  sig_handler(int);

#define MAX_TEXTBUF_SIZE 256
/* The textbuffer holds all last pressed ascii charakters.
 * It will be printed and flushed after getting a special key. */
static struct textbuffer {
	char   b[MAX_TEXTBUF_SIZE];
	size_t n;
} textbuf;

static void textbuf_zero(void);
static void textbuf_add(char);
static void textbuf_print(FILE *);

static int  done = 0;

int xb_capture(Display *D, const xb_cmd *cmd)
{
	Window root, curFocus;
	KeySym ks;
	XComposeStatus comp;
	int key, revert;
	char wname[256];
	FILE *f = stdout;
	f = prepare_capturing(cmd);

	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);

	root = DefaultRootWindow(D);
	XGetInputFocus (D, &curFocus, &revert);
	XSelectInput(D, curFocus, KeyPressMask|KeyReleaseMask|FocusChangeMask);
	textbuf_zero();
	done = 0;

	while (!done) {
		XEvent     ev;
		XKeyEvent *kev;

		XGrabPointer(D, root, True,
			PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
			GrabModeAsync, GrabModeAsync,
			None, None, CurrentTime);
		XNextEvent(D, &ev);

		switch (ev.type) {
		// Change focus window
		case FocusOut:
			if (curFocus != root)
				XSelectInput(D, curFocus, 0);
			XGetInputFocus (D, &curFocus, &revert);
			if (curFocus == PointerRoot)
				curFocus = root;
			XSelectInput(D, curFocus,
				KeyPressMask|KeyReleaseMask|FocusChangeMask);
			fprintf(f, "# Changed focus to (win=%lu) '%s'\n",
				curFocus,
				xbot_window_name(D, curFocus, wname, sizeof(wname)));
			break;

		// Key event
		case KeyPress:
			kev = (XKeyEvent*)&ev;
			XLookupString(&ev.xkey, NULL, 0, &ks, &comp);
			key = (int)ks;

			if (is_ignore_key(key))
				break;
			if (kev->state == XBOT_IS_SHIFT ||
			    kev->state == XBOT_IS_ALTGR) {
				if (isprintable(key))
					textbuf_add(key);
				else	print_key(f, key, kev->state);
			}
			else if (kev->state != 0) {
				print_key(f, key, kev->state);
			}
			else {
				if (isprintable(key))
					textbuf_add(key);
				else	print_key(f, key, kev->state);
			}
			break;

		// Mouse event
		case ButtonPress:

			// Print textbuf if filled
			textbuf_print(f);

			fprintf(f, "click %d %d %d\n",
				ev.xbutton.y_root,
				ev.xbutton.x_root,
				ev.xbutton.button);

			// We have to ungrab mousepointer and send the mouse
			// event again since otherwise the mouse click will
			// have no effect to other windows while capturing.
			XUngrabPointer(D, CurrentTime);
			xbot_mouse_click(D, ev.xbutton.button);
			break;
        	}
    	}
	XUngrabPointer(D, CurrentTime);
	printf("\rCapturing done\n");
	if (f != stdout) fclose(f);
	return 0;
}

/* Quit capturing on any signal caught. */
static void sig_handler(int s)
{
	done = True;
	signal(s, sig_handler);
}

static void textbuf_zero(void)
{
	textbuf.b[0] = '\0';
	textbuf.n = 0;
}
static void textbuf_add(char c)
{
	if (textbuf.n < MAX_TEXTBUF_SIZE-1) {
		textbuf.b[textbuf.n++] = c;
		textbuf.b[textbuf.n]   = '\0';
	}
}
static void textbuf_print(FILE *f)
{
	if (textbuf.n > 0) {
		fputs("key ", f);
		fputs(textbuf.b, f);
		fputc('\n', f);
		textbuf_zero();
	}
}

/* Wait WAIT_BEFORE_CAPTURE seconds and open capture file
 * if set in cmd->arg[0].
 * @return  Capture stream descriptor (stdout if no file set)
 */
static FILE *prepare_capturing(const xb_cmd *cmd)
{
	int   i;
	FILE *f = stdout;

	for (i = WAIT_BEFORE_CAPTURE; i > 0; i--) {
		fprintf(stdout, "\rStart capturing in %d sec ...  ", i);
		fflush(stdout);
		xb_sleep(1);
	}
	fprintf(stdout, "\rCapturing started, press ^C to quit\n");

	if (cmd->narg == 1) {
		f = fopen(cmd->arg[0], "w");
		if (!f) {
			fprintf(stderr, "! Failed to open capture file '%s', "
				"using stdout ...\n",
				cmd->arg[0]);
			return stdout;
		}
		printf(". Using capture file '%s'\n", cmd->arg[0]);
	}
	return f;
}

/* Print key with given state */
static void print_key(FILE *f, int key, int state)
{
        int n=0;
        if (!f) f=stdout;

        textbuf_print(f);
        fputs("key ", f);

        if (state & XBOT_IS_CTRL) {
                fprintf(f, "[ctrl]");
                n++;
        }
        if (state & XBOT_IS_ALT) {
                fprintf(f, "%s[alt]", (n++ ? "+" : ""));
        }
        if (state & XBOT_IS_ALTGR) {
                fprintf(f, "%s[alt-gr]", (n++ ? "+" : ""));
        }
        if (state & XBOT_IS_SHIFT) {
                fprintf(f, "%s[shift]", (n++ ? "+" : ""));
        }
        if (n) fputc('+', f);

        if (isprintable(key))
                fprintf(f, "%c\n", key);
        else    fprintf(f, "[%s]\n", xbot_special_key_name(key));
}

