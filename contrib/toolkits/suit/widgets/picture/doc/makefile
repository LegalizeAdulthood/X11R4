TROFF = ditroff

print: picture.n
	pic < $? | tbl | $(TROFF) -man | lpr -n
	echo -n '' > print
