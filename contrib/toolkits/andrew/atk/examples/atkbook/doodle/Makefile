ANDREWDIR = /usr/andrew

INCLUDES = -I. -I${ANDREWDIR}/include -I${ANDREWDIR}/include/atk

.SUFFIXES: .ih .eh .ch .do

.ch.ih:
	${ANDREWDIR}/bin/class ${INCLUDES} $*.ch
.ch.eh:
	${ANDREWDIR}/bin/class ${INCLUDES} $*.ch
.c.o:
	cc -c ${INCLUDES} $*.c
.o.do:
	${ANDREWDIR}/bin/makedo $<

all:  dood.do doodview.do 

dood.o: dood.eh

doodview.o: doodview.eh dood.ih 

dood.do: dood.o

doodview.do: doodview.o

clean:
	rm -f *.o *.do *.eh *.ih *.BAK *.CKP core
