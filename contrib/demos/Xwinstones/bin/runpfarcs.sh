#!/bin/sh
#XFillArcs (Pattern Fill)
./arcstones $@ -casename GemPFArcsN1.D180.R190.p.t -ang1 0 -ang2 180 -narcs 1 -pieslice -tile
./arcstones $@ -casename GemPFArcsN1.D180.R190.p.s -ang1 0 -ang2 180 -narcs 1 -pieslice -stip
./arcstones $@ -casename GemPFArcsN1.D180.R190.p.o -ang1 0 -ang2 180 -narcs 1 -pieslice -opaqstip
./arcstones $@ -casename GemPFArcsN1.D180.R190.c.t -ang1 0 -ang2 180 -narcs 1 -chord -tile
./arcstones $@ -casename GemPFArcsN1.D180.R190.c.s -ang1 0 -ang2 180 -narcs 1 -chord -stip
./arcstones $@ -casename GemPFArcsN1.D180.R190.c.o -ang1 0 -ang2 180 -narcs 1 -chord -opaqstip
./arcstones $@ -casename GemPFArcsN1.D360.R190.p.t -narcs 1 -pieslice -tile
./arcstones $@ -casename GemPFArcsN1.D360.R190.p.s -narcs 1 -pieslice -stip
./arcstones $@ -casename GemPFArcsN1.D360.R190.p.o -narcs 1 -pieslice -opaqstip
./arcstones $@ -casename GemPFArcsN1.D360.R190.c.t -narcs 1 -chord -tile
./arcstones $@ -casename GemPFArcsN1.D360.R190.c.s -narcs 1 -chord -stip
./arcstones $@ -casename GemPFArcsN1.D360.R190.c.o -narcs 1 -chord -opaqstip
