#! /bin/sh
# Collects multiple outputs of getbest, runs getrate, getlabel, and
# collects the output neatly.
# Mark Moraes, University of Toronto
set -e
tmp=/tmp/rates.$$
mkdir $tmp $tmp/rates
n=1
for i
do
	(echo "     $n  "; echo '--------'; getrate.sh $i) \
	    > $tmp/rates/$i
	echo "$n: $i"
	n=`expr $n + 1`
done
echo ''
(echo Operation; echo '---------'; getlabel.sh $1) | \
(cd $tmp/rates; paste $* -) | \
sed 's/	/   /g'
rm -rf $tmp
