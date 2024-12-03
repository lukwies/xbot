CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -lX11
PROG   = xbot
OBJ    = libxbot/*.o\
         capture.o cmd.o keypress.o mouse.o runfile.o window.o


$(PROG): main.c $(OBJ)
	$(CC) $(CFLAGS) $(CLIBS) -o $(PROG)\
         main.c $(OBJ)

cmd.o: cmd.c
	$(CC) $(CFLAGS) -c cmd.c
capture.o: capture.c
	$(CC) $(CFLAGS) -c capture.c
keypress.o: keypress.c
	$(CC) $(CFLAGS) -c keypress.c
mouse.o: mouse.c
	$(CC) $(CFLAGS) -c mouse.c
runfile.o: runfile.c
	$(CC) $(CFLAGS) -c runfile.c
window.o: window.c
	$(CC) $(CFLAGS) -c window.c


.PHONY: clean
clean:
	 rm -rf *.o $(PROG)

