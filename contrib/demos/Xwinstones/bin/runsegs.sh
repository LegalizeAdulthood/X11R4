#!/bin/sh
#XDrawSegments
./segstones $@ -casename GemSegs.N1612.Solid.Copy -nsegs 1612
./segstones $@ -casename GemSegs.N1612.Solid.Xor -alu GXxor -nsegs 1612
./segstones $@ -casename GemSegs.N1612.Dash.Copy  -dash 2 4 4  -nsegs 1612
./segstones $@ -casename GemSegs.N1612.Dash.Xor -alu GXxor -dash 2 4 4 -nsegs 1612
