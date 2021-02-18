#!/bin/sh
X="/usr/lpp/X11/bin/X -p 0 -s 0"
TEMP=`$X &`
DISPLAY="unix:0"
export DISPLAY
$@
sync; sync; sync;
sleep 3
/bin/kill `ps -e | grep X | awk '{print $1 }'`
