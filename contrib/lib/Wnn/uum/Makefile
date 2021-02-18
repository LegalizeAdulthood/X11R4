#
#	Makefile for uum
#
#	version 4.0
#
#
# If you change WNN_LIB or JSERVER_LIB, you should  change "include/config.h".
# 
#
# Comment the following BINDIR and LIBDIR out when use ../Makefile
#
SHELL = /bin/sh
MAKE = make
#BINDIR = /usr/local/bin/Wnn4
#LIBDIR = /usr/local/lib/wnn

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

##	make with ../Makefile
CFLAGS = ${DEFS} -I../include
LIBS = ../jlib/libwnn.a ../jlib/libconvkey.a ${LIB_TERM} ${GETOPTLIB}
##

##	for SUN
#CFLAGS = -g  -DBSD42 -DTERMCAP -DSUN -I../include
#LIBS = ../jlib/libwnn.a ../jlib/libconvkey.a -ltermcap ${GETOPTLIB}
##

##	for BSD42
#CFLAGS = -g  -DBSD42 -DTERMCAP -I../include
#LIBS = ../jlib/libwnn.a ../jlib/libconvkey.a -ltermcap ${GETOPTLIB}
##

##	for BSD43 (VAX Ultrix)
#CFLAGS = -g  -DBSD43 -DBSD42 -DTERMCAP -I../include
#LIBS = ../jlib/libwnn.a ../jlib/libconvkey.a -ltermcap ${GETOPTLIB}
##

##	for SYSVR2
#CFLAGS =  -O -DSYSVR2 -DTERMINFO -I../include -I../romkan
#LIBS = ../jlib/libwnn.a ../jlib/libconvkey.a ${GETOPTLIB} -lcurses
##

##	for SX9100 (UniOS U)
#CFLAGS =  -O -Zs -DSX8870 -I../include -I../romkan
#CFLAGS =  -g -Zs -DSX8870 -I../include -I../romkan
#CFLAGS =  -O -Zs -DSX8870 -DOMRON -I../include -I../romkan
#LIBS = ../jlib/libwnn.a ../jlib/libconvkey.a ${GETOPTLIB} -lcurses
##

##	for SX9100 BSD (UniOS B)
#CFLAGS =  -O -f -20 -DSX9100BSD -I../include -I../romkan
#LIBS = ../jlib/libwnn.a ../jlib/libconvkey.a ${GETOPTLIB} 
##

##	for NEC EWS-4800
#CFLAGS =  -O -DNEC -DSYSVR2 -DTERMINFO -I../include -I../romkan
#LIBS = ../jlib/libwnn.a ../jlib/libconvkey.a ${GETOPTLIB} -lcurses
##

#TEMPORALY
#OTHERS = ../jlib/js.o ../jlib/jl.o\
#	 ../etc/strings.o ../etc/sstrings.o\
#	../jlib/wnnerrmsg.o \
#        ../romkan/rk_main.o     ../romkan/rk_read.o \
#        ../romkan/rk_modread.o  ../romkan/rk_bltinfn.o ../romkan/rk_vars.o \
#	../conv/convert_key.o ../conv/convert_read.o -ltermcap
#	../etc/bdic.o  ../etc/pwd.o
#	../etc/bcopy.o


OBJ =	epilogue.o header.o kensaku.o kuten.o jis_in.o \
	prologue.o touroku.o jikouho.o jutil.o w_string.o \
	printf.o termio.o hinsi.o termcap.o \
	ttyfdslot.o setutmp.o redraw.o history.o key_bind.o \
	screen.o basic_op.o uif.o cursor.o jhlp.o \
	select_ele.o inspect.o wnnrc_op.o \
	strings.o sstrings.o \
	bcopy.o xutoj.o
SRC =	epilogue.c header.c kensaku.c kuten.c jis_in.c \
	prologue.c touroku.c jikouho.c jutil.c w_string.c \
	printf.c termio.c hinsi.c termcap.c \
	ttyfdslot.c setutmp.c redraw.c history.c key_bind.c \
	screen.c basic_op.c uif.c cursor.c jhlp.c \
	select_ele.c inspect.c wnnrc_op.c \
	../etc/strings.c ../etc/sstrings.c \
	../etc/bcopy.c ../etc/xutoj.c

#CHDR = commonhd.h

SHDR = sdefine.h sheader.h
LHDR = ../include/jlib.h
DEFAULTS = ../include/config.h
BUFHDR = buffer.h
#LDFLAGS =  -g 
#LDFLAGS = -s


all: uum

uum:  ${OBJ} ../jlib/libonvkey.a ../jlib/libwnn.a
	$(CC) ${LDFLAGS} -o uum ${OBJ} ${LIBS}

../jlib/libonvkey.a:
	cd ../conv; ${MAKE} DEFS='$(DEFS)'

../jlib/libwnn.a:
	cd ../jlib; ${MAKE} DEFS='$(DEFS)' GETOPTLIB='$(GLIB)'

