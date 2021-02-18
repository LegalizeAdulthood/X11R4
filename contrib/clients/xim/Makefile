# phil's makefile
# $Header: /mit/phils/X/RCS/Makefile,v 1.8 89/11/10 18:19:54 phils Exp Locker: phils $

# Look in the environment for $MACHTYPE if not defined by "make"
# MACHINE       = $${MACHTYPE}
# for library and lint - must match Lib/Makefile
BASENAME      = Xim
CC 			  = cc
FLAGS         = -g
MAINLIB       = Lib
LIBDIR        = ./$(MACHINE)lib
INCLUDE       = -I/mit/x11/include
CFLAGS        = $(INCLUDE) $(FLAGS)
BINDIR        = /mit/phils/Bin/$(MACHINE)bin
EXTLIBS	  	  = -L$(LIBDIR) -L/mit/visual/phils/$(MACHINE)lib\
			-l$(BASENAME) -lX11 -lm

PROGRAMS = xxim.out ximtops.out xtoxim.out ximsetroot.out quantizim.out xrainbow.out averagim.out smoothim.out ximtofbm.out fbmtoxim.out rletoxim.out ximtorle.out

EXTHDRS       = /usr/include/X11/X.h \
		/usr/include/X11/Xlib.h \
		/usr/include/X11/Xos.h \
		/usr/include/X11/Xutil.h \
		/usr/include/X11/cursorfont.h \
		/usr/include/fcntl.h \
		/usr/include/math.h \
		/usr/include/pwd.h \
		/usr/include/stdio.h \
		/usr/include/string.h \
		/usr/include/strings.h \
		/usr/include/sys/file.h \
		/usr/include/sys/time.h \
		/usr/include/sys/types.h \
		/usr/include/time.h

HDRS		  = XimHeader.h \
		fbm.h \
		svfb_global.h

LIBS          = $(LIBDIR)/lib$(BASENAME).a

LINTLIBS      = $(LIBDIR)/llib-l$(BASENAME).ln

MAKEFILE      = Makefile

OBJS          = averagim.o \
		fbmtoxim.o \
		quantizim.o \
		rletoxim.o \
		smoothim.o \
		ximsetroot.o \
		ximtofbm.o \
		ximtops.o \
		ximtorle.o \
		xrainbow.o \
		xtoxim.o \
		xxim.o

PRINTER       = linus  # ln03-9-551

RCS_MESSAGE   = automatic chekin

SRCS          = averagim.c \
		fbmtoxim.c \
		quantizim.c \
		rletoxim.c \
		smoothim.c \
		ximsetroot.c \
		ximtofbm.c \
		ximtops.c \
		ximtorle.c \
		xrainbow.c \
		xtoxim.c \
		xxim.c

VPATH         = .

all:	$(LIBS)	$(PROGRAMS)

allnew:		spotless program lint

.SUFFIXES:
.SUFFIXES:	.out .c
.c.out:		$(LIBS)
	$(CC) $(CFLAGS) $< $(EXTLIBS) -o $@

$(LIBS):
	(cd $(LIBDIR); make $(MFLAGS) CC='$(CC)' MACHINE=$(MACHINE)\
	FLAGS='$(FLAGS)' BASENAME=$(BASENAME) library)

$(LINTLIBS):
		(cd $(LIBDIR); make $(MFLAGS) lint)

fbmtoxim.out:
	cc $(CFLAGS) $< -lfbm -lrle $(EXTLIBS) -o $@

ximtofbm.out:
	cc $(CFLAGS) $< -lfbm -lrle $(EXTLIBS) -o $@

rletoxim.out:
	cc $(CFLAGS) $< -lrle $(EXTLIBS) -o $@

ximtorle.out:
	cc $(CFLAGS) $< -lrle $(EXTLIBS) -o $@

ci:  $(HDRS) $(SRCS)
	ci -l -q -m"$(RCS_MESSAGE)" $(HDRS) $(SRCS)

clean:;		@rm -f $(PROGRAMS) $(OBJS) core *.o tags linted *~ *.bak

spotless:	clean
		(cd $(LIBDIR); make $(MFLAGS) clean)
		@rm -f $(PROGRAMS) install

depend:;
	@mkmf -a -F $(MAKEFILE) MACHINE=`machtype` "PROGRAMS=$(PROGRAMS)"
	@makedepend -- $(CFLAGS) -- $(SRCS)

