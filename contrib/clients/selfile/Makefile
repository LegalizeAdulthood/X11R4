CC = gcc
CC = cc

CFLAGS = -O -DSEL_FILE_XW
CFLAGS = -O

SRCS = SelFile.c Dir.c Path.c Draw.c
OBJS = SelFile.o Dir.o Path.o Draw.o

xdir:			xdir.o $(OBJS)
#	$(CC) $(CFLAGS) xdir.o $(OBJS) -lXw -lXt -lX11 -o $@
	$(CC) $(CFLAGS) xdir.o $(OBJS) -lXaw -lXmu -lXt -lX11 -o $@

lint:
	lint -auxz $(CFLAGS) xdir.c $(SRCS)

clean:
	rm -f xdir *.o core a.out
