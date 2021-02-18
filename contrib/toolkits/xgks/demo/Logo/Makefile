#
# XXXX-XXX (C) COPYRIGHT IBM CORPORATION 1988
# LICENSED MATERIALS - PROPERTY OF IBM
# REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
#
#	XGKS demos make script
#
# $Header: Makefile,v 3.12 89/03/10 10:37:10 amy Exp $

H = /usr/include 
LIBS = ../lib/libxgks.a -lX11 -lm

CFLAGS = -I$(H) -I/andrew/Xgks/source/xgks/include 

SRCS = bsort.c clock.c defcolors.c font.c hanoi.c mi.c pline.c pmark.c usa.c wong.c
BINS = bsort clock defcolors font hanoi mi pline pmark usa wong

SUBDIRS = Logo example sled stratego Music Backgammon

all: $(BINS)
	@for i in $(SUBDIRS); do \
		(echo '*** making: '`pwd`'/'$$i; \
		cd $$i; make $(MFLAGS) all; \
		echo '*** done making: '`pwd`'/'$$i ) done

install:

cleanbins:	clean
	-rm -f $(BINS)

clean:
	-rm *.o core 
	@for i in $(SUBDIRS); do \
		(echo '*** making: '`pwd`'/'$$i; \
		cd $$i; make $(MFLAGS) clean; \
		echo '*** done making: '`pwd`'/'$$i ) done

#
depend:
	co -l Makefile
	@for i in $(BINS); do \
	    $(CC) -M $(CFLAGS) $$i.c | sort | uniq | \
	    awk ' { if ($$1 != prev) \
		{ if (rec != "") print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; }\
		else rec = rec " " $$2 } } \
		END { print rec } ' >> makedep; \
	    echo "$$i : $$i.o" >> makedep; \
	    echo "	$$(CC) $$(CFLAGS) -o \$$@ \$$@.o $$(LIBS)" >> makedep; \
	    echo "" >> makedep; done	
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep 
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	echo '# see make depend above' >> Makefile
	ci -f -q -m"make depend" -u Makefile

# DO NOT DELETE THIS LINE -- make depend uses it
# DEPENDENCIES MUST END AT END OF FILE
bsort.o: ./demo.h /usr/include/stdio.h /usr/include/string.h
bsort.o: /usr/include/strings.h /usr/include/sys/time.h /usr/include/time.h
bsort.o: /usr/include/xgks.h bsort.c
bsort : bsort.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

clock.o: /usr/include/setjmp.h /usr/include/signal.h /usr/include/stdio.h
clock.o: /usr/include/sys/nfs_defines.h /usr/include/xgks.h clock.c
clock : clock.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

defcolors.o: ./demo.h /usr/include/stdio.h /usr/include/xgks.h defcolors.c
defcolors : defcolors.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

font.o: ./demo.h /usr/include/stdio.h /usr/include/xgks.h font.c
font : font.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

hanoi.o: ./demo.h /usr/include/math.h /usr/include/stdio.h /usr/include/xgks.h
hanoi.o: hanoi.c
hanoi : hanoi.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

mi.o: ./demo.h /usr/include/stdio.h /usr/include/xgks.h mi.c
mi : mi.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

pline.o: /usr/include/stdio.h /usr/include/xgks.h pline.c
pline : pline.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

pmark.o: /usr/include/stdio.h /usr/include/xgks.h pmark.c
pmark : pmark.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

usa.o: ./demo.h /usr/include/stdio.h /usr/include/strings.h /usr/include/xgks.h
usa.o: usa.c
usa : usa.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

wong.o: ./demo.h /usr/include/stdio.h /usr/include/xgks.h wong.c
wong : wong.o
	$(CC) $(CFLAGS) -o $@ $@.o $(LIBS)

# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
