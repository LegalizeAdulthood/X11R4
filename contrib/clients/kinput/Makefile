# Makefile generated by imake - do not edit!
# $XConsortium: imake.c,v 1.41 89/02/21 11:42:35 jim Exp $

###########################################################################
# X Window System Makefile generated from template file Imake.tmpl
# $XConsortium: Imake.tmpl,v 1.91 88/10/23 22:37:10 jim Exp $
#
# Do not change the body of the imake template file.  Server-specific
# parameters may be set in the appropriate .macros file; site-specific
# parameters (but shared by all servers) may be set in site.def.  If you
# make any changes, you'll need to rebuild the makefiles using
# "make World" (at best) or "make Makefile; make Makefiles" (at least) in
# the top level directory.
#
# If your C preprocessor doesn't define any unique symbols, you'll need
# to set BOOTSTRAPCFLAGS when rebuilding imake (usually when doing
# "make Makefile", "make Makefiles", or "make World").
#
# If you absolutely can't get imake to work, you'll need to set the
# variables at the top of each Makefile as well as the dependencies at the
# bottom (makedepend will do this automatically).
#

###########################################################################
# platform-specific configuration parameters - edit Sony.macros to change

# platform:  $XConsortium: Sun.macros,v 1.52 88/10/23 11:00:55 jim Exp $
# operating system:  OSName

BOOTSTRAPCFLAGS =
             AS = as
             CC = cc
            CPP = /lib/cpp
             LD = ld
           LINT = lint
        INSTALL = install
           TAGS = ctags
             RM = rm -f
             MV = mv
             LN = ln -s
         RANLIB = ranlib
RANLIBINSTFLAGS = -t
             AR = ar clq
             LS = ls
       LINTOPTS = -axz
    LINTLIBFLAG = -C
           MAKE = make
STD_CPP_DEFINES =
    STD_DEFINES =

###########################################################################
# site-specific configuration parameters - edit site.def to change

# site:  $XConsortium: site.def,v 1.16 88/10/12 10:30:24 jim Exp $

###########################################################################
# definitions common to all Makefiles - do not edit

          SHELL = 	/bin/sh

        DESTDIR =
      USRLIBDIR = $(DESTDIR)/usr/lib
         BINDIR = $(DESTDIR)/usr/bin/X11
         INCDIR = $(DESTDIR)/usr/include/X11
         ADMDIR = $(DESTDIR)/usr/adm
         LIBDIR = $(USRLIBDIR)/X11
     LINTLIBDIR = $(USRLIBDIR)/lint
        FONTDIR = $(LIBDIR)/fonts
       XINITDIR = $(LIBDIR)/xinit
         XDMDIR = $(LIBDIR)/xdm
         UWMDIR = $(LIBDIR)/uwm
         AWMDIR = $(LIBDIR)/awm
         TWMDIR = $(LIBDIR)/twm
        MANPATH = $(DESTDIR)/usr/man
  MANSOURCEPATH = $(MANPATH)/man
         MANDIR = $(MANSOURCEPATH)n
      LIBMANDIR = $(MANSOURCEPATH)3
    XAPPLOADDIR = $(LIBDIR)/app-defaults

   INSTBINFLAGS = -m 0755
   INSTUIDFLAGS = -m 4755
   INSTLIBFLAGS = -m 0664
   INSTINCFLAGS = -m 0444
   INSTMANFLAGS = -m 0444
   INSTAPPFLAGS = -m 0444
  INSTKMEMFLAGS = -m 4755
        FCFLAGS = -t
    CDEBUGFLAGS = -O

        PATHSEP = /
         DEPEND = $(DEPENDSRC)/makedepend
          IMAKE = $(IMAKESRC)/imake
            RGB = $(RGBSRC)/rgb
             FC = $(BDFTOSNFSRC)/bdftosnf
      MKFONTDIR = $(MKFONTDIRSRC)/mkfontdir
      MKDIRHIER = $(SCRIPTSSRC)/mkdirhier.sh

         CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES) $(DEFINES)
      LINTFLAGS = $(LINTOPTS) $(INCLUDES) $(STD_DEFINES) $(DEFINES) -DLINT
        LDFLAGS = $(CDEBUGFLAGS) $(SYS_LIBRARIES) $(SYSAUX_LIBRARIES)
            TOP = 		/usr/src/X11
      CLIENTSRC = $(TOP)/clients
        DEMOSRC = $(TOP)/demos
         LIBSRC = $(TOP)/lib
        FONTSRC = $(TOP)/fonts
     INCLUDESRC = $(TOP)/X11
      SERVERSRC = $(TOP)/server
        UTILSRC = $(TOP)/util
     SCRIPTSSRC = $(UTILSRC)/scripts
     EXAMPLESRC = $(TOP)/examples
     CONTRIBSRC = $(TOP)/contrib
         DOCSRC = $(TOP)/doc
         RGBSRC = $(TOP)/rgb
      DEPENDSRC = $(UTILSRC)/makedepend
       IMAKESRC = $(UTILSRC)/imake
       IRULESRC = $(UTILSRC)/imake.includes
        XLIBSRC = $(LIBSRC)/X
         XMUSRC = $(LIBSRC)/Xmu
     TOOLKITSRC = $(LIBSRC)/Xt
     AWIDGETSRC = $(LIBSRC)/Xaw
     OLDXLIBSRC = $(LIBSRC)/oldX
    BDFTOSNFSRC = $(FONTSRC)/bdftosnf
   MKFONTDIRSRC = $(FONTSRC)/mkfontdir
   EXTENSIONSRC = $(TOP)/extensions
   EXTENSIONLIB = $(USRLIBDIR)/libXext.a
           XLIB = $(USRLIBDIR)/libX11.a
         XMULIB = $(USRLIBDIR)/libXmu.a
        OLDXLIB = $(USRLIBDIR)/liboldX.a
       XTOOLLIB = $(USRLIBDIR)/libXt.a
         XAWLIB = $(USRLIBDIR)/libXaw.a
       LINTXLIB = $(LINTLIBDIR)/llib-lX11.ln
        LINTXMU = $(LINTLIBDIR)/llib-lXmu.ln
      LINTXTOOL = $(LINTLIBDIR)/llib-lXt.ln
        LINTXAW = $(LINTLIBDIR)/llib-lXaw.ln
       INCLUDES = -I$(TOP)
      MACROFILE = Sony.macros
   ICONFIGFILES = $(IRULESRC)/Imake.tmpl \
			$(IRULESRC)/$(MACROFILE) $(IRULESRC)/site.def
  IMAKE_DEFINES =
      IMAKE_CMD = $(NEWTOP)$(IMAKE) -TImake.tmpl -I$(NEWTOP)$(IRULESRC) \
			-s Makefile $(IMAKE_DEFINES)
         RM_CMD = $(RM) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a \
			.emacs_* tags TAGS make.log MakeOut

