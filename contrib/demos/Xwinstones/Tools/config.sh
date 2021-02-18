#!/bin/sh
MACHINE=$1
BUILD=$2
echo config.sh
echo	 machine=$MACHINE
mkdir ./Results/$MACHINE
mkdir ./Results/$MACHINE/$BUILD
date					> ./Results/$MACHINE/$BUILD/date
cat /etc/motd				> ./Results/$MACHINE/$BUILD/motd
ps -el					> ./Results/$MACHINE/$BUILD/ps
cat ./Tools/config.txt			> ./Results/$MACHINE/$BUILD/config
