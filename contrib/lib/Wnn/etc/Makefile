#
#	Make file for "etc"
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

#CFLAGS = -I../include -g -DINET
#CFLAGS = -I../include -g -DSYSV -DINET -Zs
#CFLAGS = -I../include -g -DINET -20 -f

CFLAGS = -I../include ${DEFS}

allobj = strings.o sstrings.o bdic.o hindo.o gethinsi.o revdic.o  pwd.o bcopy.o xutoj.o dic_atojis.o
#file_stamp.o

all:	${allobj}

xutoj.o:
strings.o:
sstrings.o:
bdic.o:
dic_atojis.o:
hindo.o:
gethinsi.o:
revdic.o:
file_stamp.o:
pwd.o:
bcopy.o:

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
	rm -f a.out core *~ *.orig *.rej #*

purge:
	rm -f ${allobj} a.out core *~ *.orig *.rej #*
