# @(#)Makefile	1.2	ULTRIX	6/29/84
#
# mille bourne game makefile
#
# @(#)Makefile	1.7 (Berkeley) 8/14/83
#
DESTROOT=
XLIB=/x11/lib/X/libX11.a
CDEBUGFLAGS=-O
HEADERS=mille.h
INCLUDES=-I/x11
MFLAGS = "INCLUDES=$(INCLUDES)" "XLIB=$(XLIB)" "CDEBUGFLAGS=$(CDEBUGFLAGS)"
  
CFILES=	comp.c end.c extern.c init.c mille.c misc.c move.c print.c \
	roll.c save.c types.c varpush.c ui.c drawcard.c animate.c 
OBJS=	comp.o end.o extern.o init.o mille.o misc.o move.o print.o \
	roll.o save.o types.o varpush.o ui.o drawcard.o animate.o 
CRL=	
L=	control/control.a cards/cardlib.a $(XLIB) -lm
LIBS=	${CRL} ${L}
DEFS=	
CFLAGS=	$(CDEBUGFLAGS) ${DEFS} $(INCLUDES)
BINDIR=	${DESTROOT}/usr/games

xmille: ${OBJS} ${CRL} control/control.a cards/cardlib.a $(XLIB)
	${CC} -o $@ ${CFLAGS} ${OBJS} ${LIBS}

mille: ${OBJS} ${CRL}
	${CC} ${CFLAGS} -o mille ${OBJS} ${LIBS}

control/control.a: always
	cd control; make $(MFLAGS) control.a

cards/cardlib.a: always
	cd cards; make $(MFLAGS) cardlib.a

always:

install: mille
	install -c -s mille ${BINDIR}

table: table.o extern.o
	${CC} ${CFLAGS} -i -o table table.o extern.o

readdump: readdump.o extern.o varpush.o
	${CC} ${CFLAGS} -i -o readdump readdump.o extern.o varpush.o

ctags:
	ctags ${HEADERS} ${CFILES}
	ed - tags < :ctfix
	sort tags -o tags

lint:
	lint -hxb ${DEFS} ${CFILES} ${L} > lint.out

mille.ar:
	ar ruv mille.ar Makefile tags ${HEADERS} ${CFILES}

tar:
	tar rvf /dev/rmt0 Makefile tags :ctfix ${HEADERS} ${CFILES}

lpr:
	pr Makefile ${HEADERS} ${CFILES} tags | lpr ; lpq

clean:
	rm -f ${OBJS} ${POBJS} core ? a.out mille

clobber: clean
	rm -f *.[ch] Makefile

sccsinfo:
	sccs info

sccsget:
	sccs get SCCS

.DEFAULT:
	sccs get $<

animate.o:	mille.h
animate.o:	ui.h control/control.h color.h
animate.o:	card.h
comp.o:		mille.h
curses_ui.o:	mille.h
curses_ui.o:	ui.h control/control.h color.h
drawcard.o:	mille.h
drawcard.o:	ui.h control/control.h color.h
drawcard.o:	card.h
drawcard.o:	background.h fill
end.o:		mille.h
extern.o:	mille.h
init.o:		mille.h
mille.o:	mille.h
misc.o:		mille.h
move.o:		mille.h
print.o:	mille.h
save.o:		mille.h
table.o:	mille.h
types.o:	mille.h
ui.o:		mille.h
ui.o:		ui.h control/control.h color.h
ui.o:		card.h
varpush.o:	mille.h
