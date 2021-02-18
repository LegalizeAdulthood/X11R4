#!/bin/sh
#XDrawArcs
./arcstones $@ -casename GemArcsN1.D360.R190 -narcs 1
./arcstones $@ -casename GemArcsN100.D360.R190  -narcs 100
./arcstones $@ -casename GemArcsN190.D360.Rvar -rad1 1 -rad2 1 -drad1 2 -drad2 2 -x 200 -y 200 -dx -1 -dy -1  -narcs 190
./arcstones $@ -casename GemArcsN1.D001.R190 -ang1 0 -ang2 1 -narcs 1
./arcstones $@ -casename GemArcsN100.D001.R190 -ang1 0 -ang2 1 -narcs 100
./arcstones $@ -casename GemArcsN1.D002.R190 -ang1 0 -ang2 2 -narcs 1
./arcstones $@ -casename GemArcsN100.D002.R190 -ang1 0 -ang2 2 -narcs 100
./arcstones $@ -casename GemArcsN1.D034.R2000 -rad1 2000 -rad2 2000 -ang1 73 -ang2 34 -x -700 -y 5 -narcs 1
./arcstones $@ -casename GemArcsN100.D034.R2000 -rad1 2000 -rad2 2000 -ang1 73 -ang2 34 -x -700 -y 5 -narcs 100
