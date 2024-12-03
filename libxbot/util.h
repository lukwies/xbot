#ifndef LIBXBOT_UTIL_H
#define LIBXBOT_UTIL_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#define xb_error(msg, ...)\
	fprintf(stderr, msg, ##__VA_ARGS__)

#define xb_info(msg, ...)\
	fprintf(stdout, msg, ##__VA_ARGS__)

int  xb_str2int(const char *, int *);
void xb_sleep(int seconds);
void xb_usleep(int microseconds);

char *xb_strdup(const char *);
char *xb_strndup(const char *, size_t n);

#endif
