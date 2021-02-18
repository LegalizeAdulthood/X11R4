#
#	Wnn Version 4.0
#
SHELL= /bin/sh

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

ALLSOURCES = cvt_key.c cvt_read.c
ALLOBJECTS = cvt_key.o cvt_read.o
HEADERS = cvt_head.h

CFLAGS = ${DEFS} -I../include -DWNNDEFAULT	#Make with ../Makefile

#	for SUN
#CFLAGS = -DSUN -I../include -DWNNDEFAULT

#	for BSD 4.2/4.3
#CFLAGS = -DBSD42 -I../include -DWNNDEFAULT

#	for SYSVR2
#CFLAGS = -DSYSVR2 -I../include -DWNNDEFAULT

#	for OMRON SX8870
#CFLAGS = -DSX8870 -I../include -DWNNDEFAULT -Zs -O

../jlib/libconvkey.a: ${ALLOBJECTS}
	ar vr libconvkey.a ${ALLOBJECTS}
	mv libconvkey.a ../jlib
	@if test -f /usr/bin/ranlib ; then \
		echo /usr/bin/ranlib $@ ;\
		/usr/bin/ranlib $@ ;\
	fi

${ALLOBJECTS}: ${HEADERS}

Makefile:: Imakefile \
	$(IRULESRC)/Imake.tmpl \
	$(IRULESRC)/Imake.rules \
	$(IRULESRC)/site.def
	-@if [ -f Makefile ]; then \
	echo "$(RM) Makefile.bak; $(MV) Makefile Makefile.bak"; \
	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP)

clean:
	rm -f  a.out core *.o *.a *~ *.orig *.rej #*

purge:
	rm -f ${ALLOBJECTS} ../jlib/libconvkey.a a.out core *.o *~ *.orig *.rej #*

lint: ${ALLSOURCES}
	lint -abnhx ${CFLAGS} ${ALLSOURCES}
