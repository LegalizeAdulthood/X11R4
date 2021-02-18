# fonts/news/Makefile
#
# Makefile for NeWS fonts.
# D. A. Cahlander 88/12/31.
# PostScript is a trademark of Adobe Systems, Inc.

# see README for more information
# makedev is the program that builds device descriptions

NEWSDIR = /usr/NeWS/fonts
FONTDIR = /usr/lib/X11/fonts/misc
FONTS =	${TIMES} ${COURIER} ${HELVETICA} ${SYMBOL}
OTHERFONTS = ${BOSTON} ${CYRILLIC} ${CURSOR} ${GFONT} \
	${HEVETICA_MEDIUM} ${ICON} ${KANJI} ${SCREEN}
SHELL = /bin/sh

FBFILES = \
 	Btn10.fb Btn12.fb Btn18.fb Btn30.fb Btn35.fb Btn9.fb \
	Crl11.fb \
	Crr1.fb Crr10.fb Crr12.fb Crr14.fb Crr16.fb \
	Crr18.fb Crr24.fb Crr32.fb Crr6.fb Crr7.fb Crr8.fb Crr9.fb \
	CrrB10.fb CrrB12.fb CrrB14.fb CrrB16.fb CrrB18.fb CrrB24.fb \
	CrrB32.fb \
	CrrBO10.fb CrrBO12.fb CrrBO14.fb CrrBO16.fb CrrBO18.fb CrrBO24.fb \
	CrrBO32.fb \
	CrrO10.fb CrrO12.fb CrrO14.fb CrrO16.fb CrrO18.fb CrrO24.fb \
	CrrO32.fb \
	Cursor12.fb \
	GFont12.fb \
	Hlv10.fb Hlv12.fb Hlv14.fb Hlv18.fb Hlv24.fb Hlv32.fb \
	Hlv7.fb Hlv9.fb \
	HlvB10.fb HlvB12.fb HlvB14.fb HlvB18.fb HlvB24.fb HlvB32.fb \
	HlvBO10.fb HlvBO12.fb HlvBO14.fb HlvBO18.fb HlvBO24.fb HlvBO32.fb \
	HlvO10.fb HlvO12.fb HlvO14.fb HlvO18.fb HlvO24.fb HlvO32.fb \
	HlvM32.fb \
	Icon12.fb \
	Kanji16.fb Kanji24.fb \
	Scn11.fb Scn13.fb Scn15.fb Scn7.fb \
	ScnB13.fb ScnB15.fb \
	Smb10.fb Smb12.fb Smb14.fb Smb18.fb Smb24.fb Smb32.fb \
	SmbM32.fb \
	TmsB10.fb TmsB12.fb TmsB14.fb TmsB18.fb TmsB24.fb TmsB25.fb \
	TmsB27.fb TmsB30.fb TmsB32.fb TmsB34.fb TmsB39.fb TmsB44.fb \
	TmsB49.fb TmsB54.fb TmsB59.fb TmsB69.fb TmsB89.fb \
	TmsBI10.fb TmsBI12.fb TmsBI14.fb TmsBI18.fb TmsBI24.fb TmsBI32.fb \
	TmsI10.fb TmsI12.fb TmsI14.fb TmsI18.fb TmsI24.fb TmsI26.fb \
	TmsI29.fb TmsI32.fb TmsI33.fb TmsI38.fb TmsI43.fb TmsI47.fb \
	TmsI53.fb TmsI57.fb TmsI67.fb TmsI86.fb \
	TmsR10.fb TmsR12.fb TmsR14.fb TmsR16.fb TmsR18.fb TmsR19.fb \
	TmsR20.fb TmsR23.fb TmsR24.fb TmsR26.fb TmsR28.fb TmsR31.fb \
	TmsR32.fb TmsR36.fb TmsR41.fb TmsR46.fb TmsR51.fb TmsR56.fb \
	TmsR62.fb TmsR72.fb TmsR9.fb TmsR92.fb

BOSTON = \
 	Btn10.snf Btn12.snf Btn18.snf Btn30.snf Btn35.snf Btn9.snf

CYRILLIC = Crl11.snf

COURIER = \
	Crr1.snf Crr10.snf Crr12.snf Crr14.snf Crr16.snf \
	Crr18.snf Crr24.snf Crr32.snf Crr6.snf Crr7.snf Crr8.snf Crr9.snf \
	CrrB10.snf CrrB12.snf CrrB14.snf CrrB16.snf CrrB18.snf CrrB24.snf \
	CrrB32.snf \
	CrrBO10.snf CrrBO12.snf CrrBO14.snf CrrBO16.snf CrrBO18.snf CrrBO24.snf \
	CrrBO32.snf \
	CrrO10.snf CrrO12.snf CrrO14.snf CrrO16.snf CrrO18.snf CrrO24.snf \
	CrrO32.snf

