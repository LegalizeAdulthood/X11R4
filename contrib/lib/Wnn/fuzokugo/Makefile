#
#	Wnn Version 4.0
#
#
#	Makefile for fuzokugo
#
# Comment the following BINDIR and LIBDIR out when use ../Makefile
#
SHELL= /bin/sh

#BINDIR = /usr/local/bin/Wnn4

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

CFLAGS = -I../include ${DEFS}

all: atof

atof.o: atof.c

atof: atof.o
	cc ${CFLGAS} -o atof atof.o ../etc/gethinsi.o ../etc/bdic.o ../etc/strings.o ../etc/sstrings.o ../etc/pwd.o ../etc/dic_atojis.o

install: $(BINDIR)
	cp atof $(BINDIR)
	strip $(BINDIR)/atof

#
#	dirctory check
#
$(BINDIR):
	@echo Please mkdir $(BINDIR)
	@false

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
	rm -f a.out core *.o *~ *.orig *.rej #*

purge:
	rm -f atof a.out core *.o *~ *.orig *.rej #*

lint:
	lint -abnhx *.c




