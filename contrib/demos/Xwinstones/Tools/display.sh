#!/bin/sh
MACHINE=$1
MONITOR=$2
MONITORNAME=$3
echo display.sh
echo 	machine=$MACHINE monitor=$MONITOR monitorname=$MONITORNAME
display -c $MONITORNAME
