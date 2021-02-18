##################################################################
#
#   X11 xscope - spy on characters to and from an X11 server
#
#	Author: JPeterson, MCC
#	(c) Copyright MCC 1988
#
#
#
CFLAGS = -g
SRCS = prtype.c print11.c table11.c decode11.c server.c fd.c common.c scope.c
OBJS = scope.o common.o fd.o server.o decode11.o table11.o print11.o prtype.o

ALL = xscope
all:	${ALL}

##################################################################
#
#   compilation of xscope
#

xscope:	${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o xscope

scope.o:  scope.c scope.h
	${CC} ${CFLAGS} scope.c -c


##################################################################
#
#   compilation of common support code
#
common.o: common.c scope.h
	${CC} ${CFLAGS} common.c -c

fd.o: fd.c fd.h scope.h
	${CC} ${CFLAGS} fd.c -c

server.o: server.c scope.h x11.h
	${CC} ${CFLAGS} server.c -c

decode11.o: decode11.c scope.h x11.h
	${CC} ${CFLAGS} decode11.c -c

table11.o: table11.c scope.h x11.h
	${CC} ${CFLAGS} table11.c -c

print11.o: print11.c scope.h x11.h
	${CC} ${CFLAGS} print11.c -c

prtype.o: prtype.c scope.h x11.h
	${CC} ${CFLAGS} prtype.c -c

scope.h:  fd.h

##################################################################
#
#   clean
#
clean:
	rm -f *.o ${ALL} *~

lint:	${SRCS}
	lint ${SRCS}

config:
	dvc -c Makefile.proto

##################################################################
#
#   installation
#
BINDIR = /usr/project/deli/working/bin

install: ${ALL}
	install xscope       ${BINDIR}

