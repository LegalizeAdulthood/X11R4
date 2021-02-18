#
#	Makefile for romkan
#
#	 Version 3.0
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

ALLSOURCES =  rk_bltinfn.c rk_main.c rk_modread.c rk_read.c rk_vars.c
ALLOBJECTS =  rk_bltinfn.o rk_main.o rk_modread.o rk_read.o rk_vars.o

CFLAGS =  ${DEFS} -DWNNDEFAULT -I../include	#make with ../Makefile

#	for BSD
#CFLAGS = -DBSD42 -DWNNDEFAULT -I../include

#	for SYSVR2
#CFLAGS = -DSYSVR2 -DWNNDEFAULT -I../include

#	for OMRON SX8870
#CFLAGS = -DSYSVR2 -DOMRON -DWNNDEFAULT -I../include -Zs

LFLAGS = ${DEFS} -DWNNDEFAULT -I../include

HEADERS1 = rk_header.h rk_spclval.h rk_macros.h
HEADERS2 = rk_extvars.h

all: romkanobj

romkan: ${ALLOBJECTS}
	cc ${CFLAGS} -o romkan ${ALLOBJECTS}

romkanobj: ${ALLOBJECTS}

rk_main.o rk_read.o rk_modread.o: ${HEADERS1} ${HEADERS2}

rk_bltinfn.o rk_vars.o: ${HEADERS1}

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
	rm -f core a.out *.o *~ *.orig *.rej #*

purge:
	rm -f romkan core a.out *.o *~ *.orig *.rej #*

lint: ${ALLSOURCES}
	lint -abnhx ${LFLAGS} ${ALLSOURCES}
