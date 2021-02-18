#
#	Makefile for jserver.
#
#	version 4.0
#
#
# If you change WNN_LIB or JSERVER_LIB, you should change "include/config.h".
# 
 
JSERVER_OWNER = wnn		#The user-id of jserver
#FORKDEF = -DNOTFORK		
#		Jserver usually fork and remove association with controll
#		terminal. -DNOTFORK is for DEBUG.
#
# Comment the following BINDIR and LIBDIR out when use ../Makefile
#
SHELL = /bin/sh
BINDIR = /usr/local/bin
LIBDIR = /usr/local/lib/wnn

JSERVER_DIR = /usr/local/lib/wnn/dic/

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

#DEBUG = -DDEBUG

CHDR	= ../include/commonhd.h
DHDR	= ddefine.h de_header.h
JS_CONF	= ../include/jservercf.h
DEMHDR	= ../include/demcom.h	
JH	= ../include/jh.h
JD	= ../include/jdata.h			
HINSI_FILE = ../include/hinsi_file.h
#
#
#		Make parameters for each machine.
#
#
##	make with ../Makefile
CFLAGS = -I../include ${DEFS} -DJSERVER
LIBS = ${LIB_TERM}

##	for SUN
#CFLAGS = -I../include -DBSD42 -DTERMCAP -DSUN -DJSERVER -g	#SUN Inet
#LIBS =

##	for BSD 4.2/4.3
#CFLAGS = -I../include -DBSD42 -DTERMCAP -DJSERVER		#BSD Inet
#LIBS =

##	for SYSVR2
#CFLAGS = -I../include -DSYSVR2 -DTERMINFO -DJSERVER		#SYS VR2
#LIBS =

##	for SX9100 SYSVR2 (UniOS U)
#CFLAGS = -Zs -I../include -DSX8870 -DJSERVER -DINET -g		#SX9100
#LIBS = 

##	for SX9100 BSD (UniOS B)
#CFLAGS = -20 -f -I../include -DSX9100BSD -DJSERVER -DINET -g	#SX9100 BSD
#LIBS = 

##	for NEC EWS4800
#CFLAGS = -I../include -DSYSVR2 -DTERMINFO -DNEC -DJSERVER
#LIBS =

###
ALLHEADER = \
	ddefine.h fzk.h kaiseki.h de_header.h

ALLSRC = \
	../jlib/getopt.c ../jlib/wnnerrmsg.c \
	../etc/strings.c ../etc/sstrings.c ../etc/bdic.c ../etc/dic_atojis.c\
	../etc/gethinsi.c ../etc/revdic.c ../etc/hindo.c\
	../etc/pwd.c\
	../etc/bcopy.c \
	de.c de_vars.c \
	dispatch.c do_dic_env.c do_dic_no.c do_filecom.c \
	error.c \
	jbiki.c jishoop.c  rd_jishoop.c \
	readfile.c jishosub.c \
	jmt0.c \
	malloc.c \
	kai_area.c \
	mknode0.c mknode1.c mknode2.c \
	renbn_kai.c bnsetu_kai.c daibn_kai.c \
	jikouho.c jikouho_d.c fzk.c \
	atojis.c \
	get_kaiarea.c \
	snd_rcv.c \
	do_henkan.c do_henkan1.c \
	do_env.c \
	do_hindo_s.c initjserv.c \
	hinsi_list.c
## dictoasc.c 

ALLOBJECTS = \
	getopt.o wnnerrmsg.o \
	strings.o sstrings.o bdic.o dic_atojis.o \
	gethinsi.o revdic.o hindo.o\
	pwd.o\
	bcopy.o \
	de.o de_vars.o \
	dispatch.o do_dic_env.o do_dic_no.o do_filecom.o \
	error.o \
	jbiki.o jishoop.o rd_jishoop.o \
	readfile.o jishosub.o \
	jmt0.o \
	malloc.o \
	kai_area.o \
	mknode0.o mknode1.o mknode2.o \
	renbn_kai.o bnsetu_kai.o daibn_kai.o \
	jikouho.o jikouho_d.o fzk.o \
	atojis.o \
	get_kaiarea.o \
	snd_rcv.o \
	do_henkan.o do_henkan1.o \
	do_env.o \
	do_hindo_s.o initjserv.o \
	hinsi_list.o
###dictoasc.o 
###

all: jserver


jserver: ${ALLOBJECTS} 
	${CC} ${CFLAGS} ${DEBUG} -o jserver ${ALLOBJECTS} ${LIBS}
	chmod u+s jserver


${CHDR}:	../include/wnnerror.h

de.o: de.c ${CHDR} ${DHDR}
	${CC} -c ${CFLAGS} ${DEBUG} ${FORKDEF} de.c

atojis.o:	${CHDR} ${DHDR} kaiseki.h
bnsetu_kai.o:	${CHDR} ${DHDR} ${JD} fzk.h kaiseki.h
daibn_kai.o:	${CHDR} ${DHDR} kaiseki.h fzk.h
de.o:		${CHDR} de_header.h ../include/jd_sock.h ${DEMHDR} ${JS_CONF}
de_vars.o:	${CHDR} ${DHDR} 
##dictoasc.o:	${CHDR} de_header.h ${JD} ${JH}  ##
dispatch.o:	${CHDR} de_header.h ${DEMHDR}
do_dic_env.o:	${CHDR} ${DHDR} ${JD} ${DEMHDR} 
do_dic_no.o:	${CHDR} ${DHDR} ${JD} kaiseki.h ${DEMHDR} 
do_env.o:	${CHDR} de_header.h ${DEMHDR}
do_filecom.o:	${CHDR} de_header.h ${JD} ddefine.h ${DEMHDR}
do_henkan.o:	${CHDR} ddefine.h ${JD} kaiseki.h
	${CC} -c ${CFLAGS} do_henkan.c ${DEBUG}

