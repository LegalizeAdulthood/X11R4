#
#	Wnn Version 4.0
#
#
# If you change LIBDIR, you should  change "include/config.h".
# 
#STDEVICE = /dev/rst0
STDEVICE = /dev/st0

BINDIR = /usr/local/bin/Wnn4
LIBDIR = /usr/local/lib/wnn

SHELL = /bin/sh

MAKE = make
INSTALLDICT = -cp
PUBDIC_DIR = /usr/local/lib/wnn/dic/pubdic
WNNTOUCH = jutil/wnntouch

RM = rm -f
MV = mv

TOP = ../../..
UTILSRC = $(TOP)/util
IMAKESRC = $(UTILSRC)/imake
IRULESRC = $(UTILSRC)/imake.includes
IMAKE = $(IMAKESRC)/imake
IMAKE_DEFINES =
IMAKE_CMD = $(NEWTOP)$(IMAKE) -TImake.tmpl -I$(NEWTOP)$(IRULESRC) \
		-s Makefile $(IMAKE_DEFINES)

INSTDEFS = BINDIR=$(BINDIR) LIBDIR=$(LIBDIR)

#DEFS = DEFS='-O -20 -f -DSX9100BSD'		#LUNA (UNIOS-B)
#DEFS = DEFS='-DSUN  -g'			#SUN
#DEFS = DEFS='-DBSD42 -DTERMCAP'		#BSD4.2 (ex. SONY NEWS)
#DEFS = DEFS='-DBSD42 -DTERMCAP -DBSD43'	#BSD4.3 (and VAX Ultrix)
#LIBS = LIB_TERM='-ltermcap'		#SUN,BSD4.2/4.3
#GLIB = GETOPTLIB=			#SUN
#GLIB = GETOPTLIB='../jlib/libgetopt.a'		#BSD4.2/4.3


#DEFS = DEFS='-DSYSVR2 -DTERMINFO'		#SYS V R2
DEFS = DEFS='-g -Zs -DSX8870 -DINET -DNOTFORK'	#OMRON SX8870
#DEFS = DEFS='-O -Zs -DSX8870 -DINET'	#OMRON SX8870
#DEFS = DEFS='-DSYSVR2 -DTERMINFO -DNEC'		#NEC EWS4800
LIBS = LIB_TERM='-lcurses'			#SYS V R2
GLIB = GETOPTLIB=				#SYS V R2

SUBDIRS = conv etc romkan jlib jlib.V3 \
	  fuzokugo jserver jutil pubdic uum

#
#	Make all
#

all: Romkan Etc Conv Jlib Jserver  Uum  Jutil Fuzokugo

Romkan:
	cd romkan;   $(MAKE) $(DEFS)
Etc:
	cd etc;	     $(MAKE) $(DEFS)
Conv:
	cd conv;     $(MAKE) $(DEFS)
Jlib:
	cd jlib;     $(MAKE) $(DEFS)	     $(GLIB)
Jlibv3:
	cd jlib.V3;  $(MAKE) $(DEFS)	     $(GLIB)
Jserver:
	cd jserver;  $(MAKE) $(DEFS) $(LIBS) $(GLIB)
Uum:
	cd uum;      $(MAKE) $(DEFS) $(LIBS) $(GLIB) $(INSTDEFS)
Jutil:
	cd jutil;    $(MAKE) $(DEFS)	     $(GLIB) $(INSTDEFS)
Fuzokugo:
	cd fuzokugo; $(MAKE) $(DEFS)	     $(GLIB) $(INSTDEFS)

Makefile:: Imakefile \
	$(IRULESRC)/Imake.tmpl \
	$(IRULESRC)/Imake.rules \
	$(IRULESRC)/site.def
	-@if [ -f Makefile ]; then \
	echo "$(RM) Makefile.bak; $(MV) Makefile Makefile.bak"; \
	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP)

Makefiles:: $(IMAKE)

