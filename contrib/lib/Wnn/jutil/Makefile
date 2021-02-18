#
#	makefile for jisho utility programs
#
#	Version 4.0
#
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

CFLAGS = ${DEFS} -I../include -DINET		#Make with ../Makefile

#CFLAGS = -DBSD42 -O -DINET		#BSD 4.2/4.3
#CFLAGS = -DSYSVR2 -O -DINET		#SYS V R2
#CFLAGS = -I../include -g -Zs -DSX8870
#CFLAGS = -I../include -g -20 -f


srcs = sort.c ujisf.c utilbdic.c dic_head.c atod.c dtoa.c wnnstat.c wnnkill.c wnntouch.c wdreg.c wddel.c

allcmds = dicsort oldatonewa atod dtoa wnnstat wnnkill wnntouch wdreg wddel
all: ${allcmds}

../jlib/libwnn.a: ../jlib/jl.c ../jlib/js.c ../jlib/wnnerrmsg.c
	cd ../jlib; ${MAKE} DEFS='$(DEFS)' GETOPTLIB='$(GLIB)'

atod:	atod.o  ujisf.o utilbdic.o dic_head.o strings.o sstrings.o revdic.o hindo.o bdic.o gethinsi.o pwd.o bcopy.o dic_atojis.o atorev.o
	cc ${CFLAGS} -o atod   ujisf.o atod.o utilbdic.o dic_head.o \
	strings.o sstrings.o revdic.o atorev.o \
	 hindo.o bdic.o gethinsi.o pwd.o bcopy.o dic_atojis.o

dtoa:	dtoa.o  ujisf.o utilbdic.o dic_head.o  strings.o sstrings.o revdic.o hindo.o bdic.o gethinsi.o pwd.o bcopy.o dic_atojis.o
	cc ${CFLAGS} -o dtoa  ujisf.o dtoa.o utilbdic.o  dic_head.o \
	strings.o sstrings.o revdic.o \
	 hindo.o bdic.o gethinsi.o pwd.o bcopy.o dic_atojis.o

wnntouch: wnntouch.c bdic.o pwd.o gethinsi.o sstrings.o strings.o bcopy.o dic_atojis.o
	cc ${CFLAGS} -o wnntouch wnntouch.c bdic.o pwd.o gethinsi.o sstrings.o strings.o bcopy.o dic_atojis.o

wnnstat: wnnstat.c xutoj.o strings.o sstrings.o bcopy.o ../jlib/libwnn.a
	cc ${CFLAGS} -o wnnstat wnnstat.c xutoj.o strings.o sstrings.o bcopy.o ../jlib/libwnn.a

wnnkill: wnnkill.c xutoj.o strings.o sstrings.o ../jlib/libwnn.a
	cc ${CFLAGS} -o wnnkill wnnkill.c xutoj.o strings.o sstrings.o ../jlib/libwnn.a

dicsort:	sort.o strings.o sstrings.o ujisf.o gethinsi.o  bdic.o pwd.o bcopy.o dic_atojis.o
	cc ${CFLAGS} -o dicsort strings.o sstrings.o ujisf.o sort.o gethinsi.o  bdic.o pwd.o bcopy.o dic_atojis.o

oldatonewa:	oldatonewa.c strings.o sstrings.o  bdic.o gethinsi.o pwd.o hindo.o bcopy.o dic_atojis.o
	cc ${CFLAGS} -o oldatonewa oldatonewa.c strings.o sstrings.o  bdic.o gethinsi.o pwd.o hindo.o bcopy.o dic_atojis.o

wdreg: wdreg.c ujisf.o xutoj.o strings.o sstrings.o ../jlib/libwnn.a gethinsi.o bdic.o pwd.o
	cc ${CFLAGS} -o wdreg wdreg.c ujisf.o xutoj.o gethinsi.o bdic.o  strings.o sstrings.o pwd.o dic_atojis.o ../jlib/libwnn.a

wddel: wddel.c xutoj.o ../jlib/libwnn.a strings.o sstrings.o
	cc ${CFLAGS} -o wddel wddel.c xutoj.o ../jlib/libwnn.a strings.o sstrings.o

bdic.o: ../etc/bdic.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/bdic.c

strings.o: ../etc/strings.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/strings.c

sstrings.o: ../etc/sstrings.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/sstrings.c

gethinsi.o: ../etc/gethinsi.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/gethinsi.c

revdic.o: ../etc/revdic.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/revdic.c

hindo.o: ../etc/hindo.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/hindo.c

pwd.o: ../etc/pwd.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/pwd.c

bcopy.o: ../etc/bcopy.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/bcopy.c

xutoj.o: ../etc/xutoj.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/xutoj.c

dic_atojis.o: ../etc/dic_atojis.c
	cc ${CFLAGS} ${DEBUG} -c ../etc/dic_atojis.c

clean:
	rm -f a.out core *.o *~ *.orig *.rej #*

purge:
	rm -f ${allcmds} a.out core *.o *~ *.orig *.rej #*

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
#	Wnn Version 4.0
#
#	install	util
#
install: $(BINDIR)
	strip $(allcmds)
	cp $(allcmds) $(BINDIR)
#
#	dirctory check
#
$(BINDIR):
	@echo Please mkdir $(BINDIR)
	@false

depend:
	grep '^#include' $(srcs) | grep -v '<' | \
	      sed 's@:[^"]*"\([^"]*\)".*@: \1@' | \
	      sed 's/\.c/.o/' | sed 's,^../[a-z]*/,,' | \
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } \
	      END { print rec } ' > makedep
	echo '$$r makedep' >>eddep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep

# DO NOT DELETE THIS LINE -- make depend uses it

utoa.o:  
atou.o:  
sort.o:  
ujisf.o:  
bdic.o:  
utilbdic.o:
atos.o:  
stoa.o: 
atod.o:
