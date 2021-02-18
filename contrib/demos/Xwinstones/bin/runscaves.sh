#!/bin/sh
#XFillPolygon (Solid Concave)
./polystones $@ -casename GemSCPoly.Vee50 -file ./things/vee50 -nonconvex
./polystones $@ -casename GemSCPoly.Vee100 -file ./things/vee100 -nonconvex
./polystones $@ -casename GemSCPoly.Vee200 -file ./things/vee200 -nonconvex 
./polystones $@ -casename GemSCPoly.Vee5000 -file ./things/vee5000 -nonconvex
