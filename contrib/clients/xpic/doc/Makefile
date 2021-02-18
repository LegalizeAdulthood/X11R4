# Must be invoked with DOCDIR, DOC, LIBDIR, MANEXT, MANDIR defined
# on the Make command line.
.SUFFIXES: .man .1

.man.1:
	sed -e 's?DOCDIR?$(DOCDIR)?g'\
	    -e 's?LIBDIR?$(LIBDIR)?g' $*.man > $*.1

TARGETS = x2ps.1 x2pic.1 xpic.1

# protect the sucker who invokes it by typing make in this directory.
first:

manpages: $(TARGETS)
	for i in $(TARGETS) ;\
	do \
		install -c -m 444 $$i $(MANDIR)/`basename $$i .1`.$(MANEXT); \
	done
