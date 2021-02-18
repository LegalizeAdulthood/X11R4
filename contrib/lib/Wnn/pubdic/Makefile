#
# Makefile for pubdic dictionaries
#
#   Do simply 'make' for normal cases.
#   Do 'make project', if your site participates in the PUBDIC project.
#
SHELL= /bin/sh

ATOD = ../jutil/atod
ATOF = ../fuzokugo/atof
WNNTOUCH = ../jutil/wnntouch
HINSI = ../jd/hinsi.data

RM = rm -f
MV = mv

TOP = ../../../..
UTILSRC = $(TOP)/util
IMAKESRC = $(UTILSRC)/imake
IRULESRC = $(UTILSRC)/imake.includes
IMAKE = $(IMAKESRC)/imake
IMAKE_DEFINES =
IMAKE_CMD = $(NEWTOP)$(IMAKE) -TImake.tmpl -I$(NEWTOP)$(IRULESRC) \
		-s Makefile $(IMAKE_DEFINES)


FZK_SRC=fzk.master fzk.attr fzk.jirattr fzk.fzkattr fzk.jircon fzk.con fzk.shuutan
FZK_FLAG=

dic=\
	kihon.dic \
	setsuji.dic \
	jinmei.dic \
	chimei.dic \
	koyuu.dic \
	special.dic \
	computer.dic \
	bio.dic \
	symbol.dic \
	tankan.dic

more=\
	kihon.more \
	setsuji.more \
	jinmei.more \
	chimei.more \
	koyuu.more \
	special.more \
	computer.more \
	bio.more \
	symbol.more \
	tankan.more

VERSION= Version

all: full.fzk kougo.fzk std.fzk $(dic)

project: all $(more)

clean:
	rm -f *.fzk *.dic *.more

std.fzk: $(FZK_SRC)
	/lib/cpp $(FZK_FLAG) fzk.master | egrep -v '^(# |$$)' | $(ATOF) -h $(HINSI) $@

full.fzk: $(FZK_SRC)
	make FZK_FLAG=-DKANSAI std.fzk
	mv std.fzk $@

kougo.fzk: $(FZK_SRC)
	make FZK_FLAG=-DKOUGO std.fzk
	mv std.fzk $@

.SUFFIXES: .hd .u .dic .more

.u.dic:
	cat $*.hd $< | $(ATOD) -p $(VERSION) -P $(VERSION) -h $(HINSI) $@

.hd.more:
	$(ATOD) -h $(HINSI) $@ < $<

$(dic): $(HINSI)

Makefile:: Imakefile \
	$(IRULESRC)/Imake.tmpl \
	$(IRULESRC)/Imake.rules \
	$(IRULESRC)/site.def
	-@if [ -f Makefile ]; then \
	echo "$(RM) Makefile.bak; $(MV) Makefile Makefile.bak"; \
	$(RM) Makefile.bak; $(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP)
