
!
!		(c) Copyright Carnegie Mellon University
!
! See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
! CMU-ITC-071) for full text & permitted uses.
!

!
! Makefile for site type 'vms', directory 'valid'
! Generated on Thu Nov  2 10:20:58 1989
!

! Std definitions
CC = cc
INCLUDES = /include_directory=([],[-.prog],[-.raster],[-.isode],[-.src],[-.site.vms])
CFLAGS = $(INCLUDES)/standard=portable
LDFLAGS = 
AR1 = library/create
AR2 = library/insert

! Default compilation rule
.c.obj :
	   $(CC) $(CFLAGS) $*.c
	   purge $*.obj

! Rules for PROG "valid"

valid.obj :  valid.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
valid.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
valid.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
valid.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
valid.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
valid.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
valid.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
valid.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
valid.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
valid.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
valid.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
valid.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
valid.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
valid.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
valid.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
valid.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
valid.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
valid.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
valid.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
valid.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
valid.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
valid.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
valid.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
valid.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
valid.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
valid.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, [-.site.vms]vlocal.h 
valid.obj :     vstruc.h

vtest1.obj :  vtest1.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
vtest1.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
vtest1.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
vtest1.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
vtest1.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
vtest1.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
vtest1.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
vtest1.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
vtest1.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
vtest1.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
vtest1.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
vtest1.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
vtest1.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
vtest1.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
vtest1.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
vtest1.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
vtest1.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
vtest1.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
vtest1.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
vtest1.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
vtest1.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
vtest1.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
vtest1.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
vtest1.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
vtest1.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
vtest1.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

vtest2.obj :  vtest2.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
vtest2.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
vtest2.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
vtest2.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
vtest2.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
vtest2.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
vtest2.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
vtest2.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
vtest2.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
vtest2.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
vtest2.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
vtest2.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
vtest2.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
vtest2.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
vtest2.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
vtest2.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
vtest2.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
vtest2.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
vtest2.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
vtest2.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
vtest2.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
vtest2.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
vtest2.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
vtest2.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
vtest2.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
vtest2.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

vtest3.obj :  vtest3.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
vtest3.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
vtest3.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
vtest3.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
vtest3.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
vtest3.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
vtest3.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
vtest3.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
vtest3.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
vtest3.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
vtest3.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
vtest3.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
vtest3.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
vtest3.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
vtest3.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
vtest3.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
vtest3.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
vtest3.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
vtest3.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
vtest3.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
vtest3.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
vtest3.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
vtest3.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
vtest3.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
vtest3.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
vtest3.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

vtest4.obj :  vtest4.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
vtest4.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
vtest4.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
vtest4.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
vtest4.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
vtest4.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
vtest4.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
vtest4.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
vtest4.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
vtest4.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
vtest4.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
vtest4.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
vtest4.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
vtest4.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
vtest4.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
vtest4.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
vtest4.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
vtest4.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
vtest4.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
vtest4.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
vtest4.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
vtest4.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
vtest4.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
vtest4.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
vtest4.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
vtest4.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

vtest5.obj :  vtest5.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
vtest5.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
vtest5.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
vtest5.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
vtest5.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
vtest5.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
vtest5.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
vtest5.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
vtest5.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
vtest5.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
vtest5.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
vtest5.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
vtest5.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
vtest5.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
vtest5.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
vtest5.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
vtest5.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
vtest5.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
vtest5.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
vtest5.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
vtest5.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
vtest5.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
vtest5.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
vtest5.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
vtest5.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
vtest5.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

vtest6.obj :  vtest6.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
vtest6.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
vtest6.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
vtest6.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
vtest6.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
vtest6.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
vtest6.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
vtest6.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
vtest6.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
vtest6.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
vtest6.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
vtest6.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
vtest6.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
vtest6.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
vtest6.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
vtest6.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
vtest6.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
vtest6.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
vtest6.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
vtest6.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
vtest6.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
vtest6.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
vtest6.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
vtest6.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
vtest6.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
vtest6.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

vtest7.obj :  vtest7.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
vtest7.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
vtest7.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
vtest7.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
vtest7.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
vtest7.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
vtest7.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
vtest7.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
vtest7.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
vtest7.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
vtest7.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
vtest7.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
vtest7.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
vtest7.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
vtest7.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
vtest7.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
vtest7.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
vtest7.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
vtest7.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
vtest7.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
vtest7.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
vtest7.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
vtest7.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
vtest7.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
vtest7.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
vtest7.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

vsupp.obj :  vsupp.c, [-.src]odatk.h, [-.site.vms]lcsite.h, [-.src]apityp.h 
vsupp.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
vsupp.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
vsupp.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
vsupp.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
vsupp.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
vsupp.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
vsupp.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
vsupp.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
vsupp.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
vsupp.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
vsupp.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
vsupp.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
vsupp.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
vsupp.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
vsupp.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
vsupp.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
vsupp.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
vsupp.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
vsupp.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
vsupp.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
vsupp.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
vsupp.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
vsupp.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
vsupp.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
vsupp.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h

valid : valid.obj, vtest1.obj, vtest2.obj, vtest3.obj, vtest4.obj, vtest5.obj 
valid :    vtest6.obj, vtest7.obj, vsupp.obj, [-.src]liboda.olb 
valid :    [-.site.vms]liblocal.olb
	link/exec=valid valid.obj, vtest1.obj, vtest2.obj, vtest3.obj, vtest4.obj, vtest5.obj , \
	    vtest6.obj, vtest7.obj, vsupp.obj, [-.src]liboda.olb/library , \
	    [-.site.vms]liblocal.olb/library, SYS$LIBRARY:VAXCRTL.OLB/library

