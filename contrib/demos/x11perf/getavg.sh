#! /bin/sh
# Takes an output from x11perf and gets the averaged numbers for each
# operation.  To compare this with other servers, you may want to
# run getrate.sh and then paste the results of that with getlabel.sh
# Mark Moraes, University of Toronto
awk '$2 == "reps" || $2 == "trep" {
	line = $0;
	next;
    }
    NF == 0 && line != "" {
	print line;
	line="";
	next;
    }
' $1
