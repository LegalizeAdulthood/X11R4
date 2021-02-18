#!/bin/sh
#XDrawPoints
./pointstones $@ -casename GemPointsTLH -top -left -horiz
./pointstones $@ -casename GemPointsTRH -top -right -horiz
./pointstones $@ -casename GemPointsTLV -top -left -vert
./pointstones $@ -casename GemPointsTRV -top -right -vert
./pointstones $@ -casename GemPointsBLH -bottom -left -horiz
./pointstones $@ -casename GemPointsBRH -bottom -right -horiz
./pointstones $@ -casename GemPointsBLV -bottom -left -vert
./pointstones $@ -casename GemPointsBRV -bottom -right -vert