###########################################################################
# rules:  $XConsortium: Imake.rules,v 1.71 88/10/23 22:46:34 jim Exp $

###########################################################################
# start of Imakefile

           WNNSRC = $(CONTRIBSRC)/lib/Wnn

          SUBDIRS = cconv jclib xlocal

     STD_INCLUDES = -I$(TOOLKITSRC) -I$(AWIDGETSRC) -I$(TOP)
     WNN_INCLUDES = -I$(WNNSRC)/include
   LOCAL_INCLUDES = -I. -I./jclib -I./cconv -I./xlocal
         INCLUDES = $(LOCAL_INCLUDES) $(WNN_INCLUDES) $(STD_INCLUDES)

           STDLIB = $(XAWLIB) $(XMULIB) $(XTOOLLIB) $(XLIB)
            JCLIB = jclib/libjc.a
            CCLIB = cconv/libcc.a
        XLOCALLIB = xlocal/libXlocal.a
           WNNLIB = $(WNNSRC)/jlib/libwnn.a
  LOCAL_LIBRARIES = $(XLOCALLIB) $(JCLIB) $(CCLIB) $(WNNLIB) $(STDLIB)

          DEFINES = -DHEADER_IS_HERE

             SRCS = KanjiInput.c KISub.c kinput.c confirm.c
             OBJS = KanjiInput.o KISub.o kinput.o confirm.o

all::
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
		(cd $$i ; echo "making all in $$i"; \
			$(MAKE) 'WNNSRC=$(WNNSRC)' 'CDEBUGFLAGS=$(CDEBUGFLAGS)' $(MFLAGS)); \
	done

depend::
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
		(cd $$i ; echo "depending in $$i"; \
			$(MAKE) $(MFLAGS) depend ); \
	done

        PROGRAM = kinput

all:: kinput

kinput: $(OBJS) $(LOCAL_LIBRARIES)
	$(RM) $@
	$(CC) -o $@ $(OBJS) $(LOCAL_LIBRARIES) $(LDFLAGS) $(SYSLAST_LIBRARIES)

relink::
	$(RM) $(PROGRAM)
	$(MAKE) $(MFLAGS) $(PROGRAM)

install:: kinput
	$(INSTALL) -c $(INSTALLFLAGS) kinput $(BINDIR)

install.man:: kinput.man
	$(INSTALL) -c $(INSTMANFLAGS) kinput.man $(MANDIR)/kinput.n

depend:: $(DEPEND)

depend::
	$(DEPEND) -s "# DO NOT DELETE" -- $(CFLAGS) -- $(SRCS)

$(DEPEND):
	@echo "making $@"; \
	cd $(DEPENDSRC); $(MAKE)

