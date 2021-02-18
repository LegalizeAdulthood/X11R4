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

            TOP = ../../..
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
    STD_DEFINES = -DAIXrt -Dibm032 -D_pcc_ -Nd4000 -Nn3000 -DAIXEXTENSIONS
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

#  Copyright International Business Machines, 1989
#
#
#
#  IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
#  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR ANY
#  PARTICULAR USE.  IN 0 EVENT SHALL
#  IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
#  ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
#  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
#  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
#  SOFTWARE
#
#
#
#
LOCAL_LIBRARIES = $(XLIB)
SRCS = run.c setup.c
OBJS = run.o setup.o ../gen/events.o ../gen/date.o ../gen/init.o \
       ../gen/main.o ../gen/options.o ../gen/seconds.o ../gen/timer.o
INCLUDES = -I../gen
TOP=../../..

 PROGRAM = charstones

all:: charstones

charstones: $(OBJS) $(LOCAL_LIBRARIES)
	$(RM) $@
	$(CC) -o $@ $(OBJS) $(LOCAL_LIBRARIES) $(LDFLAGS)
	$(LN) charstones ../bin/charstones

relink::
	$(RM) $(PROGRAM)
	$(MAKE) $(MFLAGS) $(PROGRAM)

install:: charstones
	$(INSTALL) -c $(INSTALLFLAGS) charstones $(BINDIR)

depend:: $(DEPEND)

depend::
	$(DEPEND) -s "# DO NOT DELETE" -- $(CFLAGS) -- $(SRCS)

$(DEPEND):
	@echo "making $(DEPENDSRC)"; \
	cd $(DEPENDSRC); $(MAKE)

clean::
	$(RM) $(PROGRAM)

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

