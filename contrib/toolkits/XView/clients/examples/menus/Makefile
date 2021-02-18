#
# @(#)Makefile 1.3 89/12/06 SMI
#
WHOAMI = menus
SRCFILES=  menu.c menu_dir.c menu_dir2.c pin_menu.c simple_menu.c xv_menu.c

MKFILE=    Makefile.customer
SRCSALL=   ${MKFILE} ${SRCFILES}

EXAMPLEDIR=/usr/share/src/sun/xview/examples

.KEEP_STATE:

all::

include ../../../Makefile.master

gprof:: 

lint:: 
	${MAKE} ${MFLAGS} ${COMMONFLAGS} lint

install:

install_sources: ${SRCSALL} 
	-install -d -m 755 ${DESTDIR}${EXAMPLEDIR}/${WHOAMI}
	-install -m 644 ${MKFILE} ${DESTDIR}${EXAMPLEDIR}/${WHOAMI}/Makefile
	-@for i in ${SRCFILES} ; do \
		echo "installing $$i"; \
		if [ -f ${DESTDIR}${EXAMPLEDIR}/${WHOAMI}/$$i ]; then \
		    rm -f ${DESTDIR}${EXAMPLEDIR}/${WHOAMI}/$$i; \
		fi; \
		install -m 444 $$i ${DESTDIR}${EXAMPLEDIR}/${WHOAMI}/$$i; \
	done


clean::
	rm -f *.o *.pg core ${SRCSALL}

