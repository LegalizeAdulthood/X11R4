# Makefile for pbm tools.
#
# Copyright (C) 1989 by Jef Poskanzer.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that the above copyright notice appear in all copies and that both that
# copyright notice and this permission notice appear in supporting
# documentation.  This software is provided "as is" without express or
# implied warranty.

# Default values, usually overridden by top-level Makefile.
# CC =		cc
CC =		gcc -fcombine-regs
# CFLAGS =	
# CFLAGS =	-g
CFLAGS =	-O
# CFLAGS =	-g -O
LDFLAGS =	-s
# LDFLAGS =	
INSTALLBINARIES =	/usr/new/pbm
INSTALLMANUALS1 =	/usr/man/mann
INSTALLMANUALS3 =	/usr/man/mann
INSTALLMANUALS5 =	/usr/man/mann

SHELL =		/bin/sh
ALLCFLAGS =	$(CFLAGS)
LIBPBM =	libpbm.a

BINARIES =	brushtopbm cmuwmtopbm g3topbm icontopbm gemtopbm macptopbm \
		mgrtopbm pbmlife pbmmake pbmmask pbmpaste pbmreduce \
		pbmtoascii pbmtobbnbg pbmtocmuwm pbmtog3 pbmtogo pbmtoicon \
		pbmtolj pbmtomacp pbmtomgr pbmtoptx pbmtorast pbmtox10bm \
		pbmtoxbm pbmtoxwd pbmupc pcxtopbm picttopbm rasttopbm xbmtopbm \
		xwdtopbm

MANUALS1 =	brushtopbm.1 cmuwmtopbm.1 g3topbm.1 icontopbm.1 \
		gemtopbm.1 macptopbm.1 mgrtopbm.1 \
		pbmlife.1 pbmmake.1 pbmmask.1 pbmpaste.1 \
		pbmreduce.1 pbmtoascii.1 pbmtobbnbg.1 pbmtocmuwm.1 pbmtog3.1 \
		pbmtogo.1 pbmtoicon.1 pbmtolj.1 pbmtomacp.1 pbmtomgr.1 \
		pbmtoptx.1 pbmtorast.1 pbmtox10bm.1 pbmtoxbm.1 \
		pbmtoxwd.1 pbmupc.1 pcxtopbm.1 picttopbm.1 rasttopbm.1 \
		xbmtopbm.1 xwdtopbm.1
MANUALS3 =	libpbm.3
MANUALS5 =	pbm.5

all:		binaries

install:	installbinaries installmanuals
# install:	installbinaries

binaries:	$(BINARIES)

installbinaries:	binaries
	cp $(BINARIES) $(INSTALLBINARIES)

installmanuals:
	cp $(MANUALS1) $(INSTALLMANUALS1)
	cp $(MANUALS3) $(INSTALLMANUALS3)
	cp $(MANUALS5) $(INSTALLMANUALS5)

# Rule for plain programs.
$(BINARIES):	pbm.h ../pbmplus.h $(LIBPBM)
	$(CC) $(ALLCFLAGS) $(LDFLAGS) -o $@ $@.c $(LIBPBM)

# And library.
$(LIBPBM):	libpbm1.o libpbm2.o libpbm3.o libpbm4.o libpbm5.o
	-rm $(LIBPBM)
	ar rc $(LIBPBM) libpbm1.o libpbm2.o libpbm3.o libpbm4.o libpbm5.o
	-ranlib $(LIBPBM)

libpbm1.o:	pbm.h ../pbmplus.h libpbm.h libpbm1.c
	$(CC) $(ALLCFLAGS) -c libpbm1.c
libpbm2.o:	pbm.h ../pbmplus.h libpbm.h libpbm2.c
	$(CC) $(ALLCFLAGS) -c libpbm2.c
libpbm3.o:	pbm.h ../pbmplus.h libpbm.h libpbm3.c
	$(CC) $(ALLCFLAGS) -c libpbm3.c
libpbm4.o:	pbm.h ../pbmplus.h libpbm.h libpbm4.c
	$(CC) $(ALLCFLAGS) -c libpbm4.c
libpbm5.o:	pbm.h ../pbmplus.h rast.h libpbm5.c
	$(CC) $(ALLCFLAGS) -c libpbm5.c

# Other dependencies.
brushtopbm:	brushtopbm.c
cmuwmtopbm:	cmuwmtopbm.c cmuwm.h
g3topbm:	g3topbm.c g3.h
icontopbm:	icontopbm.c
gemtopbm:	gemtopbm.c
macptopbm:	macptopbm.c macp.h
mgrtopbm:	mgrtopbm.c mgr.h
pbmlife:	pbmlife.c
pbmmake:	pbmmake.c
pbmmask:	pbmmask.c
pbmpaste:	pbmpaste.c
pbmreduce:	pbmreduce.c
pbmtoascii:	pbmtoascii.c
pbmtobbnbg:	pbmtobbnbg.c
pbmtocmuwm:	pbmtocmuwm.c cmuwm.h
pbmtog3:	pbmtog3.c g3.h
pbmtogo:	pbmtogo.c
pbmtoicon:	pbmtoicon.c
pbmtolj:	pbmtolj.c
pbmtomacp:	pbmtomacp.c macp.h
pbmtomgr:	pbmtomgr.c mgr.h
pbmtoptx:	pbmtoptx.c
pbmtorast:	pbmtorast.c rast.h
pbmtox10bm:	pbmtox10bm.c
pbmtoxbm:	pbmtoxbm.c
pbmtoxwd:	pbmtoxwd.c x11wd.h
pbmupc:		pbmupc.c
pcxtopbm:	pcxtopbm.c
picttopbm:	picttopbm.c pict.h
rasttopbm:	rasttopbm.c rast.h
xbmtopbm:	xbmtopbm.c
xwdtopbm:	xwdtopbm.c x11wd.h x10wd.h

clean:
	-rm -f *.o *.a *.cat core $(BINARIES)


# Imakefile stuff.  Ignore if you're not an X11 type.

            TOP = ../../../../../../usr/src/new/X11

             RM = rm -f
             MV = mv
        UTILSRC = $(TOP)/util
       IMAKESRC = $(UTILSRC)/imake
       IRULESRC = $(UTILSRC)/imake.includes
          IMAKE = $(IMAKESRC)/imake
  IMAKE_DEFINES =
      IMAKE_CMD = $(NEWTOP)$(IMAKE) -TImake.tmpl -I$(NEWTOP)$(IRULESRC) \
			-s Makefile $(IMAKE_DEFINES)
Makefile: Imakefile
	-@if [ -f Makefile ]; then \
	echo "$(RM) Makefile.bak; $(MV) Makefile Makefile.bak"; \
	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP)

Makefiles:
