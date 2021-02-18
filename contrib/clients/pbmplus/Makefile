# Makefile for pbmplus tools.
#
# Copyright (C) 1989 by Jef Poskanzer.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that the above copyright notice appear in all copies and that both that
# copyright notice and this permission notice appear in supporting
# documentation.  This software is provided "as is" without express or
# implied warranty.

# CONFIGURE: define which of the four sub-packages you want to make.  PBM
# is required; PNM is recommended; and if you want PPM you must have PGM
# too.  This means there are only six possibilities:
SUBDIRS =	pbm pgm ppm pnm
# SUBDIRS =	pbm pgm ppm
# SUBDIRS =	pbm pgm pnm
# SUBDIRS =	pbm pgm
# SUBDIRS =	pbm pnm
# SUBDIRS =	pbm

# CONFIGURE: gcc makes things go about 20% faster, but not everyone has it.
# Warning: do not use gcc's -finline-functions or -fstrength-reduce flags,
# they can produce incorrect code.  (This is with gcc version 1.35, later
# versions may fix these bugs.)
# CC =		cc
CC =		gcc -fcombine-regs

# CONFIGURE: cc flags go here.
# CFLAGS =	-g
CFLAGS =	-O
# CFLAGS =	-g -O

# CONFIGURE: ld flags go here.
LDFLAGS =	-s
# LDFLAGS =	

# CONFIGURE: Define the directory that you want the binaries copied to.
INSTALLBINARIES =	/usr/new/pbm

# CONFIGURE: Define the directories that you want the manual sources copied to.
INSTALLMANUALS1 =	/usr/man/mann
INSTALLMANUALS3 =	/usr/man/mann
INSTALLMANUALS5 =	/usr/man/mann

SHELL =		/bin/sh

all:
	for i in $(SUBDIRS) ; do \
	    ( cd $$i ; make $(MFLAGS) 'CC=$(CC)' 'CFLAGS=$(CFLAGS)' 'LDFLAGS=$(LDFLAGS)' 'INSTALLBINARIES=$(INSTALLBINARIES)' 'INSTALLMANUALS1=$(INSTALLMANUALS1)' 'INSTALLMANUALS3=$(INSTALLMANUALS3)' 'INSTALLMANUALS5=$(INSTALLMANUALS5)' all ); \
	done

install:
	for i in $(SUBDIRS) ; do \
	    ( cd $$i ; make $(MFLAGS) 'CC=$(CC)' 'CFLAGS=$(CFLAGS)' 'LDFLAGS=$(LDFLAGS)' 'INSTALLBINARIES=$(INSTALLBINARIES)' 'INSTALLMANUALS1=$(INSTALLMANUALS1)' 'INSTALLMANUALS3=$(INSTALLMANUALS3)' 'INSTALLMANUALS5=$(INSTALLMANUALS5)' install ); \
	done

installbinaries:
	for i in $(SUBDIRS) ; do \
	    ( cd $$i ; make $(MFLAGS) 'CC=$(CC)' 'CFLAGS=$(CFLAGS)' 'LDFLAGS=$(LDFLAGS)' 'INSTALLBINARIES=$(INSTALLBINARIES)' installbinaries ); \
	done

installmanuals:
	for i in $(SUBDIRS) ; do \
	    ( cd $$i ; make $(MFLAGS) 'CC=$(CC)' 'CFLAGS=$(CFLAGS)' 'LDFLAGS=$(LDFLAGS)' 'INSTALLMANUALS1=$(INSTALLMANUALS1)' 'INSTALLMANUALS3=$(INSTALLMANUALS3)' 'INSTALLMANUALS5=$(INSTALLMANUALS5)' installmanuals ); \
	done

clean:
	-rm -f *.shar*
	for i in $(SUBDIRS) ; do \
	    ( cd $$i ; make $(MFLAGS) clean ); \
	done


# Sharchive stuff.

shar:		pbmplus.shar pbm.shar pgm.shar ppm.shar pnm.shar

pbmplus.shar:	README CHANGES FORMATS OTHER.SYSTEMS TIPS Imakefile \
		pbmplus.h compataliases
	shar -v -c -p X README CHANGES FORMATS OTHER.SYSTEMS TIPS Makefile Imakefile pbmplus.h compataliases > $@

pbm.shar:	pbm.shar1 pbm.shar2 pbm.shar3 pbm.shar4 pbm.shar5 pbm.shar6
pbm.shar1:	pbm/Makefile pbm/Imakefile pbm/pbm.h pbm/icontopbm.c \
		pbm/icontopbm.1 pbm/macptopbm.c pbm/macptopbm.1 \
		pbm/rasttopbm.c pbm/rasttopbm.1 pbm/xbmtopbm.c \
		pbm/xbmtopbm.1 pbm/xwdtopbm.c
	shar -v -c -p X pbm/Makefile pbm/Imakefile pbm/pbm.h pbm/icontopbm.c pbm/icontopbm.1 pbm/macptopbm.c pbm/macptopbm.1 pbm/rasttopbm.c pbm/rasttopbm.1 pbm/xbmtopbm.c pbm/xbmtopbm.1 pbm/xwdtopbm.c > $@
pbm.shar2:	pbm/xwdtopbm.1 pbm/pbmtoicon.c pbm/pbmtoicon.1 \
		pbm/pbmtoptx.c pbm/pbmtoptx.1 pbm/pbmtorast.c pbm/pbmtorast.1 \
		pbm/pbmtoxbm.c pbm/pbmtoxbm.1 pbm/pbmtox10bm.c \
		pbm/pbmtox10bm.1 pbm/pbmtoascii.c pbm/pbmtoascii.1 \
		pbm/pbmpaste.c pbm/pbmpaste.1 pbm/g3topbm.c pbm/g3topbm.1
	shar -v -c -p X pbm/xwdtopbm.1 pbm/pbmtoicon.c pbm/pbmtoicon.1 pbm/pbmtoptx.c pbm/pbmtoptx.1 pbm/pbmtorast.c pbm/pbmtorast.1 pbm/pbmtoxbm.c pbm/pbmtoxbm.1 pbm/pbmtox10bm.c pbm/pbmtox10bm.1 pbm/pbmtoascii.c pbm/pbmtoascii.1 pbm/pbmpaste.c pbm/pbmpaste.1 pbm/g3topbm.c pbm/g3topbm.1 > $@
pbm.shar3:	pbm/pbmmake.c pbm/pbmmake.1 pbm/pbmtolj.c \
		pbm/pbmtolj.1 pbm/pbmtomacp.c pbm/pbmtomacp.1 \
		pbm/pbmtoxwd.c pbm/g3.h pbm/pbmtog3.c pbm/pbmtog3.1 \
		pbm/pbmtoxwd.1 pbm/brushtopbm.c pbm/brushtopbm.1 \
		pbm/mgrtopbm.c pbm/mgrtopbm.1
	shar -v -c -p X pbm/pbmmake.c pbm/pbmmake.1 pbm/pbmtolj.c pbm/pbmtolj.1 pbm/pbmtomacp.c pbm/pbmtomacp.1 pbm/pbmtoxwd.c pbm/g3.h pbm/pbmtog3.c pbm/pbmtog3.1 pbm/pbmtoxwd.1 pbm/brushtopbm.c pbm/brushtopbm.1 pbm/mgrtopbm.c pbm/mgrtopbm.1 > $@
pbm.shar4:	pbm/pbm.5 pbm/libpbm.3 pbm/libpbm.h pbm/macp.h pbm/x10wd.h \
		pbm/x11wd.h pbm/bitreverse.h pbm/pbmreduce.c \
		pbm/pbmreduce.1 pbm/gemtopbm.c pbm/gemtopbm.1 pbm/pbmtogo.c \
		pbm/pbmtogo.1 pbm/pbmlife.c pbm/pbmlife.1 pbm/pcxtopbm.c
	shar -v -c -p X pbm/pbm.5 pbm/libpbm.3 pbm/libpbm.h pbm/macp.h pbm/x10wd.h pbm/x11wd.h pbm/bitreverse.h pbm/pbmreduce.c pbm/pbmreduce.1 pbm/gemtopbm.c pbm/gemtopbm.1 pbm/pbmtogo.c pbm/pbmtogo.1 pbm/pbmlife.c pbm/pbmlife.1 pbm/pcxtopbm.c > $@
pbm.shar5:	pbm/pcxtopbm.1 pbm/pbmupc.c pbm/pbmupc.1 pbm/mgr.h \
		pbm/libpbm1.c pbm/libpbm2.c pbm/libpbm3.c pbm/libpbm4.c \
		pbm/libpbm5.c
	shar -v -c -p X pbm/pcxtopbm.1 pbm/pbmupc.c pbm/pbmupc.1 pbm/mgr.h pbm/libpbm1.c pbm/libpbm2.c pbm/libpbm3.c pbm/libpbm4.c pbm/libpbm5.c > $@
pbm.shar6:	pbm/cmuwm.h pbm/cmuwmtopbm.c pbm/cmuwmtopbm.1 \
		pbm/pbmtocmuwm.c pbm/pbmtocmuwm.1 pbm/pbmmask.c pbm/pbmmask.1 \
		pbm/pbmtobbnbg.c pbm/pbmtobbnbg.1 pbm/pbmtomgr.c \
		pbm/pbmtomgr.1 pbm/pict.h pbm/picttopbm.c pbm/picttopbm.1 \
		pbm/rast.h
	shar -v -c -p X pbm/cmuwm.h pbm/cmuwmtopbm.c pbm/cmuwmtopbm.1 pbm/pbmtocmuwm.c pbm/pbmtocmuwm.1 pbm/pbmmask.c pbm/pbmmask.1 pbm/pbmtobbnbg.c pbm/pbmtobbnbg.1 pbm/pbmtomgr.c pbm/pbmtomgr.1 pbm/pict.h pbm/picttopbm.c pbm/picttopbm.1 pbm/rast.h > $@

pgm.shar:	pgm.shar1 pgm.shar2 pgm.shar3
pgm.shar1:	pgm/Makefile pgm/Imakefile pgm/pgm.5 pgm/libpgm.3 \
		pgm/libpgm1.c pgm/libpgm2.c pgm/pgm.h pgm/libpgm.h \
		pgm/fstopgm.c pgm/fstopgm.1 pgm/hipstopgm.c pgm/hipstopgm.1 \
		pgm/pgmenhance.c pgm/pgmenhance.1
	shar -v -c -p X pgm/Makefile pgm/Imakefile pgm/pgm.5 pgm/libpgm.3 pgm/libpgm1.c pgm/libpgm2.c pgm/pgm.h pgm/libpgm.h pgm/fstopgm.c pgm/fstopgm.1 pgm/hipstopgm.c pgm/hipstopgm.1 pgm/pgmenhance.c pgm/pgmenhance.1 > $@
pgm.shar2:	pgm/pgmtopbm.c pgm/pgmtopbm.1 pgm/dithers.h pgm/tiff.h \
		pgm/pgmtops.c pgm/pgmtops.1 pgm/psidtopgm.c pgm/psidtopgm.1 \
		pgm/fitstopgm.c pgm/fitstopgm.1
	shar -v -c -p X pgm/pgmtopbm.c pgm/pgmtopbm.1 pgm/dithers.h pgm/tiff.h pgm/pgmtops.c pgm/pgmtops.1 pgm/psidtopgm.c pgm/psidtopgm.1 pgm/fitstopgm.c pgm/fitstopgm.1 > $@
pgm.shar3:	pgm/pgmtofits.c pgm/pgmtofits.1 pgm/tifftopgm.c \
		pgm/tifftopgm.1 pgm/rawtopgm.c pgm/rawtopgm.1 pgm/pgmhist.c \
		pgm/pgmhist.1 pgm/pgmnorm.c pgm/pgmnorm.1
	shar -v -c -p X pgm/pgmtofits.c pgm/pgmtofits.1 pgm/tifftopgm.c pgm/tifftopgm.1 pgm/rawtopgm.c pgm/rawtopgm.1 pgm/pgmhist.c pgm/pgmhist.1 pgm/pgmnorm.c pgm/pgmnorm.1 > $@

ppm.shar:	ppm.shar1 ppm.shar2 ppm.shar3 ppm.shar4 ppm.shar5 ppm.shar6 \
		ppm.shar7
ppm.shar1:	ppm/Makefile ppm/Imakefile ppm/ppm.5 ppm/libppm.3 \
		ppm/libppm1.c ppm/libppm2.c ppm/libppm3.c ppm/libppm4.c \
		ppm/libppm5.c ppm/convolscripts/ppmsmooth
	shar -v -c -p X ppm/Makefile ppm/Imakefile ppm/ppm.5 ppm/libppm.3 ppm/libppm1.c ppm/libppm2.c ppm/libppm3.c ppm/libppm4.c ppm/libppm5.c ppm/convolscripts/ppmsmooth > $@
ppm.shar2:	ppm/ppm.h ppm/ppmcmap.h ppm/ppmdraw.h ppm/libppm.h \
		ppm/tga.h ppm/mtvtoppm.c ppm/mtvtoppm.1 ppm/giftoppm.c \
		ppm/giftoppm.1 ppm/ppmrotate.c ppm/ppmrotate.1
	shar -v -c -p X ppm/ppm.h ppm/ppmcmap.h ppm/ppmdraw.h ppm/libppm.h ppm/tga.h ppm/mtvtoppm.c ppm/mtvtoppm.1 ppm/giftoppm.c ppm/giftoppm.1 ppm/ppmrotate.c ppm/ppmrotate.1 > $@
ppm.shar3:	ppm/ppmscale.c ppm/ppmscale.1 ppm/ppmshear.c \
		ppm/ppmshear.1 ppm/ppmpat.c
	shar -v -c -p X ppm/ppmscale.c ppm/ppmscale.1 ppm/ppmshear.c ppm/ppmshear.1 ppm/ppmpat.c > $@
ppm.shar4:	ppm/ppmpat.1 ppm/ppmconvol.c ppm/ppmconvol.1 \
		ppm/ppmtopgm.c ppm/ppmtopgm.1 ppm/ppmtorast.c ppm/ppmtorast.1 \
		ppm/rasttoppm.c ppm/rasttoppm.1
	shar -v -c -p X ppm/ppmpat.1 ppm/ppmconvol.c ppm/ppmconvol.1 ppm/ppmtopgm.c ppm/ppmtopgm.1 ppm/ppmtorast.c ppm/ppmtorast.1 ppm/rasttoppm.c ppm/rasttoppm.1 > $@
ppm.shar5:	ppm/ppmcscale.c ppm/ppmcscale.1 ppm/ilbmtoppm.c \
		ppm/ilbm.h ppm/ilbmtoppm.1 ppm/ppmquant.c ppm/ppmquant.1 \
		ppm/ppmarith.c ppm/ppmarith.1
	shar -v -c -p X ppm/ppmcscale.c ppm/ppmcscale.1 ppm/ilbmtoppm.c ppm/ilbm.h ppm/ilbmtoppm.1 ppm/ppmquant.c ppm/ppmquant.1 ppm/ppmarith.c ppm/ppmarith.1 > $@
ppm.shar6:	ppm/qrttoppm.c ppm/qrttoppm.1 ppm/ppmtops.c \
		ppm/ppmtops.1 ppm/ppmtogif.c ppm/ppmtogif.1 ppm/ppmtoilbm.c \
		ppm/ppmtoilbm.1
	shar -v -c -p X ppm/qrttoppm.c ppm/qrttoppm.1 ppm/ppmtops.c ppm/ppmtops.1 ppm/ppmtogif.c ppm/ppmtogif.1 ppm/ppmtoilbm.c ppm/ppmtoilbm.1 > $@
ppm.shar7:	ppm/tgatoppm.c ppm/tgatoppm.1 ppm/ppmhist.c \
		ppm/ppmhist.1 ppm/xwdtoppm.c ppm/xwdtoppm.1 ppm/ppmtoxwd.c \
		ppm/ppmtoxwd.1 ppm/imgtoppm.c ppm/imgtoppm.1
	shar -v -c -p X ppm/tgatoppm.c ppm/tgatoppm.1 ppm/ppmhist.c ppm/ppmhist.1 ppm/xwdtoppm.c ppm/xwdtoppm.1 ppm/ppmtoxwd.c ppm/ppmtoxwd.1 ppm/imgtoppm.c ppm/imgtoppm.1 > $@

pnm.shar:	pnm.shar1 pnm.shar2
pnm.shar1:	pnm/Makefile pnm/Imakefile pnm/pnm.5 pnm/libpnm.3 \
		pnm/libpnm1.c pnm/libpnm2.c pnm/libpnm3.c pnm/pnm.h \
		pnm/pnmcat.c pnm/pnmcat.1 pnm/pnmcrop.c pnm/pnmcrop.1 \
		pnm/pnmcut.c pnm/pnmcut.1
	shar -v -c -p X pnm/Makefile pnm/Imakefile pnm/pnm.5 pnm/libpnm.3 pnm/libpnm1.c pnm/libpnm2.c pnm/libpnm3.c pnm/pnm.h pnm/pnmcat.c pnm/pnmcat.1 pnm/pnmcrop.c pnm/pnmcrop.1 pnm/pnmcut.c pnm/pnmcut.1 > $@
pnm.shar2:	pnm/pnmenlarge.c pnm/pnmenlarge.1 pnm/pnmflip.c pnm/pnmflip.1 \
		pnm/pnminvert.c pnm/pnminvert.1 pnm/pnmpaste.c pnm/pnmpaste.1 \
		pnm/pnmtile.c pnm/pnmtile.1
	shar -v -c -p X pnm/pnmflip.c pnm/pnmflip.1 pnm/pnminvert.c pnm/pnminvert.1 pnm/pnmpaste.c pnm/pnmpaste.1 pnm/pnmtile.c pnm/pnmtile.1 pnm/pnmenlarge.c pnm/pnmenlarge.1 > $@


# Imakefile stuff.  Ignore if you're not an X11 type.

            TOP = ../../../../../usr/src/new/X11

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
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
	echo "Making Makefiles in $$i..."; \
	make subdirMakefiles NEWTOP=../ MAKEFILE_SUBDIR=$$i;\
	done

subdirMakefiles:
	$(RM) $(MAKEFILE_SUBDIR)/Makefile.bak
	-@if [ -f $(MAKEFILE_SUBDIR)/Makefile ]; then \
	echo "$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak"; \
	$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak; \
	else exit 0; fi
	cd $(MAKEFILE_SUBDIR); $(IMAKE_CMD) -DTOPDIR=../$(TOP); \
	make $(MFLAGS) Makefiles
