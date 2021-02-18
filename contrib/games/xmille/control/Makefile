.PRECIOUS: control.a
HDRS =	control.h co_class.h
CFLAGS = -g -I/x11

OFILES =	button.o scrollbar.o init.o affirm.o dispatch.o prompted.o

SRCCOMMON =	button.c scrollbar.c init.c affirm.c dispatch.c prompted.c

control.a : $(OFILES)
	ar cr $@ $?
	ranlib $@

control.o : $(OFILES)
	$(LD) -r -o $@ $(OFILES)

clean:
	rm -f $(OFILES) control.a
