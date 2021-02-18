# Makefile for ppm tools.
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

PGMDIR =	../pgm
INCLUDEPGM =	-I$(PGMDIR)
LIBPGM =	$(PGMDIR)/libpgm.a
DEFPGM =	$(PGMDIR)/pgm.h
DEFLIBPGM =	$(PGMDIR)/libpgm.h

PBMDIR =	../pbm
INCLUDEPBM =	-I$(PBMDIR)
LIBPBM =	$(PBMDIR)/libpbm.a
DEFPBM =	$(PBMDIR)/pbm.h ../pbmplus.h
DEFLIBPBM =	$(PBMDIR)/libpbm.h

SHELL =		/bin/sh
INCLUDE =	$(INCLUDEPGM) $(INCLUDEPBM)
ALLCFLAGS =	$(CFLAGS) $(INCLUDE)
LIBPPM =	libppm.a

PORTBINARIES =	giftoppm ilbmtoppm imgtoppm mtvtoppm ppmarith ppmconvol \
		ppmcscale ppmhist ppmquant ppmscale ppmtogif ppmtoilbm \
		ppmtopgm ppmtops ppmtorast ppmtoxwd qrttoppm rasttoppm \
		tgatoppm xwdtoppm
MATHBINARIES =	ppmpat ppmrotate ppmshear
BINARIES =	$(PORTBINARIES) $(MATHBINARIES)

MANUALS1 =	giftoppm.1 ilbmtoppm.1 imgtoppm.1 mtvtoppm.1 ppmarith.1 \
		ppmconvol.1 ppmcscale.1 ppmhist.1 ppmpat.1 ppmquant.1 \
		ppmrotate.1 ppmscale.1 ppmshear.1 ppmtogif.1 ppmtoilbm.1 \
		ppmtopgm.1 ppmtops.1 ppmtorast.1 ppmtoxwd.1 qrttoppm.1 \
		rasttoppm.1 tgatoppm.1 xwdtoppm.1
MANUALS3 =	libppm.3
MANUALS5 =	ppm.5

all:		binaries
install:	installbinaries installmanuals
# install:	installbinaries

binaries:	$(BINARIES)

installbinaries:	binaries
	cp $(BINARIES) $(INSTALLBINARIES)
	cp convolscripts/* $(INSTALLBINARIES)

installmanuals:
	cp $(MANUALS1) $(INSTALLMANUALS1)
	cp $(MANUALS3) $(INSTALLMANUALS3)
	cp $(MANUALS5) $(INSTALLMANUALS5)

# Rule for plain programs.
$(PORTBINARIES):	ppm.h $(DEFPGM) $(DEFPBM) $(LIBPPM) $(LIBPGM) $(LIBPBM)
	$(CC) $(ALLCFLAGS) $(LDFLAGS) -o $@ $@.c $(LIBPPM) $(LIBPGM) $(LIBPBM)

# Rule for math-dependent programs.
$(MATHBINARIES):	ppm.h $(DEFPGM) $(DEFPBM) $(LIBPPM) $(LIBPGM) $(LIBPBM)
	$(CC) $(ALLCFLAGS) $(LDFLAGS) -o $@ $@.c -lm $(LIBPPM) $(LIBPGM) $(LIBPBM)

# And library.
$(LIBPPM):	libppm1.o libppm2.o libppm3.o libppm4.o libppm5.o
	-rm $(LIBPPM)
	ar rc $(LIBPPM) libppm1.o libppm2.o libppm3.o libppm4.o libppm5.o
	-ranlib $(LIBPPM)

libppm1.o:	ppm.h $(DEFPGM) $(DEFPBM) libppm.h libppm1.c
	$(CC) $(ALLCFLAGS) -c libppm1.c
libppm2.o:	ppm.h $(DEFPGM) $(DEFPBM) libppm.h libppm2.c $(DEFLIBPGM) \
		$(DEFLIBPBM)
	$(CC) $(ALLCFLAGS) -c libppm2.c
libppm3.o:	ppm.h $(DEFPGM) $(DEFPBM) libppm.h libppm3.c
	$(CC) $(ALLCFLAGS) -c libppm3.c
libppm4.o:	ppm.h $(DEFPGM) $(DEFPBM) ppmcmap.h libppm4.c
	$(CC) $(ALLCFLAGS) -c libppm4.c
libppm5.o:	ppm.h $(DEFPGM) $(DEFPBM) ppmdraw.h libppm5.c
	$(CC) $(ALLCFLAGS) -c libppm5.c

# Other dependencies.
giftoppm:	giftoppm.c
ilbmtoppm:	ilbmtoppm.c ilbm.h
imgtoppm:	imgtoppm.c
mtvtoppm:	mtvtoppm.c
ppmarith:	ppmarith.c
ppmconvol:	ppmconvol.c
ppmcscale:	ppmcscale.c
ppmhist:	ppmhist.c ppmcmap.h
ppmpat:		ppmpat.c ppmdraw.h
ppmquant:	ppmquant.c $(PGMDIR)/dithers.h ppmcmap.h
ppmrotate:	ppmrotate.c
ppmscale:	ppmscale.c
ppmshear:	ppmshear.c
ppmtogif:	ppmtogif.c ppmcmap.h
ppmtoilbm:	ppmtoilbm.c ilbm.h ppmcmap.h
ppmtopgm:	ppmtopgm.c
ppmtops:	ppmtops.c
ppmtoxwd:	ppmtoxwd.c $(PBMDIR)/x11wd.h ppmcmap.h
ppmtorast:	ppmtorast.c $(PBMDIR)/rast.h ppmcmap.h
qrttoppm:	qrttoppm.c
rasttoppm:	rasttoppm.c $(PBMDIR)/rast.h
tgatoppm:	tgatoppm.c tga.h
xwdtoppm:	xwdtoppm.c $(PBMDIR)/x11wd.h $(PBMDIR)/x10wd.h

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
