#
#	Makefile for library.
#
#
#	version 4.0
#
#
SHELL = /bin/sh
##	for Not SX9100
#WNNLIBDIR = /usr/local/lib
WNNINCDIR = /usr/include/wnn
##
##	for SX9100	##
WNNLIBDIR = /usr/lib
##

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

ALL =	jlv3.o gethinsi.o
#strings.o sstrings.o

#	../jlib/js.o ../jlib/wnnerrormsg.o ../jlib/jl.o \
#	../romkan/rk_main.o	../romkan/rk_read.o \
#	../romkan/rk_modread.o	../romkan/rk_bltinfn.o ../romkan/rk_vars.o



##	for SUN
#CFLAGS = -I../include
#DEFS	= -DINET -DSUN
##

##	for SX9100
#CFLAGS = -I../include -Zs -g
#DEFS	= -DINET -DSX8870
##

CFLAGS = -I../include

alllib:	libjd.a

libjd.a: ${ALL} ../jlib/libwnn.a
	cp ../jlib/libwnn.a libjd.a
	ar rv libjd.a `lorder ${ALL} | tsort `
	@if test -f /usr/bin/ranlib ; then \
		echo /usr/bin/ranlib $@ ;\
		/usr/bin/ranlib $@ ;\
	fi


jlv3.o: jlv3.c jlib.h
	cc -c ${CFLAGS} ${DEFS} jlv3.c

gethinsi.o: ../etc/gethinsi.c
	cc -c ${CFLAGS} ${DEFS} ../etc/gethinsi.c

#strings.o: ../etc/strings.c
#	cc -c ${CFLAGS} ${DEFS} strings.c
#
#sstrings.o: ../etc/sstrings.c
#	cc -c ${CFLAGS} ${DEFS} sstrings.c

libwnn.a: ../jlib/libwnn.a
	cd ../jlib; $(MAKE) DEFS='$(DEFS)'

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
#	install jlib
#
install: $(WNNLIBDIR)
	cp libjd.a			$(WNNLIBDIR)
	cp ../include/jlib.h		$(WNNINCDIR)
	cp ../include/rk_spclval.h	$(WNNINCDIR)
	cp ../include/wnnerror.h	$(WNNINCDIR)
	@if test -f /usr/bin/ranlib ; then \
		echo /usr/bin/ranlib $(WNNLIBDIR)/libjd.a ;\
		/usr/bin/ranlib $(WNNLIBDIR)/libjd.a ;\
	fi

#
#	clean
#
clean:
	rm -f core a.out *.o *~ *.orig *.rej #*

purge:
	rm -f *.a core a.out *.o *~ *.orig *.rej #*


#
#	dirctory check
#

$(WNNLIBDIR):
	@echo Please mkdir $(WNNLIBDIR)
	@false
