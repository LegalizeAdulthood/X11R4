#!/bin/sh 
#Calls other run shells.
/bin/rm -f .out
./runarcs.sh $@ 
sync; sync; sync;
./runsfarcs.sh $@
sync; sync; sync;
./runpfarcs.sh $@
sync; sync; sync;
./runpixblits.sh $@
sync; sync; sync;
./runscrblits.sh $@
sync; sync; sync;
./runcolors.sh $@
sync; sync; sync;
./runximages.sh $@
sync; sync; sync;
./runzimages.sh $@
sync; sync; sync;
./runlines0.sh  $@
sync; sync; sync;
./runlines3.sh $@
sync; sync; sync;
./runpoints.sh $@
sync; sync; sync;
./runpolys.sh $@
sync; sync; sync;
./runsquads.sh $@
sync; sync; sync;
./runpquads.sh $@
sync; sync; sync;
./runscaves.sh $@
sync; sync; sync;
./runpcaves.sh $@
sync; sync; sync;
./runrects.sh $@
sync; sync; sync;
./runsfrects.sh $@
sync; sync; sync;
./runpfrects.sh $@
sync; sync; sync;
./runsegs.sh $@
sync; sync; sync;
./runstrings.sh $@
sync; sync; sync;
./runistrings.sh $@
sync; sync; sync;
./runvect.sh $@
sync; sync; sync;
