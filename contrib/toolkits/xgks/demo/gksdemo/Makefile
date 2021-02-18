#
# makefile for FORTRAN GKS programs
#

FLIBDIR = /andrew/Xgks/source/xgks/lib
CLIBDIR = /andrew/Xgks/source/xgks/lib
MYLIB  = $(FLIBDIR)/fortxgks.a
GKSLIB = $(CLIBDIR)/libxgks.a
DESTDIR = .

#CFLAGS = -O
CFLAGS = -O -DDEBUG

FFLAGS =  

LFLAGS = 

LLIBS  = ${MYLIB} ${GKSLIB} -lX11 -lm
 
all:		$(DESTDIR)/gksdemo

$(DESTDIR)/gksdemo:	gksdemo.o ${MYLIB} ${GKSLIB}
		@echo "Getting libxgks from" $(CLIBDIR)
		@echo "Getting fortxgks from" $(FLIBDIR)
		@echo "Placing gksdemo in" $(DESTDIR)
		f77 ${LFLAGS} gksdemo.o ${LLIBS} -o $@

star:		star.o ${MYLIB} ${GKSLIB}
		f77 ${LFLAGS} star.o ${LLIBS} -o $@

gksdemo.o:	gksdemo.f
		f77 ${FFLAGS} -c $<

star.o:		star.f
		f77 ${FFLAGS} -c $<

clean:		
		rm *.o star gksdemo