do_henkan1.o:	${CHDR} ${DHDR} ${JD} ${DEMHDR} fzk.h kaiseki.h
	${CC} -c ${CFLAGS} do_henkan1.c ${DEBUG}

do_hindo_s.o:	${CHDR} ${DHDR} ${JD} kaiseki.h
error.o:	${CHDR} ${DHDR} ${JS_CONF}
	${CC} -c ${CFLAGS} error.c ${DEBUG}

fzk.o:		${CHDR} ${JS_CONF} ${DHDR} fzk.h kaiseki.h
get_kaiarea.o: ${CHDR} ${DHDR} ${JD} ${JS_CONF} kaiseki.h
initjserv.o:	${CHDR} ${DHDR} ${JS_CONF}
jbiki.o:	${CHDR} ${DHDR} ${JD}  kaiseki.h ${JH}
jikouho.o:	${CHDR} ${DHDR} ${JD} fzk.h kaiseki.h
jikouho_d.o:	${CHDR} ${DHDR} ${JD} fzk.h kaiseki.h
jishoop.o:	${CHDR} de_header.h ${JD} ${JH}
rd_jishoop.o:	${CHDR} de_header.h ${JD} ${JH}
jishosub.o:	${CHDR} de_header.h ${JD} ${JH}
jmt0.o:		${CHDR} ddefine.h ${JD} kaiseki.h
kai_area.o:	${CHDR} ddefine.h
mknode0.o:	${CHDR} ${DHDR} kaiseki.h
mknode1.o:	${CHDR} ${DHDR} kaiseki.h
mknode2.o:	${CHDR} ${DHDR} kaiseki.h
readfile.o:	${CHDR} de_header.h ${JD} ${JH}
renbn_kai.o:	${CHDR} ${DHDR} fzk.h kaiseki.h
hinsi_list.o:	${CHDR} ${DHDR} ${JD} ${HINSI_FILE}

version.o : s.version.c
	get -p s.version.c > version.c
	${CC} -c version.c

getopt.o: ../jlib/getopt.c ${CHDR} ${DHDR}
	${CC} -c ${CFLAGS} ${FORKDEF} ../jlib/getopt.c

wnnerrmsg.o: ../jlib/wnnerrmsg.c ${CHDR} ${DHDR}
	${CC} -c ${CFLAGS} ${FORKDEF} ../jlib/wnnerrmsg.c

bdic.o: ../etc/bdic.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/bdic.c

dic_atojis.o: ../etc/dic_atojis.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/dic_atojis.c

strings.o: ../etc/strings.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/strings.c

sstrings.o: ../etc/sstrings.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/sstrings.c

gethinsi.o: ../etc/gethinsi.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/gethinsi.c

revdic.o: ../etc/revdic.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/revdic.c

hindo.o: ../etc/hindo.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/hindo.c

pwd.o: ../etc/pwd.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/pwd.c

bcopy.o: ../etc/bcopy.c
	${CC} ${CFLAGS} ${DEBUG} -c ../etc/bcopy.c

Makefile:: Imakefile \
	$(IRULESRC)/Imake.tmpl \
	$(IRULESRC)/Imake.rules \
	$(IRULESRC)/site.def
	-@if [ -f Makefile ]; then \
	echo "$(RM) Makefile.bak; $(MV) Makefile Makefile.bak"; \
	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP)

#
#
#
clean:
	rm -f core a.out  *.o *~ *.orig *.rej #*

purge:
	rm -f jserver core a.out  *.o *~ *.orig *.rej #*

cflow:
	cflow -D${FZKDEF} -I../include -DSX8870 ${ALLSRC}

lint:
	lint -n -D${FZKDEF} -DBSD42 -DTERMCAP -DJSERVER	 -I../include -DSX8870 ${ALLSRC}

ctags:
	ctags ${ALLSRC} ${ALLHEADER}


#
#	Wnn Version 4.0
#
#
install:	instjserver
#
#	install jserver
#
instjserver: $(BINDIR) $(LIBDIR) $(JSERVER_DIR)
	-cp jserver $(BINDIR)
	strip $(BINDIR)/jserver
	echo chown $(JSERVER_OWNER) $(BINDIR)/jserver
	-@if test -f /etc/chown ; then \
	    /etc/chown $(JSERVER_OWNER) $(BINDIR)/jserver ; \
	else \
	    /bin/chown $(JSERVER_OWNER) $(BINDIR)/jserver ; \
	fi
	chmod u+s $(BINDIR)/jserver
	-cp ../jd/jserverrc* $(LIBDIR)
	-cp ../jd/hinsi.data $(LIBDIR)
#
#	dirctory check
#
$(BINDIR):
	@echo Please mkdir $(BINDIR)
	@false

$(LIBDIR):
	@echo Please mkdir $(LIBDIR)
	@false

$(JSERVER_DIR):
	@echo Please mkdir $(JSERVER_DIR)
	@false


