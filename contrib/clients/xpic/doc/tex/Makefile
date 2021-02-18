#DVI2PS = /local/src/tex/drivers/dvi2ps.3.13/driver/src/dvi2ps
DVI2PS=dvi2ps
X2PS=x2ps
X2TPIC=x2tpic

first: xpic.ps

clean:
	rm -f *.dvi *.log *.o x2tpic *.ps *.pic node.tex shrunknode.tex *~. \#*

xpic.ps: xpic.dvi 
	rm -f xpic.ps
	dvi2ps xpic > xpic.ps

texwarning.ps: texwarning.x
	rm -f texwarning.ps
	x2ps texwarning.x > texwarning.ps

funstuff.ps: funstuff.x
	rm -f funstuff.ps
	x2ps funstuff.x > funstuff.ps

node.tex: node.pic
	tpic node.pic

node.pic: node.x
	rm -f node.pic
	x2tpic node.x > node.pic

shrunknode.tex: shrunknode.pic
	tpic shrunknode.pic

shrunknode.pic: node.x
	rm -f shrunknode.pic
	x2tpic -s 0.6 node.x > shrunknode.pic

xpic.dvi: xpic.tex node.tex shrunknode.tex texwarning.ps funstuff.ps 
	rm -f xpic.dvi
	lt xpic
