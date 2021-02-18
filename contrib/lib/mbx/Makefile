#
# Warning: the cpp used on this machine replaces
# all newlines and multiple tabs/spaces in a macro
# expansion with a single space.  Imake tries to
# compensate for this, but is not always
# successful.
#

#
#                         WARNING to imake users
#
# This makefile is generated automatically by the imake program.  Do not
# modify it or you will lose your changes the next time imake is run.  Read
# the INSTALLATION GUIDE and look at the Imake.tmpl and *.macros files in
# the directory ./util/imake.includes/ if you need to change any of the
# configuration parameters.
#
# You may ignore this message if you are not using imake.
#

            TOP = ../../.
             AS = as
            CPP = /lib/cpp
             LD = ld
           LINT = lint
           MAKE = make
        DESTDIR =
        INSTALL = install
           TAGS = ctags
             RM = rm -f
             MV = mv
             LS = ls
             CP = cp
    STD_DEFINES =

          SHELL = /bin/sh
             CC = cc
    CDEBUGFLAGS =
 LDCOMBINEFLAGS = -r
SYSAUX_LIBRARIES = -lbsd
    STD_DEFINES = -DAIXrt -Dibm032 -D_pcc_ -Nd4000 -Nn3000 -DBSD_INCLUDES -DAIXEXTENSIONS
             LN = ln
         RANLIB = echo ranlib
             AR = ar cq
       LINTOPTS = -axz
    LINTLIBFLAG = -C

        PATHSEP = /
         DEPEND = $(DEPENDSRC)/makedepend
          IMAKE = $(IMAKESRC)/imake
            RGB = $(RGBSRC)/rgb
         CFLAGS = $(CDEBUGFLAGS) $(INCLUDES) $(STD_DEFINES) $(DEFINES)
      LINTFLAGS = $(LINTOPTS) $(INCLUDES) $(STD_DEFINES) $(DEFINES) -DLINT
        LDFLAGS = $(CDEBUGFLAGS) $(SYS_LIBRARIES) $(SYSAUX_LIBRARIES)
   INSTUIDFLAGS = -m 4755
   INSTLIBFLAGS = -m 0664
   INSTINCFLAGS = -m 0444
   INSTMANFLAGS = -m 0444
   INSTAPPFLAGS = -m 0444

      USRLIBDIR = $(DESTDIR)/usr/lib
     INSTALLDIR = /usr/bin/X11
         BINDIR = $(DESTDIR)$(INSTALLDIR)
         LIBDIR = $(USRLIBDIR)/X11
     LINTLIBDIR = $(USRLIBDIR)/lint
         INCDIR = $(DESTDIR)/usr/include/X11
        FONTDIR = $(LIBDIR)/fonts
         UWMDIR = $(LIBDIR)/uwm
         AWMDIR = $(LIBDIR)/awm
        MANPATH = $(DESTDIR)/usr/man
         MANDIR = $(MANPATH)/mann
      LIBMANDIR = $(MANPATH)/manLibManSuffix
    XAPPLOADDIR = $(LIBDIR)/app-defaults
         ADMDIR = $(DESTDIR)/usr/adm

      CLIENTSRC = $(TOP)/clients
        DEMOSRC = $(TOP)/demos
         LIBSRC = $(TOP)/lib
        FONTSRC = $(TOP)/fonts
     INCLUDESRC = $(TOP)/X11
      SERVERSRC = $(TOP)/server
        UTILSRC = $(TOP)/util
     EXAMPLESRC = $(TOP)/examples
     CONTRIBSRC = $(TOP)/contrib
         DOCSRC = $(TOP)/doc
      DEPENDSRC = $(UTILSRC)/makedepend
       IMAKESRC = $(UTILSRC)/imake
       IRULESRC = $(UTILSRC)/imake.includes
         RGBSRC = $(UTILSRC)/rgb
        XLIBSRC = $(LIBSRC)/X
     TOOLKITSRC = $(LIBSRC)/Xt
     AWIDGETSRC = $(LIBSRC)/Xaw
     OLDXLIBSRC = $(LIBSRC)/oldX
   EXTENSIONSRC = $(TOP)/extensions
        XMANSRC = $(DOCSRC)/Xlib/Xman
   EXTENSIONLIB = $(EXTENSIONSRC)/lib/libXext.a
           XLIB = $(XLIBSRC)/libX11.a
        OLDXLIB = $(OLDXLIBSRC)/liboldX.a
       XTOOLLIB = $(TOOLKITSRC)/libXt.a
         XAWLIB = $(AWIDGETSRC)/libXaw.a
       LINTXLIB = $(XLIBSRC)/llib-lX11.ln
      LINTXTOOL = $(TOOLKITSRC)/llib-lXt.ln
        LINTXAW = $(AWIDGETSRC)/llib-lXaw.ln
       INCLUDES = -I$(TOP)
      MACROFILE = AIXRt.macros
      IMAKE_CMD = $(NEWTOP)$(IMAKE) -TImake.tmpl \
			-I$(NEWTOP)$(IRULESRC) \
			-s Makefile
         RM_CMD = $(RM) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a \
			.emacs_* tags TAGS make.log MakeOut

