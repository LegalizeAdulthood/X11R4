#! /bin/sh
# Extracts the column of rates of various operations, so that they
# can be pasted together later. Run this after getbest.sh or getavg.sh
# After this, you can run paste on the results of getbest on all
# the servers you want to compare to get nice columnar results.
# Also paste the output of getlabel.sh at the end.
# Mark Moraes, University of Toronto
sed 's/( *\([0-9]*\)/(\1/' $1  |
awk '$2 == "reps" || $2 == "trep" {
	n = substr($6,2,length($6)-7);
	printf "%8s\n", n;
    }
'
