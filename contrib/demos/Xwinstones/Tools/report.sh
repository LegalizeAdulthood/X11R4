#!/bin/sh
HEADER0=$1
HEADER1=$2
HEADER2=$3
MACHINE=$4
MONITOR=$5
BUILD=$6
echo report.sh
echo 	head0=$HEADER0 head1=$HEADER1 head2=$HEADER2 
echo 	machine=$MACHINE monitor=$MONITOR
awk -f ./Tools/report.awk head0=$HEADER0 head1=$HEADER1 head2=$HEADER2 \
./Results/$MACHINE/$BUILD/data > ./Results/$MACHINE/$BUILD/db.report
