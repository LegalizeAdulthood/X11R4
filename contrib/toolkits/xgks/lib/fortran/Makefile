#
# 5799-WZQ (C) COPYRIGHT IBM CORPORATION 1987
# LICENSED MATERIALS - PROPERTY OF IBM
# REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
#
# $Header: Makefile,v 1.10 89/06/06 11:01:49 amy Exp $
# $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/Makefile,v $ 
# $Log:	Makefile,v $
# Revision 1.10  89/06/06  11:01:49  amy
# Put fortxgks.a in lib subdir with libxgks.a.
# 
# Revision 1.9  89/06/06  10:51:29  amy
# Removed -DDEBUG parm from flags.
# 
# Revision 1.8  89/06/06  10:50:15  amy
# Added -I parm to makedepend statement.
# 
# Revision 1.7  89/04/06  13:40:16  bruce
# Temporary change to test PTR# c2064 to see if it would compile.
# 
# Revision 1.6  89/02/13  15:44:15  admin
# PTR c2007	Added depend label and makedepend statement.
# 

INCLUDES = -I. -I/andrew/Xgks/source/xgks/include
#CFLAGS = $(INCLUDES) -O -DDEBUG
CFLAGS = $(INCLUDES) -O 
LIBDIR = /andrew/Xgks/source/xgks/lib

OBJS =	control.o \
        error.o \
        gerhnd.o\
	escapes.o \
        inputevent.o\
        inputinit.o \
        inputmode.o \
        inputreq.o \
        inputsamp.o \
        inqerrlist.o \
        inqgksdesc.o \
        inqgkslist.o \
	inqlun.o \
        inqpixel.o \
        inqseglist.o \
        inqstate.o \
        inqwsdesc.o \
        inqwslist.o \
        meta.o \
        output.o \
        rep.o \
        segattr.o \
        segment.o \
        transform.o \
        wsioutattr.o \
        utils.o 

SRC =	control.c \
        error.c \
        gerhnd.c\
	escapes.c \
        inputevent.c\
        inputinit.c \
        inputmode.c \
        inputreq.c \
        inputsamp.c \
        inqerrlist.c \
        inqgksdesc.c \
        inqgkslist.c \
	inqlun.f \
        inqpixel.c \
        inqseglist.c \
        inqstate.c \
        inqwsdesc.c \
        inqwslist.c \
        meta.c \
        output.c \
        rep.c \
        segattr.c \
        segment.c \
        transform.c \
        wsioutattr.c \
        utils.c 

all:	$(LIBDIR)/fortxgks.a

$(LIBDIR)/fortxgks.a: $(OBJS)
	ar cr $(LIBDIR)/fortxgks.a $(OBJS)
	ranlib $(LIBDIR)/fortxgks.a

clean: 
	rm -f *.o *.BAK core 

ci:		
	ci -u -tcheckin *.[ch] Makefile

depend:
	makedepend -I$(INCLUDES) *.c

install: all


# DO NOT DELETE THIS LINE -- make depend depends on it.

control.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
control.o: /andrew/Xgks/source/xgks/include/xgks.h /usr/include/sys/types.h
control.o: /usr/include/sys/stat.h
error.o: /andrew/Xgks/source/xgks/include/xgks.h /usr/include/stdio.h
error.o: fortxgks.h fortmac.h rtaixdefs.h
escapes.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
escapes.o: /andrew/Xgks/source/xgks/include/xgks.h
gerhnd.o: /andrew/Xgks/source/xgks/include/xgks.h /usr/include/stdio.h
gerhnd.o: fortxgks.h fortmac.h rtaixdefs.h
inputevent.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inputevent.o: /andrew/Xgks/source/xgks/include/xgks.h
inputinit.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inputinit.o: /andrew/Xgks/source/xgks/include/xgks.h pdrutils.h
inputmode.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inputmode.o: /andrew/Xgks/source/xgks/include/xgks.h
inputreq.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inputreq.o: /andrew/Xgks/source/xgks/include/xgks.h
inputsamp.o: /andrew/Xgks/source/xgks/include/xgks.h /usr/include/stdio.h
inputsamp.o: fortxgks.h fortmac.h rtaixdefs.h
inqerrlist.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inqerrlist.o: /andrew/Xgks/source/xgks/include/xgks.h
inqgksdesc.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inqgksdesc.o: /andrew/Xgks/source/xgks/include/xgks.h
inqgkslist.o: /andrew/Xgks/source/xgks/include/xgks.h /usr/include/stdio.h
inqgkslist.o: fortxgks.h fortmac.h rtaixdefs.h
inqpixel.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inqpixel.o: /andrew/Xgks/source/xgks/include/xgks.h
inqseglist.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inqseglist.o: /andrew/Xgks/source/xgks/include/xgks.h
inqstate.o: /andrew/Xgks/source/xgks/include/xgks.h /usr/include/stdio.h
inqstate.o: fortxgks.h fortmac.h rtaixdefs.h
inqwsdesc.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inqwsdesc.o: /andrew/Xgks/source/xgks/include/xgks.h
inqwslist.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
inqwslist.o: /andrew/Xgks/source/xgks/include/xgks.h
meta.o: /andrew/Xgks/source/xgks/include/xgks.h /usr/include/stdio.h
meta.o: fortxgks.h fortmac.h rtaixdefs.h
output.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
output.o: /andrew/Xgks/source/xgks/include/xgks.h
rep.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
rep.o: /andrew/Xgks/source/xgks/include/xgks.h
segattr.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
segattr.o: /andrew/Xgks/source/xgks/include/xgks.h
segment.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
segment.o: /andrew/Xgks/source/xgks/include/xgks.h
transform.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
transform.o: /andrew/Xgks/source/xgks/include/xgks.h
utils.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
utils.o: /andrew/Xgks/source/xgks/include/xgks.h pdrutils.h
wsioutattr.o: fortxgks.h fortmac.h rtaixdefs.h /usr/include/stdio.h
wsioutattr.o: /andrew/Xgks/source/xgks/include/xgks.h
