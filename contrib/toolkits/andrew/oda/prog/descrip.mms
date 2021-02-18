
!
!		(c) Copyright Carnegie Mellon University
!
! See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
! CMU-ITC-071) for full text & permitted uses.
!

!
! Makefile for site type 'vms', directory 'prog'
! Generated on Thu Nov  2 10:20:59 1989
!

! Std definitions
CC = cc
INCLUDES = /include_directory=([],[-.valid],[-.raster],[-.isode],[-.src],[-.site.vms])
CFLAGS = $(INCLUDES)/standard=portable
LDFLAGS = 
AR1 = library/create
AR2 = library/insert

! Default compilation rule
.c.obj :
	   $(CC) $(CFLAGS) $*.c
	   purge $*.obj

! Rules for PROG "convert"

convert.obj :  convert.c, [-.raster]rgtk.h, [-.raster]rgsite.h, [-.raster]rgreal.h 
convert.obj :     [-.site.vms]lcsite.h, [-.site.vms]local.h, [-.raster]rgdbg.h, [-.raster]rgerr.h 
convert.obj :     [-.raster]rginit.h, [-.raster]raster.h, [-.raster]rasio.h, [-.raster]iog3.h 
convert.obj :     [-.raster]iog4.h, [-.raster]ioobm.h, [-.raster]iopbm.h, [-.raster]iomacp.h 
convert.obj :     [-.raster]iops.h, [-.raster]ioandy.h, [-.raster]iox.h, [-.raster]iofbm.h 
convert.obj :     [-.raster]ioetc.h, [-.raster]rglist.h, [-.raster]pel.h

convert : convert.obj, [-.raster]libraster.olb, [-.site.vms]liblocal.olb
	link/exec=convert convert.obj, [-.raster]libraster.olb/library, [-.site.vms]liblocal.olb/library, SYS$LIBRARY:VAXCRTL.OLB/library

! Rules for PROG "prtext"

prtext.obj :  prtext.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
prtext.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
prtext.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
prtext.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
prtext.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
prtext.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
prtext.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
prtext.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
prtext.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
prtext.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
prtext.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
prtext.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
prtext.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
prtext.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
prtext.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
prtext.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
prtext.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
prtext.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
prtext.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
prtext.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
prtext.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
prtext.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
prtext.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
prtext.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
prtext.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
prtext.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

prtext : prtext.obj, [-.src]liboda.olb, [-.site.vms]liblocal.olb
	link/exec=prtext prtext.obj, [-.src]liboda.olb/library, [-.site.vms]liblocal.olb/library, SYS$LIBRARY:VAXCRTL.OLB/library

! Rules for PROG "proda"

proda.obj :  proda.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
proda.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
proda.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
proda.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
proda.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
proda.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
proda.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
proda.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
proda.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
proda.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
proda.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
proda.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
proda.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
proda.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
proda.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
proda.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
proda.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
proda.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
proda.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
proda.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
proda.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
proda.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
proda.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
proda.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
proda.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
proda.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

proda : proda.obj, [-.src]liboda.olb, [-.site.vms]liblocal.olb
	link/exec=proda proda.obj, [-.src]liboda.olb/library, [-.site.vms]liblocal.olb/library, SYS$LIBRARY:VAXCRTL.OLB/library

! Rules for PROG "prodif"

prodif.obj :  prodif.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
prodif.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
prodif.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
prodif.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
prodif.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
prodif.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
prodif.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
prodif.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
prodif.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
prodif.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
prodif.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
prodif.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
prodif.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
prodif.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
prodif.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
prodif.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
prodif.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
prodif.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
prodif.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
prodif.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
prodif.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
prodif.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
prodif.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
prodif.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
prodif.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
prodif.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, [-.isode]odaiso.h 
prodif.obj :     [-.isode]isrd.h, [-.isode]isrd19.h, [-.isode]isrd20.h, [-.isode]ishsh.h 
prodif.obj :     [-.isode]iswr.h, [-.isode]iswr19.h, [-.isode]iswr20.h, [-.isode]ispr.h 
prodif.obj :     [-.isode]ispr19.h

prodif : prodif.obj, [-.isode]libisode.olb, [-.src]liboda.olb, [-.site.vms]liblocal.olb
	link/exec=prodif prodif.obj, [-.isode]libisode.olb/library, [-.src]liboda.olb/library, [-.site.vms]liblocal.olb/library, SYS$LIBRARY:VAXCRTL.OLB/library

