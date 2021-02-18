#!/bin/sh
#XDrawLines (Width 3)
./linestones $@ -casename GemLines3house -file things/house -wide 3
./linestones $@ -casename GemLines3houseXor -alu GXxor -file things/house -wide 3
./linestones $@ -casename GemLines3houseDash -file things/house -dash 2 4 4 -wide 3
./linestones $@ -casename GemLines3houseXorDash -alu GXxor -file things/house -dash 2 4 4 -wide 3
