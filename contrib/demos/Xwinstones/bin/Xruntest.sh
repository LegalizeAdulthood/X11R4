#!/bin/sh
X="/usr/lpp/X11/bin/X -p 0 -s 0"
cmdname=$0
TEMP=`$X &`
num=`expr "$TEMP" : '.*:\([0-9]*\)'`
dis=`expr "$TEMP" : '\(....\)'`
DISPLAY="unix:$num"
# DISPLAY="$dis:$num"
export DISPLAY
./runtest.sh $@
sync; sync; sync;
sleep 4
/bin/kill `ps -e | grep X | awk '{print $1 }'`