index:;		@ctags -wx $(HDRS) $(SRCS)

install: $(PROGRAMS)
		for pgm in $?; do \
			install -s -m 755 $$pgm $(BINDIR)/`basename $$pgm .out`; \
		done
		@touch install

lint:	$(LINTLIBS)
		@lint -hbuz $(INCLUDE) $(SRCS) $(LINTLIBS) | tee linted

print:;
		@rm -f XIM
		@touch XIM
		@for i in $(SRCS) $(HDRS); do \
        	expand -4 $$i | cat -n | pr -f -h $$i >> XIM; \
            echo $$i; \
        done
		@enscript -2r -G -P$(PRINTER) XIM
		@rm -f XIM

program:	$(PROGRAMS)

saber:;
	/mit/saber2/vaxbin/saber

saber_src:
	#load $(CFLAGS) $(SRCS) $(LIBS) $(EXTLIBS)

saber_obj:
	#load $(OBJS) $(LIBS) $(EXTLIBS)

tags:		$(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

tar:
	tar cvf xim.tar $(HDRS) $(SRCS) $(MAKEFILE) Imakefile *.1 *.5\
	$(MAINLIB)/*.c $(MAINLIB)/*.h $(MAINLIB)/$(MAKEFILE)\
	$(MAINLIB)/Imakefile

# update:		$(BINDIR)/$(PROGRAMS)
#
# $(BINDIR)/$(PROGRAMS): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
# 		@make -f $(MAKEFILE) BINDIR=$(BINDIR) install

# DO NOT DELETE THIS LINE -- make depend depends on it.

averagim.o: averagim.c /usr/include/stdio.h /usr/include/strings.h
averagim.o: XimHeader.h /usr/include/sys/types.h
fbmtoxim.o: fbmtoxim.c /usr/include/stdio.h /usr/include/strings.h
fbmtoxim.o: XimHeader.h /usr/include/sys/types.h fbm.h
quantizim.o: quantizim.c /usr/include/stdio.h /usr/include/strings.h
quantizim.o: XimHeader.h /usr/include/sys/types.h
rletoxim.o: rletoxim.c /usr/include/stdio.h /usr/include/strings.h
rletoxim.o: XimHeader.h /usr/include/sys/types.h svfb_global.h
smoothim.o: smoothim.c /usr/include/stdio.h /usr/include/strings.h
smoothim.o: XimHeader.h /usr/include/sys/types.h
ximsetroot.o: ximsetroot.c /usr/include/stdio.h /mit/x11/include/X11/Xos.h
ximsetroot.o: /usr/include/sys/types.h /usr/include/strings.h
ximsetroot.o: /usr/include/sys/file.h /usr/include/sys/time.h
ximsetroot.o: /usr/include/sys/time.h /mit/x11/include/X11/Xlib.h
ximsetroot.o: /mit/x11/include/X11/X.h /mit/x11/include/X11/Xutil.h
ximsetroot.o: XimHeader.h
ximtofbm.o: ximtofbm.c /usr/include/stdio.h /usr/include/strings.h
ximtofbm.o: XimHeader.h /usr/include/sys/types.h fbm.h
ximtops.o: ximtops.c /usr/include/stdio.h /usr/include/strings.h
ximtops.o: /usr/include/sys/time.h /usr/include/sys/time.h /usr/include/pwd.h
ximtops.o: XimHeader.h /usr/include/sys/types.h
ximtorle.o: ximtorle.c /usr/include/stdio.h /usr/include/strings.h
ximtorle.o: svfb_global.h XimHeader.h /usr/include/sys/types.h
xrainbow.o: xrainbow.c /usr/include/stdio.h /usr/include/strings.h
xrainbow.o: /usr/include/math.h XimHeader.h /usr/include/sys/types.h
xtoxim.o: xtoxim.c /usr/include/stdio.h XimHeader.h /usr/include/sys/types.h
xxim.o: xxim.c /usr/include/stdio.h /mit/x11/include/X11/Xos.h
xxim.o: /usr/include/sys/types.h /usr/include/strings.h
xxim.o: /usr/include/sys/file.h /usr/include/sys/time.h
xxim.o: /usr/include/sys/time.h /mit/x11/include/X11/Xlib.h
xxim.o: /mit/x11/include/X11/X.h /mit/x11/include/X11/Xutil.h
xxim.o: /mit/x11/include/X11/cursorfont.h XimHeader.h
