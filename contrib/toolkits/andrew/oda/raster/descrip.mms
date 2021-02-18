
!
!		(c) Copyright Carnegie Mellon University
!
! See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
! CMU-ITC-071) for full text & permitted uses.
!

!
! Makefile for site type 'vms', directory 'raster'
! Generated on Thu Nov  2 10:21:00 1989
!

! Std definitions
CC = cc
INCLUDES = /include_directory=([],[-.valid],[-.prog],[-.isode],[-.src],[-.site.vms])
CFLAGS = $(INCLUDES)/standard=portable
LDFLAGS = 
AR1 = library/create
AR2 = library/insert

! Default compilation rule
.c.obj :
	   $(CC) $(CFLAGS) $*.c
	   purge $*.obj



! Rules for LIB "libraster.olb"

rgmem.obj :  rgmem.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
rgmem.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rginit.h 
rgmem.obj :     raster.h, rgmem.h

rglist.obj :  rglist.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
rglist.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
rglist.obj :     rglist.h

rginit.obj :  rginit.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
rginit.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rginit.h 
rginit.obj :     raster.h

rgdbg.obj :  rgdbg.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
rgdbg.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h

raster.obj :  raster.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
raster.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
raster.obj :     pel.h, rglist.h, raster.h

rasio.obj :  rasio.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
rasio.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rginit.h 
rasio.obj :     raster.h, rasio.h, iog3.h, iog4.h 
rasio.obj :     ioobm.h, iopbm.h, iomacp.h, iops.h 
rasio.obj :     ioandy.h, iox.h, iofbm.h, ioetc.h

pel.obj :    pel.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
pel.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, raster.h 
pel.obj :     pel.h

iox.obj :    iox.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
iox.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
iox.obj :     raster.h, rasio.h, iog3.h, iog4.h 
iox.obj :     ioobm.h, iopbm.h, iomacp.h, iops.h 
iox.obj :     ioandy.h, iox.h, iofbm.h, ioetc.h 
iox.obj :     rglist.h

iops.obj :   iops.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
iops.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, raster.h 
iops.obj :     rasio.h, iog3.h, iog4.h, ioobm.h 
iops.obj :     iopbm.h, iomacp.h, iops.h, ioandy.h 
iops.obj :     iox.h, iofbm.h, ioetc.h

iopbm.obj :  iopbm.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
iopbm.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
iopbm.obj :     raster.h, rasio.h, iog3.h, iog4.h 
iopbm.obj :     ioobm.h, iopbm.h, iomacp.h, iops.h 
iopbm.obj :     ioandy.h, iox.h, iofbm.h, ioetc.h 
iopbm.obj :     pel.h

ioobm.obj :  ioobm.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
ioobm.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, raster.h 
ioobm.obj :     rasio.h, iog3.h, iog4.h, ioobm.h 
ioobm.obj :     iopbm.h, iomacp.h, iops.h, ioandy.h 
ioobm.obj :     iox.h, iofbm.h, ioetc.h, rglist.h

iomacp.obj :  iomacp.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
iomacp.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
iomacp.obj :     raster.h, rasio.h, iog3.h, iog4.h 
iomacp.obj :     ioobm.h, iopbm.h, iomacp.h, iops.h 
iomacp.obj :     ioandy.h, iox.h, iofbm.h, ioetc.h 
iomacp.obj :     pel.h, rglist.h

iog4.obj :   iog4.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
iog4.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
iog4.obj :     raster.h, rasio.h, iog3.h, iog4.h 
iog4.obj :     ioobm.h, iopbm.h, iomacp.h, iops.h 
iog4.obj :     ioandy.h, iox.h, iofbm.h, ioetc.h 
iog4.obj :     iog32.h, iog33.h, rglist.h

iog33.obj :  iog33.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
iog33.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
iog33.obj :     raster.h, rasio.h, iog3.h, iog4.h 
iog33.obj :     ioobm.h, iopbm.h, iomacp.h, iops.h 
iog33.obj :     ioandy.h, iox.h, iofbm.h, ioetc.h 
iog33.obj :     iog32.h, iog33.h, rglist.h

iog32.obj :  iog32.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
iog32.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, raster.h 
iog32.obj :     rasio.h, iog3.h, iog4.h, ioobm.h 
iog32.obj :     iopbm.h, iomacp.h, iops.h, ioandy.h 
iog32.obj :     iox.h, iofbm.h, ioetc.h, iog32.h 
iog32.obj :     rglist.h

