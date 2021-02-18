# Example makefile to print the documentation of GWM
# consult your local TeX/LaTeX guru if you need help
# (or just grab the postcript version: gwm.ps by ftp on expo.lcs.mit.edu)

# To adjust the placement of the text on the page, play with the
# hoffset/voffset dimensions at the top of the "gwm.tex" file 

# if the included postscript files upsets your dvi driver, uncomment the
# corresponding line in the "gwm.tex" file

# NOTE: it is NORMAL to get on first pass a lot of :
#	LaTeX Warning: Reference `xxx' on page xx undefined.

all:	gwm.ps

gwm.dvi: gwm.tex colas.sty title.tex usage.tex overview.tex wool_ref.tex \
	contents.tex quick_ref.tex index.tex standard.tex
#	rm -f *.aux	# clean dir in case of stange errors
	echo 'Rerun to get cross-references right'> gwm.log
	@echo '***** expect a LOT of LaTeX Warnings now *****'
	../gwm_util latex gwm

# These are EXAMPLES of how to print a .dvi file. Refer to your local TeX
# Wizard for more details
# you will need a dvi2ps understanding the \special{psfile=...} command

gwm.ps:	gwm.dvi idraw.pro
	../gwm_util latex gwm
	-dvi2ps -i idraw.pro gwm.dvi > gwm.ps

print: gwm.ps
	lpr gwm.ps 
	pr HISTORY KNOWN_BUGS |lpr
