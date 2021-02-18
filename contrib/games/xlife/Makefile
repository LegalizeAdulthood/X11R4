CFLAGS = -O -fomit-frame-pointer -fstrength-reduce -finline-functions -fcombine-regs -fforce-addr

CFLAGS+{pmax_3} = -O3 

CC = gcc
CC+{pmax_3} = cc

.SUFFIXES: .c .o .u .h

OBJS =  button.o\
	data.o\
	expose.o\
	file.o\
	generate.o\
	help.o\
	key.o\
	keyin.o\
	lifeutil.o\
	main.o\
	motion.o\
	resize.o\
	utils.o

OBJS+{pmax_3} = button.u\
	data.u\
	expose.u\
	file.u\
	generate.u\
	help.u\
	key.u\
	keyin.u\
	lifeutil.u\
	main.u\
	motion.u\
	resize.u\
	utils.u

.c.u :
	cc $(CFLAGS) -j $*.c

.c.o : 
	gcc $(CFLAGS) -c $*.c

xlife : ${OBJS}
	$(CC) $(CFLAGS) -o xlife $(OBJS) -lX11

install : xlife
	cp xlife $(DESTDIR)bin/.unpriv/@sys
	strip $(DESTDIR)bin/.unpriv/@sys/xlife

data.o : data.h defs.h struct.h
data.u : data.h defs.h struct.h