clean::
	$(RM) $(PROGRAM)

install.man:: kinput.man
	$(INSTALL) -c $(INSTMANFLAGS) kinput.man $(MANDIR)/kinput.n

###########################################################################
# Imake.tmpl common rules for all Makefiles - do not edit

emptyrule::

clean::
	$(RM_CMD) \#*

Makefile:: $(IMAKE)

Makefile:: Imakefile \
	$(IRULESRC)/Imake.tmpl \
	$(IRULESRC)/Imake.rules \
	$(IRULESRC)/site.def \
	$(IRULESRC)/$(MACROFILE)
	-@if [ -f Makefile ]; then \
		echo "$(RM) Makefile.bak; $(MV) Makefile Makefile.bak"; \
		$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP)

$(IMAKE):
	@echo "making $@"; \
	cd $(IMAKESRC); $(MAKE) BOOTSTRAPCFLAGS=$(BOOTSTRAPCFLAGS)

tags::
	$(TAGS) -w *.[ch]
	$(TAGS) -xw *.[ch] > TAGS

###########################################################################
# rules for building in SUBDIRS - do not edit

install::
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
		(cd $$i ; echo "installing in $$i"; \
			$(MAKE) $(MFLAGS) \
				DESTDIR='$(DESTDIR)' install ); \
	done

install.man::
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
		(cd $$i ; echo "installing man pages in $$i"; \
			$(MAKE) $(MFLAGS) \
				DESTDIR='$(DESTDIR)' install.man ); \
	done

clean::
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
		(cd $$i ; echo "cleaning in $$i"; \
			$(MAKE) $(MFLAGS) RM_CMD='$(RM_CMD)' clean ); \
	done

tags::
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
		(cd $$i ; echo "tagging in $$i"; \
			$(MAKE) $(MFLAGS) TAGS='$(TAGS)' tags ); \
	done

Makefiles:: $(IMAKE)

Makefiles::
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS) ;\
	do \
		echo "Making Makefiles in $$i..."; \
		case "$$i" in \
		./?*/?*/?*/?*) newtop=../../../../ sub=subsubsubsub;; \
		./?*/?*/?*) newtop=../../../ sub=subsubsub;; \
		./?*/?*)    newtop=../../ sub=subsub;; \
		./?*)       newtop=../ sub=sub;; \
		*/?*/?*/?*)    newtop=../../../../ sub=subsubsubsub;; \
		*/?*/?*)    newtop=../../../ sub=subsubsub;; \
		*/?*)       newtop=../../ sub=subsub;; \
		*)	    newtop=../ sub=sub;; \
		esac; \
		$(MAKE) $${sub}dirMakefiles NEWTOP=$$newtop \
			MAKEFILE_SUBDIR=$$i;\
	done

subdirMakefiles:
	$(RM) $(MAKEFILE_SUBDIR)/Makefile.bak
	-@if [ -f $(MAKEFILE_SUBDIR)/Makefile ]; then \
	echo "$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak"; \
	$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak; \
	else exit 0; fi
	cd $(MAKEFILE_SUBDIR); $(IMAKE_CMD) -DTOPDIR=../$(TOP); \
	$(MAKE) $(MFLAGS) Makefiles

subsubdirMakefiles:
	$(RM) $(MAKEFILE_SUBDIR)/Makefile.bak
	-@if [ -f $(MAKEFILE_SUBDIR)/Makefile ]; then \
	echo "$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak"; \
	$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak; \
	else exit 0; fi
	cd $(MAKEFILE_SUBDIR); $(IMAKE_CMD) -DTOPDIR=../../$(TOP); \
	$(MAKE) $(MFLAGS) Makefiles

subsubsubdirMakefiles:
	$(RM) $(MAKEFILE_SUBDIR)/Makefile.bak
	-@if [ -f $(MAKEFILE_SUBDIR)/Makefile ]; then \
	echo "$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak"; \
	$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak; \
	else exit 0; fi
	cd $(MAKEFILE_SUBDIR); $(IMAKE_CMD) -DTOPDIR=../../../$(TOP); \
	$(MAKE) $(MFLAGS) Makefiles

subsubsubsubdirMakefiles:
	$(RM) $(MAKEFILE_SUBDIR)/Makefile.bak
	-@if [ -f $(MAKEFILE_SUBDIR)/Makefile ]; then \
	echo "$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak"; \
	$(MV) $(MAKEFILE_SUBDIR)/Makefile $(MAKEFILE_SUBDIR)/Makefile.bak; \
	else exit 0; fi
	cd $(MAKEFILE_SUBDIR); $(IMAKE_CMD) -DTOPDIR=../../../../$(TOP); \
	$(MAKE) $(MFLAGS) Makefiles

###########################################################################
# dependencies generated by makedepend

