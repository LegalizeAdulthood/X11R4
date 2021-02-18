# 
# Makefile for conversion guide
#

SRCS=	convert_index.doc.ps \
	appendixd.doc.ps \
	GenHTD.doc.ps \
	appendixc.doc.ps \
	GenHTC.doc.ps \
	appendixb.doc.ps \
	GenHTB.doc.ps \
	appendixa.doc.ps \
	GenHTA.doc.ps \
	chap5.doc.ps \
	GenHT5.doc.ps \
	chap4.doc.ps \
	GenHT4.doc.ps \
	chap3.doc.ps \
	GenHT3.doc.ps \
	chap2.doc.ps \
	GenHT2.doc.ps \
	chap1.doc.ps \
	GenHT1.doc.ps \
	convert_pref.doc.ps \
	convert_figures.doc.ps \
	convert_tables.doc.ps \
	convertTOC.doc.ps \
	convert_cover.doc.ps
#
# Change this command for your local system
#
PSPRINT= /usr/doctools/bin/psprint `/bin/hostname`

all:: $(SRCS)
	for i in $(SRCS); do \
		($(PSPRINT) $$i); \
	done
	lpr corrections
