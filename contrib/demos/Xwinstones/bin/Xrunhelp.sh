#!/bin/sh
X="/usr/lpp/X11/bin/X -p 0 -s 0"
TEMP=`$X &`
DISPLAY="unix:0"
export DISPLAY
arcstones -help >> helpstones
arcstones -h >> helpstones
blitstones -h >> helpstones
charstones -h >> helpstones
colorstones -h >> helpstones
imagestones -h >> helpstones
linestones -h >> helpstones
pointstones -h >> helpstones
polystones -h >> helpstones
rectstones -h >> helpstones
stringstones -h >> helpstones
sweepstones -h >> helpstones
vectstones -h >> helpstones
sync; sync; sync;
/bin/kill `ps -e | grep X | awk '{print $1 }'`