CURSOR = Cursor12.snf

GFONT =	GFont12.snf

HELVETICAL = \
	Hlv10.snf Hlv12.snf Hlv14.snf Hlv18.snf Hlv24.snf Hlv32.snf \
	Hlv7.snf Hlv9.snf \
	HlvB10.snf HlvB12.snf HlvB14.snf HlvB18.snf HlvB24.snf HlvB32.snf \
	HlvBO10.snf HlvBO12.snf HlvBO14.snf HlvBO18.snf HlvBO24.snf HlvBO32.snf \
	HlvO10.snf HlvO12.snf HlvO14.snf HlvO18.snf HlvO24.snf HlvO32.snf

HEVETICA_MEDIUM = HlvM32.snf

ICON =	Icon12.snf

KANJI = \
	Kanji16.snf Kanji24.snf

SCREEN = \
	Scn11.snf Scn13.snf Scn15.snf Scn7.snf \
	ScnB13.snf ScnB15.snf

SYMBOL = \
	Smb10.snf Smb12.snf Smb14.snf Smb18.snf Smb24.snf Smb32.snf \
	SmbM32.snf

TIMES =	 \
	TmsB10.snf TmsB12.snf TmsB14.snf TmsB18.snf TmsB24.snf TmsB25.snf \
	TmsB27.snf TmsB30.snf TmsB32.snf TmsB34.snf TmsB39.snf TmsB44.snf \
	TmsB49.snf TmsB54.snf TmsB59.snf TmsB69.snf TmsB89.snf \
	TmsBI10.snf TmsBI12.snf TmsBI14.snf TmsBI18.snf TmsBI24.snf TmsBI32.snf \
	TmsI10.snf TmsI12.snf TmsI14.snf TmsI18.snf TmsI24.snf TmsI26.snf \
	TmsI29.snf TmsI32.snf TmsI33.snf TmsI38.snf TmsI43.snf TmsI47.snf \
	TmsI53.snf TmsI57.snf TmsI67.snf TmsI86.snf \
	TmsR10.snf TmsR12.snf TmsR14.snf TmsR16.snf TmsR18.snf TmsR19.snf \
	TmsR20.snf TmsR23.snf TmsR24.snf TmsR26.snf TmsR28.snf TmsR31.snf \
	TmsR32.snf TmsR36.snf TmsR41.snf TmsR46.snf TmsR51.snf TmsR56.snf \
	TmsR62.snf TmsR72.snf TmsR9.snf TmsR92.snf

all:	${FONTS}

more:	${OTHERFONTS}

kanji:	${KANJI}

symbol:	${SYMBOL}

times:	${TIMES}

sample:	\
	Btn10.snf  \
	Crl11.snf \
	Crr10.snf  \
	CrrB10.snf  \
	CrrBO10.snf  \
	CrrO10.snf  \
	Hlv10.snf  \
	HlvB10.snf  \
	HlvBO10.snf  \
	HlvO10.snf  \
	Smb10.snf  \
	TmsB10.snf  \
	TmsBI10.snf  \
	TmsI10.snf  \
	TmsR10.snf  \
	Cursor12.snf \
	GFont12.snf \
	HlvM32.snf  \
	Icon12.snf \
	Kanji16.snf \
	Scn11.snf 

shar:	Makefile README news.awk
	echo "This is a shar archive.  Cut off the header and" > shar.ar
	echo "execute:" >> shar.ar
	echo "	sh < file" >> shar.ar
	echo Files: $? >> shar.ar
	echo "=================== cut here ===================" >> shar.ar
	shar $? >> shar.ar

${FBFILES}:
	ln -s ${NEWSDIR}/$@ .

install:: all
	cp ${FONTS} ${FONTDIR}
	cd ${FONTDIR}; mkfontdir

installmore: more
	cp ${OTHERFONTS} ${FONTDIR}
	cd ${FONTDIR}; mkfontdir

clean::
	rm -f ${SNFFILES} ${FBFILES} core *.out

clobber: clean

.SUFFIXES: .fb .afb .bdf .snf

.fb.afb:
	dumpfont -a -d . ${NEWSDIR}/$<

.afb.snf:
	awk -f news.awk $< | tee $*.bdf | bdftosnf > $*.snf
	rm $<

.fb.snf:
	dumpfont -a -d . ${NEWSDIR}/$<
	awk -f news.awk $*.afb | tee $*.bdf | bdftosnf > $*.snf
	rm $*.afb
