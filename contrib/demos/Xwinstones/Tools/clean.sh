#!/bin/sh
RMFLAGS=$1
MKFLAGS=$2
echo clean.sh
echo 	rmflags=$RMFLAGS mkflags=$MKFLAGS
rm $RMFLAGS ./.out
cd ./bin
make $MKFLAGS clean
cd ..
