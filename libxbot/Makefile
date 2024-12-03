CC      = gcc
CFLAGS  = -g -Wall -Wextra -lX11
#CFLAGS  = -g -Wall -Wextra -std=c99 -pedantic -D_XOPEN_SOURCE=500
LIBNAME = libxbot.a
OBJ     = mouse.o key.o libxbot.o util.o

$(LIBNAME): $(OBJ)

lib:
	ar rcs $(LIBNAME) $(OBJ)

#test: test.c logger.o
#	$(CC) $(CFLAGS) test.c iface.o


libxbot.o: libxbot.c
	$(CC) $(CFLAGS) -c libxbot.c
mouse.o: mouse.c
	$(CC) $(CFLAGS) -c mouse.c
key.o: key.c
	$(CC) $(CFLAGS) -c key.c
util.o: util.c
	$(CC) $(CFLAGS) -c util.c
clean:
	rm -f $(LIBNAME) *.o
