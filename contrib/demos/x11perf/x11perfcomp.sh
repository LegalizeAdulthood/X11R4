#! /bin/sh
# Collects multiple outputs of x11perf.  Runs getavg, fillblanks, getrate, on
# each file, and collects the output neatly.
#
# Normally uses the results from $1 to extract the test labels, which allows
# comparisons of a subset of x11perf tests.  Using -l <filename> as $1 and $2
# forces x11perfcomp to use the labels stored in $2.
#
# Mark Moraes, University of Toronto
# Joel McCormack, DEC Western Research Lab

set -e
tmp=/tmp/rates.$$
mkdir $tmp $tmp/rates
case $1 in
-l)	cp $2 $tmp/labels
	shift; shift
	;;
*)	getlabel.sh $1 > $tmp/labels
	;;
esac
n=1
for i
do
	(echo "     $n  "; \
	 echo '--------'; \
	 getavg.sh $i > $tmp/$i.avg; \
	 fillblanks.sh $tmp/$i.avg $tmp/labels > $tmp/$i.favg; \
	 getrate.sh $tmp/$i.favg) \
	    > $tmp/rates/$i
	echo "$n: $i"
	n=`expr $n + 1`
done
echo ''
(echo Operation; echo '---------'; getlabel.sh $1) | \
(cd $tmp/rates; paste $* -) | \
sed 's/	/   /g'
rm -rf $tmp
