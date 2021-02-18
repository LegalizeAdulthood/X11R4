
!
!		(c) Copyright Carnegie Mellon University
!
! See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
! CMU-ITC-071) for full text & permitted uses.
!

!
! Makefile for site type 'vms', directory 'site/vms'
! Generated on Thu Nov  2 10:20:57 1989
!

! Std definitions
CC = cc
INCLUDES = /include_directory=([],[-.-.valid],[-.-.prog],[-.-.raster],[-.-.isode],[-.-.src])
CFLAGS = $(INCLUDES)/standard=portable
LDFLAGS = 
AR1 = library/create
AR2 = library/insert

! Default compilation rule
.c.obj :
	   $(CC) $(CFLAGS) $*.c
	   purge $*.obj



! Rules for LIB "liblocal.olb"

local.obj :  local.c, local.h, lcsite.h

vlocal.obj :  vlocal.c, lcsite.h, [-.-.src]errors.h, vlocal.h 
vlocal.obj :     [-.-.src]odatk.h, [-.-.src]apityp.h, [-.-.src]const.h, [-.-.src]apirt.h 
vlocal.obj :     [-.-.src]attrid.h, [-.-.src]tkerr.h, [-.-.src]tkerr1.h, [-.-.src]const1.h 
vlocal.obj :     [-.-.src]const2.h, [-.-.src]const3.h, [-.-.src]aunion.h, [-.-.src]acd.h 
vlocal.obj :     [-.-.src]attr.h, [-.-.src]attr1.h, [-.-.src]attr2.h, [-.-.src]attr3.h 
vlocal.obj :     [-.-.src]attr4.h, [-.-.src]attr5.h, [-.-.src]attr6.h, [-.-.src]attr7.h 
vlocal.obj :     [-.-.src]attr8.h, [-.-.src]attr9.h, [-.-.src]attr10.h, [-.-.src]attr11.h 
vlocal.obj :     [-.-.src]attr12.h, [-.-.src]attr13.h, [-.-.src]attr15.h, [-.-.src]attr14.h 
vlocal.obj :     [-.-.src]attr16.h, [-.-.src]attr17.h, [-.-.src]attr18.h, [-.-.src]attr19.h 
vlocal.obj :     [-.-.src]attr20.h, [-.-.src]attr21.h, [-.-.src]attr22.h, [-.-.src]attr23.h 
vlocal.obj :     [-.-.src]attr24.h, [-.-.src]attr25.h, [-.-.src]attr26.h, [-.-.src]doc.h 
vlocal.obj :     [-.-.src]doc1.h, [-.-.src]atts.h, [-.-.src]atts1.h, [-.-.src]atts2.h 
vlocal.obj :     [-.-.src]atts3.h, [-.-.src]atts4.h, [-.-.src]debug.h, [-.-.src]debug1.h 
vlocal.obj :     [-.-.src]debug2.h, [-.-.src]expr.h, [-.-.src]expr1.h, [-.-.src]font.h 
vlocal.obj :     [-.-.src]font1.h, [-.-.src]init.h, [-.-.src]tags.h, [-.-.src]str.h 
vlocal.obj :     [-.-.src]seq.h, [-.-.src]seq1.h, [-.-.src]str1.h, [-.-.src]init1.h 
vlocal.obj :     [-.-.src]iter.h, [-.-.src]iter1.h, [-.-.src]iter2.h, [-.-.src]iter3.h 
vlocal.obj :     [-.-.src]iter4.h, [-.-.src]label.h, [-.-.src]label1.h, local.h 
vlocal.obj :     [-.-.src]mem.h, [-.-.src]mem1.h, [-.-.src]parms.h, [-.-.src]prior.h 
vlocal.obj :     [-.-.src]prior1.h, [-.-.src]rodif.h, [-.-.src]io.h, [-.-.src]io1.h 
vlocal.obj :     [-.-.src]mm.h, [-.-.src]mm1.h, [-.-.src]rasn1.h, [-.-.src]values.h 
vlocal.obj :     [-.-.src]asn1.h, [-.-.src]rasn11.h, [-.-.src]rasn12.h, [-.-.src]rasn13.h 
vlocal.obj :     [-.-.src]rasn14.h, [-.-.src]rodif1.h, [-.-.src]rodif2.h, [-.-.src]rodif3.h 
vlocal.obj :     [-.-.src]rodif4.h, [-.-.src]wodif.h, [-.-.src]wasn1.h, [-.-.src]wasn11.h 
vlocal.obj :     [-.-.src]wasn12.h, [-.-.src]wasn13.h, [-.-.src]wasn14.h, [-.-.src]wasn15.h 
vlocal.obj :     [-.-.src]wodif1.h, [-.-.src]wodif2.h, [-.-.src]wodif3.h, [-.-.src]wodif4.h

liblocal.olb :    local.obj, vlocal.obj
	   $(AR1) liblocal.olb local.obj, vlocal.obj
	   purge liblocal.olb
