#! /bin/sh
# Extracts the labels for x11perf operations so it can be pasted on
# to the getbest'ed, getrate'd results of x11perf for easy comparison
# of servers
# Mark Moraes, University of Toronto
awk '$2 == "reps" || $2 == "trep" {
	print $0;
	next;
    }
' $1 |
sed 's/^.*: //' |
uniq