#
# This library contains miscellaneous utility routines and is not part
# of the Xlib standard.
#

STD_DEFINES = -DAIXrt -Dibm032 -D_pcc_ -Nd4000 -Nn3000 -DBSD_INCLUDES -DAIXEXTENSIONS -DSYSV -DBSD_INCLUDES -D_NO_PROTO
CDEBUGFLAGS =
INCLUDES = -I. -I$(TOP) -I$(TOP)/X11 -I../include
   INSTALLFLAGS = $(INSTINCFLAGS)
       LINTLIBS = $(LINTXLIB)

HEADERS = \
	mbX.h

SRCS = mbX.c       \
       mbXDraw.c   \
       mbXText.c   \
       mbXLoad.c   \
       mbXibm.c

OBJS = mbX.o       \
       mbXDraw.o   \
       mbXText.o   \
       mbXLoad.o   \
       mbXibm.o

all::

.c.o:
	$(RM) $@
	$(CC) -c $(CFLAGS) $*.c

CvtStdSel.o: $(ICONFIGFILES)
	$(RM) $@
	$(CC) -c $(CFLAGS) $(OS_NAME_DEFINES) $*.c

all:: libXmu.a

libXmu.a: $(OBJS)
	$(RM) $@
	$(AR) $@ $(OBJS)
	$(RANLIB) $@

lintlib:: llib-lXmu.ln

llib-lXmu.ln: $(SRCS)
	$(RM) $@
	$(LINT) $(LINTLIBFLAG)Xmu $(LINTFLAGS) $(SRCS)

install:: libXmu.a
	$(INSTALL) -c $(INSTLIBFLAGS) libXmu.a $(USRLIBDIR)
	$(RANLIB) $(USRLIBDIR)/libXmu.a

install:: $(HEADERS)
	@case '${MFLAGS}' in *[i]*) set +e;; esac; \
	for i in $(HEADERS); do \
	(set -x; $(INSTALL) -c $(INSTALLFLAGS) $$i $(INCDIR)); \
	done

depend:: $(DEPEND)

depend::
	$(DEPEND) -s "# DO NOT DELETE" -- $(CFLAGS) -- $(SRCS)

$(DEPEND):
	@echo "making $(DEPENDSRC)"; \
	cd $(DEPENDSRC); $(MAKE)

lint:
	$(LINT) $(LINTFLAGS) $(SRCS) $(LINTLIBS)
lint1:
	$(LINT) $(LINTFLAGS) $(FILE) $(LINTLIBS)

UTILSRC=/u/jeanne/X/X.V11R3/util

clean::
	$(RM_CMD) \#*

Makefile:: $(IMAKE)

Makefile:: Imakefile \
	$(IRULESRC)/Imake.tmpl \
	$(IRULESRC)/Imake.rules \
	$(IRULESRC)/$(MACROFILE)
	-$(RM) Makefile.bak; $(MV) Makefile Makefile.bak
	$(IMAKE_CMD) -DTOPDIR=$(TOP)

$(IMAKE):
	@echo "making $(IMAKESRC)"; \
	cd $(IMAKESRC); $(MAKE) BOOTSTRAPCFLAGS=$(BOOTSTRAPCFLAGS)

tags::
	$(TAGS) -w *.[ch]
	$(TAGS) -xw *.[ch] > TAGS

install::
	@echo "install done"

Makefiles::

# DO NOT DELETE

mbX.o: locale.h mbX.h /usr/include/X11/Xlib.h ../include/sys/types.h
mbX.o: ../include/standards.h /usr/include/X11/X.h /usr/include/assert.h
mbXDraw.o: /usr/include/X11/Xlib.h ../include/sys/types.h
mbXDraw.o: ../include/standards.h /usr/include/X11/X.h locale.h mbX.h
mbXDraw.o: mbXCommon.c
mbXText.o: /usr/include/X11/Xlib.h ../include/sys/types.h
mbXText.o: ../include/standards.h /usr/include/X11/X.h locale.h mbX.h
mbXText.o: mbXCommon.c
mbXLoad.o: /usr/include/X11/Xlib.h ../include/sys/types.h
mbXLoad.o: ../include/standards.h /usr/include/X11/X.h locale.h
mbXLoad.o: /usr/include/ctype.h mbX.h
mbXibm.o: mbX.h /usr/include/X11/Xlib.h ../include/sys/types.h
mbXibm.o: ../include/standards.h /usr/include/X11/X.h mbXpc932.h
