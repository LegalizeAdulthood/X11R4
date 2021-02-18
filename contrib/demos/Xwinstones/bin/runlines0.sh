#!/bin/sh
#XDrawLines (Width 0)
./linestones $@ -casename GemLines0house -file things/house
./linestones $@ -casename GemLines0houseXor -alu GXxor -file things/house
./linestones $@ -casename GemLines0houseDash -file things/house -dash 2 4 4
./linestones $@ -casename GemLines0houseXorDash -alu GXxor -file things/house -dash 2 4 4
