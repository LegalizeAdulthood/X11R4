#
#	Makefile for library.
#
#
#	version 4.0
#
#

SHELL = /bin/sh
#WNNLIBDIR = /usr/local/lib
WNNINCDIR = /usr/include/wnn
## SX9100
WNNLIBDIR = /usr/lib
MAKE = make
#DEBUG = -DDEBUG
#DEBUG = -DJL_DEBUG

RM = rm -f
MV = mv

TOP = ../../../..
UTILSRC = $(TOP)/util
IMAKESRC = $(UTILSRC)/imake
IRULESRC = $(UTILSRC)/imake.includes
IMAKE = $(IMAKESRC)/imake
IMAKE_DEFINES =
IMAKE_CMD = $(NEWTOP)$(IMAKE) -TImake.tmpl -I$(NEWTOP)$(IRULESRC) \
		-s Makefile $(IMAKE_DEFINES)

ALL =	js.o wnnerrmsg.o jl.o \
	../romkan/rk_main.o	../romkan/rk_read.o \
	../romkan/rk_modread.o	../romkan/rk_bltinfn.o ../romkan/rk_vars.o
#	../etc/strings.o ../etc/sstrings.o ../etc/gethinsi.o 

CFLAGS = -I../include  ${DEFS}

#	for SUN and BSD 4.2/4.3
#DEFS	=
#	no defs needed.

#	for SYSVR2
#DEFS	= -DSYSVR2
#

#	for SX9100
#CFLAGS = -I../include -Zs -g
#DEFS	= -DSX8870 -DINET
#

#	for SX9100 BSD
#CFLAGS = -I../include -20 -f -g
#DEFS	= -DSX9100BSD -DINET
#

#	for NEC EWS4800
#DEFS	= -DNEC -DSYSVR2
#

alllib:	libwnn.a libgetopt.a

libwnn.a: ${ALL}
	ar rv libwnn.a `lorder ${ALL} | tsort `
	@if test -f /usr/bin/ranlib ; then \
		echo /usr/bin/ranlib $@ ;\
		/usr/bin/ranlib $@ ;\
	fi

js.o: js.c ../etc/bdic.c ../etc/pwd.c ../etc/gethinsi.c
	${CC} -c ${CFLAGS}  ${DEBUG} js.c

jl.o: jl.c
	${CC} -c ${CFLAGS}  ${DEBUG} jl.c

wnnerrmsg.o: wnnerrmsg.c
	${CC} -c ${CFLAGS}  wnnerrmsg.c


libgetopt.a: getopt.o
	ar rv libgetopt.a getopt.o
	@if test -f /usr/bin/ranlib ; then \
		echo /usr/bin/ranlib $@ ;\
		/usr/bin/ranlib $@ ;\
	fi

getopt.o: getopt.c
	${CC} -c ${CFLAGS}  getopt.c

kankana: kankana.c libwnn.a
	cc ${CFLAGS} -o kankana kankana.c libwnn.a

../romkan/rk_main.o	../romkan/rk_read.o \
../romkan/rk_modread.o	../romkan/rk_bltinfn.o ../romkan/rk_vars.o:
	cd ../romkan;	$(MAKE) DEFS='$(DEFS)'

Makefile:: Imakefile \
	$(IRULESRC)/Imake.tmpl \
	$(IRULESRC)/Imake.rules \
	$(IRULESRC)/site.def
	-@if [ -f Makefile ]; then \
	echo "$(RM) Makefile.bak; $(MV) Makefile Makefile.bak"; \
	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP)

#
#	clean
#
clean:
	rm -f core a.out *.o *~ *.orig *.rej #*

purge:
	rm -f *.a core a.out *.o *~ *.orig *.rej #*


#
#	Wnn Version 4.0
#
#
#	install jlib
#
install: $(WNNLIBDIR)
	cp libwnn.a		$(WNNLIBDIR)
#	cp libconvkey.a		$(WNNLIBDIR)
	cp ../include/jslib.h	$(WNNINCDIR)
	cp ../include/jllib.h	$(WNNINCDIR)
	cp ../include/rk_spclval.h	$(WNNINCDIR)
	cp ../include/wnnerror.h	$(WNNINCDIR)
	@if test -f /usr/bin/ranlib ; then \
		echo /usr/bin/ranlib $(WNNLIBDIR)/libwnn.a ;\
		/usr/bin/ranlib $(WNNLIBDIR)/libwnn.a ;\
	fi

#
#	dirctory check
#

$(WNNLIBDIR):
	@echo Please mkdir $(WNNLIBDIR)
	@false

#
#	lint
#
lint:
	lint -bnvu $(CFLAGS) jl.c js.c

