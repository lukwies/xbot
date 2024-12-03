#include "util.h"

int xb_str2int(const char *s, int *val)
{
	if (!strncmp(s, "0x", 2)) {
		return (sscanf(s, "0x%x", val) == 1) ? 0 : -1;
	}
	return (sscanf(s, "%d", val) == 1) ? 0 : -1;
}

static void _sleep(int sec, int usec)
{
	struct timeval tv = {sec, usec};
        (void)select(1, NULL, NULL, NULL, &tv);
}
void xb_sleep(int s)
{
	_sleep(s, 0);
}
void xb_usleep(int ms)
{
	_sleep(0, ms);
}

char *xb_strdup(const char *s)
{
	size_t n = s ? strlen(s) : 0;
	return (!n) ? NULL : xb_strndup(s, n);
}

char *xb_strndup(const char *s, size_t n)
{
	char *x = malloc(n+1);
	memcpy(x, s, n);
	x[n] = '\0';
	return x;
}
