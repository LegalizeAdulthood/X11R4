#! /bin/sh
# Some machines weren't able to complete the x11perf tests, or missed
# some tests
# Mark Moraes, University of Toronto
case $# in
2)	;;
*)	echo "Usage: $0 getbest_file label_file" >&2
	echo "  will make sure that getbest_file (assumed to be results" >&2
	echo "  of getbest on x11perf raw results) contains one line for" >&2
	echo "  each of the tests in the label file, filling in zeros for" >&2
	echo "  missing lines, so mungeall may be used for comparison" >&2
	echo "  of even incomplete runs of x11perf. label_file may be" >&2
	echo "  obtained by running getlabel on any complete x11perf" >&2
	echo "  getbest results." >&2
	exit 1
	;;
esac
f=$1
label=$2
getlabel.sh $f > /tmp/f1.$$
diff -e /tmp/f1.$$ $label > /tmp/f2.$$
others=`sed -n -e ':start' \
	-e '/^[0-9]*,[0-9]*d$/b pnext' \
	-e '/^[0-9]*[ac]$/b skpdot' \
	-e '/^[0-9]*,[0-9]*c$/b skpdot' \
	-e x -e 's/.*/bad pattern in diff output/' -e p -e n -e 'b start' \
	-e :skpdot -e p -e :skip -e n -e '/^\.$/b next' -e 'b skip' \
	-e :pnext -e p -e :next -e n -e 'b start' \
/tmp/f2.$$ | grep -v '^[0-9]*a'`
case "$others" in
'')	;;
*)	echo "$0: label file $label is incomplete or out of date," >& 2
	echo "   or you are trying to use old (pre-1.1) x11perf results." >& 2
	exit 1
	;;
esac
(sed 	-e :start -e '/^[0-9]*[ac]$/b skpdot' \
	-e :skpdot -e :skip -e n -e '/^\.$/b next' \
		-e 's,.*,     0 reps @  0.0    msec (0.0/sec): &,' \
		-e 'b skip' \
	-e :next -e n -e 'b start' \
	/tmp/f2.$$; echo '1,$p') | ed - $f > /tmp/f1.$$
cat /tmp/f1.$$
#mv /tmp/f1.$$ $f
rm -f /tmp/f1.$$ /tmp/f2.$$