iog3.obj :   iog3.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
iog3.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
iog3.obj :     raster.h, rasio.h, iog3.h, iog4.h 
iog3.obj :     ioobm.h, iopbm.h, iomacp.h, iops.h 
iog3.obj :     ioandy.h, iox.h, iofbm.h, ioetc.h 
iog3.obj :     iog32.h, iog33.h, rglist.h

iofbm.obj :  iofbm.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
iofbm.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
iofbm.obj :     pel.h, raster.h, rasio.h, iog3.h 
iofbm.obj :     iog4.h, ioobm.h, iopbm.h, iomacp.h 
iofbm.obj :     iops.h, ioandy.h, iox.h, iofbm.h 
iofbm.obj :     ioetc.h, fbm.h

ioetc.obj :  ioetc.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
ioetc.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, raster.h 
ioetc.obj :     rasio.h, iog3.h, iog4.h, ioobm.h 
ioetc.obj :     iopbm.h, iomacp.h, iops.h, ioandy.h 
ioetc.obj :     iox.h, iofbm.h, ioetc.h

ioandy.obj :  ioandy.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
ioandy.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, rgmem.h 
ioandy.obj :     pel.h, raster.h, rasio.h, iog3.h 
ioandy.obj :     iog4.h, ioobm.h, iopbm.h, iomacp.h 
ioandy.obj :     iops.h, ioandy.h, iox.h, iofbm.h 
ioandy.obj :     ioetc.h

g3code.obj :  g3code.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
g3code.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, raster.h 
g3code.obj :     iog3.h

flsun.obj :  flsun.c, fbm.h, rgsite.h, rgreal.h 
flsun.obj :     [-.site.vms]lcsite.h, [-.site.vms]local.h, rgdbg.h, rgerr.h

fliff.obj :  fliff.c, fbm.h, rgsite.h, rgreal.h 
fliff.obj :     [-.site.vms]lcsite.h, [-.site.vms]local.h, rgdbg.h, rgerr.h

flgifw.obj :  flgifw.c, fbm.h, rgsite.h, rgreal.h 
flgifw.obj :     [-.site.vms]lcsite.h, [-.site.vms]local.h, rgdbg.h, rgerr.h

flgifr.obj :  flgifr.c, fbm.h, rgsite.h, rgreal.h 
flgifr.obj :     [-.site.vms]lcsite.h, [-.site.vms]local.h, rgdbg.h, rgerr.h

flgife.obj :  flgife.c, fbm.h, rgsite.h, rgreal.h 
flgife.obj :     [-.site.vms]lcsite.h, [-.site.vms]local.h, rgdbg.h, rgerr.h

flgifc.obj :  flgifc.c, rgsite.h, rgreal.h, [-.site.vms]lcsite.h 
flgifc.obj :     [-.site.vms]local.h, rgdbg.h, rgerr.h, fbm.h

flbyte.obj :  flbyte.c, fbm.h, rgsite.h, rgreal.h 
flbyte.obj :     [-.site.vms]lcsite.h, [-.site.vms]local.h, rgdbg.h, rgerr.h

flalfb.obj :  flalfb.c, fbm.h, rgsite.h, rgreal.h 
flalfb.obj :     [-.site.vms]lcsite.h, [-.site.vms]local.h, rgdbg.h, rgerr.h

libraster.olb :    rgmem.obj, rglist.obj, rginit.obj, rgdbg.obj, raster.obj, rasio.obj 
libraster.olb :    pel.obj, iox.obj, iops.obj, iopbm.obj, ioobm.obj, iomacp.obj 
libraster.olb :    iog4.obj, iog33.obj, iog32.obj, iog3.obj, iofbm.obj, ioetc.obj 
libraster.olb :    ioandy.obj, g3code.obj, flsun.obj, fliff.obj, flgifw.obj, flgifr.obj 
libraster.olb :    flgife.obj, flgifc.obj, flbyte.obj, flalfb.obj
	   $(AR1) libraster.olb rgmem.obj, rglist.obj, rginit.obj, rgdbg.obj, raster.obj 
	   $(AR2) libraster.olb rasio.obj, pel.obj, iox.obj, iops.obj, iopbm.obj 
	   $(AR2) libraster.olb ioobm.obj, iomacp.obj, iog4.obj, iog33.obj, iog32.obj 
	   $(AR2) libraster.olb iog3.obj, iofbm.obj, ioetc.obj, ioandy.obj, g3code.obj 
	   $(AR2) libraster.olb flsun.obj, fliff.obj, flgifw.obj, flgifr.obj, flgife.obj 
	   $(AR2) libraster.olb flgifc.obj, flbyte.obj, flalfb.obj
	   purge libraster.olb
