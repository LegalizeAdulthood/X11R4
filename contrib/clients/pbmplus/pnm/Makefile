# Makefile for pnm tools.
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

# CONFIGURE: You can compile PNM without PPM.  If you don't want PPM,
# comment out the next five lines.  This will make the PNM programs use
# less memory.
PPMOPT =	-DPPM
PPMDIR =	../ppm
INCLUDEPPM =	-I$(PPMDIR)
LIBPPM =	$(PPMDIR)/libppm.a
DEFPPM =	$(PPMDIR)/ppm.h ../pbmplus.h
DEFLIBPPM =	$(PPMDIR)/libppm.h

# CONFIGURE: Likewise here: if you don't have PGM, comment these lines out.
PGMOPT =	-DPGM
PGMDIR =	../pgm
INCLUDEPGM =	-I$(PGMDIR)
LIBPGM =	$(PGMDIR)/libpgm.a
DEFPGM =	$(PGMDIR)/pgm.h
DEFLIBPGM =	$(PGMDIR)/libpgm.h

# PBM is required, don't comment these lines out.
PBMOPT =	-DPBM
PBMDIR =	../pbm
INCLUDEPBM =	-I$(PBMDIR)
LIBPBM =	$(PBMDIR)/libpbm.a
DEFPBM =	$(PBMDIR)/pbm.h
DEFLIBPBM =	$(PBMDIR)/libpbm.h

SHELL =		/bin/sh
INCLUDE =	$(INCLUDEPPM) $(INCLUDEPGM) $(INCLUDEPBM)
ALLCFLAGS =	$(CFLAGS) $(PPMOPT) $(PGMOPT) $(PBMOPT) $(INCLUDE)
LIBPNM =	libpnm.a

BINARIES =	pnmcat pnmcrop pnmcut pnmenlarge pnmflip pnminvert pnmpaste \
		pnmtile

MANUALS1 =	pnmcat.1 pnmcrop.1 pnmcut.1 pnmenlarge.1 pnmflip.1 \
		pnminvert.1 pnmpaste.1 pnmtile.1
MANUALS3 =	libpnm.3
MANUALS5 =	pnm.5

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
$(BINARIES):	pnm.h $(DEFPPM) $(DEFPGM) $(DEFPBM) $(LIBPNM) \
			$(LIBPPM) $(LIBPGM) $(LIBPBM)
	$(CC) $(ALLCFLAGS) $(LDFLAGS) -o $@ $@.c $(LIBPNM) $(LIBPPM) $(LIBPGM) $(LIBPBM)

# And library.
$(LIBPNM):	libpnm1.o libpnm2.o libpnm3.o
	-rm $(LIBPNM)
	ar rc $(LIBPNM) libpnm1.o libpnm2.o libpnm3.o
	-ranlib $(LIBPNM)

libpnm1.o:	pnm.h $(DEFPPM) $(DEFPGM) $(DEFPBM) libpnm1.c
	$(CC) $(ALLCFLAGS) -c libpnm1.c
libpnm2.o:	pnm.h $(DEFPPM) $(DEFPGM) $(DEFPBM) libpnm2.c $(DEFLIBPPM) \
		$(DEFLIBPGM) $(DEFLIBPBM)
	$(CC) $(ALLCFLAGS) -c libpnm2.c
libpnm3.o:	pnm.h $(DEFPPM) $(DEFPGM) $(DEFPBM) libpnm3.c $(DEFLIBPPM) \
		$(DEFLIBPGM) $(DEFLIBPBM)
	$(CC) $(ALLCFLAGS) -c libpnm3.c

# Other dependencies.
pnmcat:		pnmcat.c
pnmcrop:	pnmcrop.c
pnmcut:		pnmcut.c
pnmenlarge:	pnmenlarge.c
pnmflip:	pnmflip.c
pnminvert:	pnminvert.c
pnmpaste:	pnmpaste.c
pnmtile:	pnmtile.c

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
