#! /bin/sh
# Takes an output from x11perf and extracts the best figures for each
# operation. To compare this with other servers, you may want to
# run getrate.sh and then paste the results of that with getlabel.sh
# Mark Moraes, University of Toronto
sed 's/( *\([0-9]*\)/(\1/' $1 |
    awk '$2 == "reps" {
	n = substr($6,2,length($6)-7);
	if (n > best) {
		best = n;
		bestline = $0
	}
	next;
    }
    NF == 0 && bestline != "" {
	print bestline;
	bestline="";
	best = 0;
	next;
    }
'
