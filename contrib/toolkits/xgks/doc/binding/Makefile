# C language Binding for Xgks  - troff with me macros

TROFF=ptroff
INSTALLHOME=
MANSUFFIX=n

all: cbinding.trf

depend:

install: cbinding.trf
	install -c -m 664 cbinding.trf $(INSTALLHOME)/lib/xgks

clean:
	-rm -f f cbinding.trf AP core *.out

print: cbinding.trf
	$(TROFF) -me cbinding.trf

cbinding.trf :
	mkcbinding
