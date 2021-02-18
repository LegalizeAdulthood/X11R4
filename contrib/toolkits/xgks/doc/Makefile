#
# XXXX-XXX (C) COPYRIGHT IBM CORPORATION 1988
# LICENSED MATERIALS - PROPERTY OF IBM
# REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
#
#

TROFF=ptroff
INSTALLHOME=
MANSUFFIX=n

SUBDIRS = binding

all:
	for i in $(SUBDIRS); do \
		(cd $$i; make $(MFLAGS) INSTALLHOME=$(INSTALLHOME) all ) done

depend:
	for i in $(SUBDIRS); do \
		(cd $$i; make $(MFLAGS) INSTALLHOME=$(INSTALLHOME) depend ) done

install:
	install -c -m 664 xgks.man $(INSTALLHOME)/man/man$(MANSUFFIX)/xgks.$(MANSUFFIX)
	for i in $(SUBDIRS); do \
		(cd $$i; make $(MFLAGS) INSTALLHOME=$(INSTALLHOME) install ) done

clean:
	-rm -f f function_list.tr core *.out
	for i in $(SUBDIRS); do \
		(cd $$i; make $(MFLAGS) INSTALLHOME=$(INSTALLHOME) clean ) done

print: flist design1 supported notsupported gks

flist:
	prfunc

design1:
	$(TROFF) -me xgks.des.tr

supported:
	supported

notsupported:
	notsupported

gks: gks.trf
	$(TROFF) gks.trf
	
gks.trf : gks.vdoc
	-rm -f gks.trf
	ezprint -t gks.vdoc > gks.trf