Makefiles::
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
	echo "Making Makefiles in $$i..."; \
	cd $$i ;$(MAKE) Makefile $(INSTDEFS) ; \
	cd ../ ; \
	done

#
#	install all
#
#	If you do not want to use new dictionaries,
#	you must not use this.
#	And you must check 'instpubdic' in the file "jserver/Makefile".
#
install: instjserver instuum instjutil instpubdic instjlib instjlibv3

instjserver:
	cd jserver; $(MAKE) $(INSTDEFS)	instjserver

instuum:
	cd uum;     $(MAKE) $(INSTDEFS)	$(LIBS) install

instjutil:
	cd jutil;   $(MAKE) $(INSTDEFS)	install
	cd fuzokugo; $(MAKE) $(INSTDEFS) install

instpubdic:$(PUBDIC_DIR)
	$(INSTALLDICT) pubdic/*.dic pubdic/*.fzk $(PUBDIC_DIR)
	$(WNNTOUCH) $(PUBDIC_DIR)/*.dic $(PUBDIC_DIR)/*.fzk
#	cp pubdic/Version $(PUBDIC_DIR)
	
$(PUBDIC_DIR):
	@echo Please mkdir $(PUBDIC_DIR)
	@false

instjlib:
	cd jlib;    $(MAKE) $(INSTDEFS)	install

instjlibv3:
	cd jlib.V3; $(MAKE) $(INSTDEFS)	install

#
#	clean
#
clean:
	cd etc;      $(MAKE)	clean
	cd jserver;  $(MAKE)	clean
	cd jlib;     $(MAKE)	clean
	cd jlib.V3;  $(MAKE)	clean
	cd romkan;   $(MAKE)	clean
	cd uum;      $(MAKE)	clean $(INSTDEFS)
	cd jutil;    $(MAKE)	clean $(INSTDEFS)
	cd conv;     $(MAKE)	clean
	cd fuzokugo; $(MAKE)	clean $(INSTDEFS)

#
#	purge
#
purge:
	cd etc;      $(MAKE)	purge
	cd jserver;  $(MAKE)	purge
	cd jlib;     $(MAKE)	purge
	cd jlib.V3;  $(MAKE)	purge
	cd romkan;   $(MAKE)	purge
	cd uum;      $(MAKE)	purge $(INSTDEFS)
	cd jutil;    $(MAKE)	purge $(INSTDEFS)
	cd conv;     $(MAKE)	purge
	cd fuzokugo; $(MAKE)	purge $(INSTDEFS)

#
#	make Tape
#

#tape:
#	tar cvhFF READ.ME Makefile pubdic etc jd \
#		include jserver romkan conv jlib jlib.V3 uum jutil \
#		manual fuzokugo
#
#stape:
#	tar cvhfbFF $(STDEVICE) 126 READ.ME Makefile binbo pubdic jd himanashi \
#		etc include jserver romkan conv jlib jlib.V3 uum jutil \
#		manual fuzokugo
#
#sxtape:	
#	tar cvf /dev/st0 READ.ME Makefile binbo pubdic jd himanashi \
#		etc include jserver romkan conv jlib jlib.V3 uum jutil \
#		manual fuzokugo
#
#binbotape:
#	tar cv binbo pubdic jd
#
#binbostape:
#	tar cvfb $(STDEVICE) 126 binbo pubdic jd
#
#binbosxtape:
#	tar cvf $(STDEVICE) binbo pubdic jd

#
#	make Binbo dir
#
binbodir:
#	rm -f binbo/*
#	ln	binbo.Makefile jserver/jserver uum/uum fuzokugo/atof \
#		jutil/atod jutil/dtoa jutil/dicsort jutil/oldatonewa \
#		jutil/wddel  jutil/wdreg \
#		jutil/wnnstat  jutil/wnnkill \
#	   binbo
#	mv binbo/binbo.Makefile binbo/Makefile

