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
 
all:		$(DESTDIR)/xhib

$(DESTDIR)/xhib:	xhib.o ${MYLIB} ${GKSLIB}
		@echo "Getting libxgks from" $(CLIBDIR)
		@echo "Getting fortxgks from" $(FLIBDIR)
		@echo "Placing xhib in" $(DESTDIR)
		f77 ${LFLAGS} xhib.o ${LLIBS} -o $@

star:		star.o ${MYLIB} ${GKSLIB}
		f77 ${LFLAGS} star.o ${LLIBS} -o $@

xhib.o:	xhib.f
		f77 ${FFLAGS} -c $<

star.o:		star.f
		f77 ${FFLAGS} -c $<

clean:		
		rm *.o star xhib
