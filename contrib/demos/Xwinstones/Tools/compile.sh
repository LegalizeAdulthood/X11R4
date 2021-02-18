#!/bin/sh
MKFLAGS=$1
echo compile.sh
echo 	mkflags=$MKFLAGS
cd ./bin
make $MKFLAGS all
cd ..
