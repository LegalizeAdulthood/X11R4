# Makefile for pgm tools.
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

PBMDIR =	../pbm
INCLUDEPBM =	-I$(PBMDIR)
LIBPBM =	$(PBMDIR)/libpbm.a
DEFPBM =	$(PBMDIR)/pbm.h ../pbmplus.h
DEFLIBPBM =	$(PBMDIR)/libpbm.h

SHELL =		/bin/sh
INCLUDE =	$(INCLUDEPBM)
ALLCFLAGS =	$(CFLAGS) $(INCLUDE)
LIBPGM =	libpgm.a

BINARIES =	fitstopgm fstopgm hipstopgm pgmenhance pgmhist pgmnorm \
		pgmtopbm pgmtofits pgmtops psidtopgm rawtopgm tifftopgm

MANUALS1 =	fitstopgm.1 fstopgm.1 hipstopgm.1 pgmenhance.1 pgmhist.1 \
		pgmnorm.1 pgmtofits.1 pgmtopbm.1 pgmtops.1 psidtopgm.1 \
		rawtopgm.1 tifftopgm.1
MANUALS3 =	libpgm.3
MANUALS5 =	pgm.5

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
$(BINARIES):	pgm.h $(DEFPBM) $(LIBPGM) $(LIBPBM)
	$(CC) $(ALLCFLAGS) $(LDFLAGS) -o $@ $@.c $(LIBPGM) $(LIBPBM)

# And library.
$(LIBPGM):	libpgm1.o libpgm2.o
	-rm $(LIBPGM)
	ar rc $(LIBPGM) libpgm1.o libpgm2.o
	-ranlib $(LIBPGM)

libpgm1.o:	pgm.h $(DEFPBM) libpgm.h libpgm1.c
	$(CC) $(ALLCFLAGS) -c libpgm1.c
libpgm2.o:	pgm.h $(DEFPBM) libpgm.h libpgm2.c $(DEFLIBPBM)
	$(CC) $(ALLCFLAGS) -c libpgm2.c

# Other dependencies.
fitstopgm:	fitstopgm.c
fstopgm:	fstopgm.c
hipstopgm:	hipstopgm.c
pgmenhance:	pgmenhance.c
pgmhist:	pgmhist.c
pgmnorm:	pgmnorm.c
pgmtofits:	pgmtofits.c
pgmtopbm:	pgmtopbm.c dithers.h $(DEFPBM)
pgmtops:	pgmtops.c
psidtopgm:	psidtopgm.c
rawtopgm:	rawtopgm.c
tifftopgm:	tifftopgm.c tiff.h

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