uif.o: ${CHDR} ${SHDR} ${LHDR} ${BUFHDR}  uif.c
screen.o: ${SHDR} ${CHDR} ${BUFHDR} screen.c
basic_op.o: ${SHDR} ${CHDR} ${BUFHDR} basic_op.c 
jhlp.o: ${SHDR} ${CHDR} ${DEFAULTS} jhlp.c 
test.o: ${CHDR} ${CHDR} test.c
epilogue.o: ${LHDR} ${CHDR} epilogue.c
hyouji1.o: ${CHDR} ${CHDR} hyouji1.c
kensaku.o: ${CHDR} ${SHDR} ${LHDR} kensaku.c
newuif.o: ${CHDR} ${SHDR} ${LHDR} newuif.c
header.o: ${CHDR} ${LHDR} ${SHDR} ${BUFHDR} header.c
prologue.o: ${CHDR} ${LHDR} ${SHDR} ${DEFAULTS}  prologue.c
jikouho.o: ${CHDR} ${LHDR} ${SHDR} jikouho.c
hyouji.o: ${CHDR} ${SHDR} hyouji.c
kuten.o: ${CHDR} ${SHDR} kuten.c
newkuten.o: ${CHDR} ${SHDR} newkuten.c
jis_in.o: ${CHDR} ${SHDR} jis_in.c
touroku.o: ${CHDR} ${LHDR} ${SHDR} ${BUFHDR} touroku.c
jutil.o: ${CHDR} ${SHDR} jutil.c
hinsi.o: ${CHDR} ${SHDR} hinsi.c
w_string.o: ${CHDR} w_string.c

hyouji_kari.o: ${CHDR} hyouji_kari.c
initsock.o: ${CHDR} initsock.c
jistoujis.o: ${CHDR} jistoujis.c
ujistojis.o: ${CHDR} ujistojis.c
printf.o: ${CHDR} printf.c
flush.o: ${CHDR} flush.c
termio.o: ${CHDR} termio.c
setutmp.o: setutmp.c 
ttyfdslot.o: ttyfdslot.c
termcap.o:${CHDR} termcap.c
redraw.o: ${CHDR} redraw.c
cursor.o: ${CHDR} cursor.c
select_ele.o: ${CHDR} ${LHDR} ${SHDR} select_ele.c
inspect.o: ${CHDR} ${LHDR} ${SHDR} inspect.c
history.o: history.c
key_bind.o: key_bind.c
wnnrc_op.o: wnnrc_op.c
strings.o: ../etc/strings.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/strings.c

sstrings.o: ../etc/sstrings.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/sstrings.c

bcopy.o: ../etc/bcopy.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/bcopy.c

xutoj.o: ../etc/xutoj.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/xutoj.c

Makefile:: Imakefile \
	$(IRULESRC)/Imake.tmpl \
	$(IRULESRC)/Imake.rules \
	$(IRULESRC)/site.def
	-@if [ -f Makefile ]; then \
	echo "$(RM) Makefile.bak; $(MV) Makefile Makefile.bak"; \
	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP)

#TEMPORALY

clean:
	rm -f core a.out *.o *~ *.orig *.rej #*

purge:
	rm -f uum core a.out *.o *~ *.orig *.rej #*

cflow:
	cflow -DSX8870 -I../include ${SRC}

sxlint:
	lint -abuhx -DSX8870 -I../include ${SRC}

sunlint:
	lint -abuhx -DSUN -DINET -I../include ${SRC}

#
#	Wnn Version 4.0
#
#
#	install uum
#
install: $(BINDIR) $(LIBDIR) $(LIBDIR)/rk $(LIBDIR)/rk.vi
	-cp uum			$(BINDIR)
	strip $(BINDIR)/uum
	-su root -c 'chmod 4775		$(BINDIR)/uum'
	-cp ../jd/uumrc*	$(LIBDIR)
	-cp ../jd/uumkey*	$(LIBDIR)
	-cp ../jd/wnnenvrc*	$(LIBDIR)
	-cp ../jd/rk/*		$(LIBDIR)/rk
	-cp ../jd/rk.vi/*	$(LIBDIR)/rk.vi
	-@if test "$(LIB_TERM)" = "-ltermcap" ; then \
		echo "cp ../jd/cvt_key_*	$(LIBDIR)" ; \
		cp ../jd/cvt_key_*	$(LIBDIR) ; \
		echo 'su root -c chown root	$(BINDIR)/uum' ; \
		su root -c '/etc/chown root	$(BINDIR)/uum' ; \
	else \
		echo "	cp ../jd/sysV/cvt_key_*	$(LIBDIR)" ; \
		cp ../jd/sysV/cvt_key_*	$(LIBDIR) ; \
		echo '	su root -c chown root	$(BINDIR)/uum' ; \
		su root -c 'chown root	$(BINDIR)/uum' ; \
	fi

#
#	dirctory check
#

$(BINDIR):
	@echo Please mkdir $(BINDIR)
	@false

$(LIBDIR):
	@echo Please mkdir $(LIBDIR)
	@false

$(LIBDIR)/rk:
	@echo Please mkdir $(LIBDIR)/rk
	@false

$(LIBDIR)/rk.vi:
	@echo Please mkdir $(LIBDIR)/rk.vi
	@false
