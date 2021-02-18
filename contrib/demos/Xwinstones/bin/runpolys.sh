#!/bin/sh
#XFillPolygon (Solid Complex)
./polystones $@ -casename GemPoly.Bowtie.Wind -file ./things/bowtie -complex -winding
./polystones $@ -casename GemPoly.Bowtie.Even -file ./things/bowtie -complex -evenodd
./polystones $@ -casename GemPoly.Halfstar.Wind -file ./things/halfstar -complex -winding
./polystones $@ -casename GemPoly.Halfstar.Even -file ./things/halfstar -complex -evenodd
./polystones $@ -casename GemPoly.Super390.Wind -file ./things/super.390 -complex -winding
./polystones $@ -casename GemPoly.Super390.Even -file ./things/super.390 -complex -evenodd
./polystones $@ -casename GemPoly.String390.Wind -file ./things/string.390 -complex -winding
./polystones $@ -casename GemPoly.String390.Even -file ./things/string.390 -complex -evenodd
