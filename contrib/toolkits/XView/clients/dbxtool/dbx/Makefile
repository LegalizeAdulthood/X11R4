#
# @(#)Makefile 1.5 89/10/31 SMI
#

ASFLAGS = -P

MAKEDEFSC = library.c makedefs.c pipeout.c
MAKEDEFSH = defs.h library.h
MAKEDEFSOBJ = library.o $(ARCH)_cerror.o
MAKEDEFSS = sun3_cerror.s sun4_cerror.s sun386_cerror.s

all:  makedefs

include ../../../Makefile.master

makedefs: makedefs.c $(MAKEDEFSH) $(MAKEDEFSOBJ)
	$(CC) -o makedefs makedefs.c $(MAKEDEFSOBJ)


install_sources: Makefile.customer makedefs.c ${MAKEDEFSH}
	-install -d -m 755 ${DESTDIR}${SRCDIR}/dbxtool/dbx
	-install -m 644 Makefile.customer ${DESTDIR}${SRCDIR}/dbxtool/dbx/Makefile
	-for i in ${MAKEDEFSC} ${MAKEDEFSH} ${MAKEDEFSS}; do \
		install -m 644 $$i ${DESTDIR}${SRCDIR}/dbxtool/dbx; \
	done
