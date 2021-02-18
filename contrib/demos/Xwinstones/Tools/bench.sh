#!/bin/sh
MACHINE=$1
shift
SERVER=$1
shift
MONITOR=$1
shift
BUILD=$1
shift
ARGS=$@
echo bench.sh
echo 	machine=$MACHINE server=$SERVER monitor=$MONITOR args=$ARGS
cd ./bin
./Xruntest.sh -machine $MACHINE $SERVER -monitor $MONITOR -build $BUILD $ARGS
mv ./.out ../Results/$MACHINE/$BUILD/data
cd ..
