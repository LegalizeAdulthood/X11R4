#
# @(#)Makefile 1.3 89/12/06 SMI
#
WHOAMI = panels
SRCFILES=  btn_menu.c choices.c client_data.c item_move.c list_glyphs.c \
		panel_repaint.c repaint.c simple_panel.c stop_frame.c

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

