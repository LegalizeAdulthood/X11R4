#!/bin/sh
#XFillArcs (Solid Fill)
./arcstones $@ -casename GemSFArcsN1.D010.R190.p -ang1 0 -ang2 10 -narcs 1 -pieslice
./arcstones $@ -casename GemSFArcsN1.D020.R190.p -ang1 0 -ang2 20 -narcs 1 -pieslice
./arcstones $@ -casename GemSFArcsN1.D090.R190.p -ang1 0 -ang2 90 -narcs 1 -pieslice
./arcstones $@ -casename GemSFArcsN1.D270.R190.p -ang1 0 -ang2 270 -narcs 1 -pieslice
./arcstones $@ -casename GemSFArcsN1.D180.R190.c -ang1 0 -ang2 180 -narcs 1 -chord
./arcstones $@ -casename GemSFArcsN1.D180.R190.p -ang1 0 -ang2 180 -narcs 1 -pieslice
./arcstones $@ -casename GemSFArcsN1.D360.R190.p -narcs 1 -pieslice
./arcstones $@ -casename GemSFArcsN1.D360.R190.c -narcs 1 -chord
