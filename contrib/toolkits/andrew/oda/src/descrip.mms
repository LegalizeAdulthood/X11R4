
!
!		(c) Copyright Carnegie Mellon University
!
! See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
! CMU-ITC-071) for full text & permitted uses.
!

!
! Makefile for site type 'vms', directory 'src'
! Generated on Thu Nov  2 10:21:09 1989
!

! Std definitions
CC = cc
INCLUDES = /include_directory=([],[-.valid],[-.prog],[-.raster],[-.isode],[-.site.vms])
CFLAGS = $(INCLUDES)/standard=portable
LDFLAGS = 
AR1 = library/create
AR2 = library/insert

! Default compilation rule
.c.obj :
	   $(CC) $(CFLAGS) $*.c
	   purge $*.obj



! Rules for LIB "liboda.olb"

wasn14.obj :  wasn14.c, wasn1.h, [-.site.vms]lcsite.h, tkerr.h 
wasn14.obj :     errors.h, tkerr1.h, values.h, io.h 
wasn14.obj :     io1.h, acd.h, attr.h, apirt.h 
wasn14.obj :     aunion.h, apityp.h, const.h, attrid.h 
wasn14.obj :     const1.h, const2.h, const3.h, attr1.h 
wasn14.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
wasn14.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
wasn14.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
wasn14.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
wasn14.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
wasn14.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
wasn14.obj :     attr26.h, doc.h, doc1.h, asn1.h 
wasn14.obj :     wodif.h, debug.h, debug1.h, debug2.h 
wasn14.obj :     wodif1.h, wodif2.h, wodif3.h, wodif4.h 
wasn14.obj :     wasn11.h, wasn12.h, wasn13.h, wasn14.h 
wasn14.obj :     wasn15.h, tags.h, parms.h

wodif3.obj :  wodif3.c, wodif.h, [-.site.vms]lcsite.h, acd.h 
wodif3.obj :     attr.h, errors.h, apirt.h, tkerr.h 
wodif3.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
wodif3.obj :     attrid.h, const1.h, const2.h, const3.h 
wodif3.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
wodif3.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
wodif3.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
wodif3.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
wodif3.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
wodif3.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
wodif3.obj :     attr25.h, attr26.h, doc.h, doc1.h 
wodif3.obj :     debug.h, debug1.h, debug2.h, values.h 
wodif3.obj :     io.h, io1.h, asn1.h, wasn1.h 
wodif3.obj :     wasn11.h, wasn12.h, wasn13.h, wasn14.h 
wodif3.obj :     wasn15.h, wodif1.h, wodif2.h, wodif3.h 
wodif3.obj :     wodif4.h, tags.h

wodif2.obj :  wodif2.c, wodif.h, [-.site.vms]lcsite.h, acd.h 
wodif2.obj :     attr.h, errors.h, apirt.h, tkerr.h 
wodif2.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
wodif2.obj :     attrid.h, const1.h, const2.h, const3.h 
wodif2.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
wodif2.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
wodif2.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
wodif2.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
wodif2.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
wodif2.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
wodif2.obj :     attr25.h, attr26.h, doc.h, doc1.h 
wodif2.obj :     debug.h, debug1.h, debug2.h, values.h 
wodif2.obj :     io.h, io1.h, asn1.h, wasn1.h 
wodif2.obj :     wasn11.h, wasn12.h, wasn13.h, wasn14.h 
wodif2.obj :     wasn15.h, wodif1.h, wodif2.h, wodif3.h 
wodif2.obj :     wodif4.h, tags.h

attr16.obj :  attr16.c, parms.h, acd.h, attr.h 
attr16.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr16.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr16.obj :     attrid.h, const1.h, const2.h, const3.h 
attr16.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr16.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr16.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr16.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr16.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr16.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr16.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr16.obj :     str.h, tags.h, seq.h, seq1.h 
attr16.obj :     str1.h, mm.h, [-.site.vms]local.h, mm1.h 
attr16.obj :     attr0.h, rnames.h

wodif4.obj :  wodif4.c, wodif.h, [-.site.vms]lcsite.h, acd.h 
wodif4.obj :     attr.h, errors.h, apirt.h, tkerr.h 
wodif4.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
wodif4.obj :     attrid.h, const1.h, const2.h, const3.h 
wodif4.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
wodif4.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
wodif4.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
wodif4.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
wodif4.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
wodif4.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
wodif4.obj :     attr25.h, attr26.h, doc.h, doc1.h 
wodif4.obj :     debug.h, debug1.h, debug2.h, values.h 
wodif4.obj :     io.h, io1.h, asn1.h, wasn1.h 
wodif4.obj :     wasn11.h, wasn12.h, wasn13.h, wasn14.h 
wodif4.obj :     wasn15.h, wodif1.h, wodif2.h, wodif3.h 
wodif4.obj :     wodif4.h, tags.h, font.h, font1.h 
wodif4.obj :     seq.h, seq1.h

wasn13.obj :  wasn13.c, parms.h, wasn1.h, [-.site.vms]lcsite.h 
wasn13.obj :     tkerr.h, errors.h, tkerr1.h, values.h 
wasn13.obj :     io.h, io1.h, acd.h, attr.h 
wasn13.obj :     apirt.h, aunion.h, apityp.h, const.h 
wasn13.obj :     attrid.h, const1.h, const2.h, const3.h 
wasn13.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
wasn13.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
wasn13.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
wasn13.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
wasn13.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
wasn13.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
wasn13.obj :     attr25.h, attr26.h, doc.h, doc1.h 
wasn13.obj :     asn1.h, wodif.h, debug.h, debug1.h 
wasn13.obj :     debug2.h, wodif1.h, wodif2.h, wodif3.h 
wasn13.obj :     wodif4.h, wasn11.h, wasn12.h, wasn13.h 
wasn13.obj :     wasn14.h, wasn15.h

wasn12.obj :  wasn12.c, seq.h, [-.site.vms]lcsite.h, errors.h 
wasn12.obj :     apirt.h, apityp.h, const.h, attrid.h 
wasn12.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
wasn12.obj :     const3.h, tags.h, seq1.h, wasn1.h 
wasn12.obj :     values.h, io.h, io1.h, acd.h 
wasn12.obj :     attr.h, aunion.h, attr1.h, attr2.h 
wasn12.obj :     attr3.h, attr4.h, attr5.h, attr6.h 
wasn12.obj :     attr7.h, attr8.h, attr9.h, attr10.h 
wasn12.obj :     attr11.h, attr12.h, attr13.h, attr15.h 
wasn12.obj :     attr14.h, attr16.h, attr17.h, attr18.h 
wasn12.obj :     attr19.h, attr20.h, attr21.h, attr22.h 
wasn12.obj :     attr23.h, attr24.h, attr25.h, attr26.h 
wasn12.obj :     doc.h, doc1.h, asn1.h, wodif.h 
wasn12.obj :     debug.h, debug1.h, debug2.h, wodif1.h 
wasn12.obj :     wodif2.h, wodif3.h, wodif4.h, wasn11.h 
wasn12.obj :     wasn12.h, wasn13.h, wasn14.h, wasn15.h 
wasn12.obj :     parms.h

mem1.obj :   mem1.c, [-.site.vms]lcsite.h, acd.h, attr.h 
mem1.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
mem1.obj :     aunion.h, apityp.h, const.h, attrid.h 
mem1.obj :     const1.h, const2.h, const3.h, attr1.h 
mem1.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
mem1.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
mem1.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
mem1.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
mem1.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
mem1.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
mem1.obj :     attr26.h, doc.h, doc1.h, mem.h 
mem1.obj :     mem1.h

wasn15.obj :  wasn15.c, wasn1.h, [-.site.vms]lcsite.h, tkerr.h 
wasn15.obj :     errors.h, tkerr1.h, values.h, io.h 
wasn15.obj :     io1.h, acd.h, attr.h, apirt.h 
wasn15.obj :     aunion.h, apityp.h, const.h, attrid.h 
wasn15.obj :     const1.h, const2.h, const3.h, attr1.h 
wasn15.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
wasn15.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
wasn15.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
wasn15.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
wasn15.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
wasn15.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
wasn15.obj :     attr26.h, doc.h, doc1.h, asn1.h 
wasn15.obj :     wodif.h, debug.h, debug1.h, debug2.h 
wasn15.obj :     wodif1.h, wodif2.h, wodif3.h, wodif4.h 
wasn15.obj :     wasn11.h, wasn12.h, wasn13.h, wasn14.h 
wasn15.obj :     wasn15.h, tags.h, parms.h, font.h 
wasn15.obj :     font1.h

wasn11.obj :  wasn11.c, wasn1.h, [-.site.vms]lcsite.h, tkerr.h 
wasn11.obj :     errors.h, tkerr1.h, values.h, io.h 
wasn11.obj :     io1.h, acd.h, attr.h, apirt.h 
wasn11.obj :     aunion.h, apityp.h, const.h, attrid.h 
wasn11.obj :     const1.h, const2.h, const3.h, attr1.h 
wasn11.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
wasn11.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
wasn11.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
wasn11.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
wasn11.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
wasn11.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
wasn11.obj :     attr26.h, doc.h, doc1.h, asn1.h 
wasn11.obj :     wodif.h, debug.h, debug1.h, debug2.h 
wasn11.obj :     wodif1.h, wodif2.h, wodif3.h, wodif4.h 
wasn11.obj :     wasn11.h, wasn12.h, wasn13.h, wasn14.h 
wasn11.obj :     wasn15.h

wodif1.obj :  wodif1.c, wodif.h, [-.site.vms]lcsite.h, acd.h 
wodif1.obj :     attr.h, errors.h, apirt.h, tkerr.h 
wodif1.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
wodif1.obj :     attrid.h, const1.h, const2.h, const3.h 
wodif1.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
wodif1.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
wodif1.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
wodif1.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
wodif1.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
wodif1.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
wodif1.obj :     attr25.h, attr26.h, doc.h, doc1.h 
wodif1.obj :     debug.h, debug1.h, debug2.h, values.h 
wodif1.obj :     io.h, io1.h, asn1.h, wasn1.h 
wodif1.obj :     wasn11.h, wasn12.h, wasn13.h, wasn14.h 
wodif1.obj :     wasn15.h, wodif1.h, wodif2.h, wodif3.h 
wodif1.obj :     wodif4.h, seq.h, tags.h, seq1.h 
wodif1.obj :     label.h, label1.h, init.h, str.h 
wodif1.obj :     str1.h, init1.h

seq1.obj :   seq1.c, rnames.h, seq.h, [-.site.vms]lcsite.h 
seq1.obj :     errors.h, apirt.h, apityp.h, const.h 
seq1.obj :     attrid.h, tkerr.h, tkerr1.h, const1.h 
seq1.obj :     const2.h, const3.h, tags.h, seq1.h 
seq1.obj :     attr.h, aunion.h, attr1.h, attr2.h 
seq1.obj :     attr3.h, attr4.h, attr5.h, attr6.h 
seq1.obj :     attr7.h, attr8.h, attr9.h, attr10.h 
seq1.obj :     attr11.h, attr12.h, attr13.h, attr15.h 
seq1.obj :     attr14.h, attr16.h, attr17.h, attr18.h 
seq1.obj :     attr19.h, attr20.h, attr21.h, attr22.h 
seq1.obj :     attr23.h, attr24.h, attr25.h, attr26.h 
seq1.obj :     mm.h, [-.site.vms]local.h, mm1.h, str.h 
seq1.obj :     str1.h, values.h, parms.h

rodif4.obj :  rodif4.c, font.h, [-.site.vms]lcsite.h, errors.h 
rodif4.obj :     apirt.h, apityp.h, const.h, attrid.h 
rodif4.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
rodif4.obj :     const3.h, font1.h, rodif.h, tags.h 
rodif4.obj :     acd.h, attr.h, aunion.h, attr1.h 
rodif4.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
rodif4.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
rodif4.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
rodif4.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
rodif4.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
rodif4.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
rodif4.obj :     attr26.h, doc.h, doc1.h, seq.h 
rodif4.obj :     seq1.h, io.h, io1.h, parms.h 
rodif4.obj :     mm.h, [-.site.vms]local.h, mm1.h, str.h 
rodif4.obj :     str1.h, rasn1.h, values.h, asn1.h 
rodif4.obj :     rasn11.h, rasn12.h, rasn13.h, rasn14.h 
rodif4.obj :     rodif1.h, rodif2.h, rodif3.h, rodif4.h

rodif3.obj :  rodif3.c, rodif.h, [-.site.vms]lcsite.h, errors.h 
rodif3.obj :     apirt.h, tkerr.h, tkerr1.h, tags.h 
rodif3.obj :     acd.h, attr.h, aunion.h, apityp.h 
rodif3.obj :     const.h, attrid.h, const1.h, const2.h 
rodif3.obj :     const3.h, attr1.h, attr2.h, attr3.h 
rodif3.obj :     attr4.h, attr5.h, attr6.h, attr7.h 
rodif3.obj :     attr8.h, attr9.h, attr10.h, attr11.h 
rodif3.obj :     attr12.h, attr13.h, attr15.h, attr14.h 
rodif3.obj :     attr16.h, attr17.h, attr18.h, attr19.h 
rodif3.obj :     attr20.h, attr21.h, attr22.h, attr23.h 
rodif3.obj :     attr24.h, attr25.h, attr26.h, doc.h 
rodif3.obj :     doc1.h, seq.h, seq1.h, io.h 
rodif3.obj :     io1.h, parms.h, mm.h, [-.site.vms]local.h 
rodif3.obj :     mm1.h, str.h, str1.h, rasn1.h 
rodif3.obj :     values.h, asn1.h, rasn11.h, rasn12.h 
rodif3.obj :     rasn13.h, rasn14.h, rodif1.h, rodif2.h 
rodif3.obj :     rodif3.h, rodif4.h

stubs2.obj :  stubs2.c, [-.site.vms]lcsite.h, tkerr.h, errors.h 
stubs2.obj :     tkerr1.h, rnames.h, const.h, apirt.h 
stubs2.obj :     attrid.h, const1.h, const2.h, const3.h 
stubs2.obj :     attr.h, aunion.h, apityp.h, attr1.h 
stubs2.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
stubs2.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
stubs2.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
stubs2.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
stubs2.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
stubs2.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
stubs2.obj :     attr26.h

stubs.obj :  stubs.c, [-.site.vms]lcsite.h, tkerr.h, errors.h 
stubs.obj :     tkerr1.h, rnames.h, const.h, apirt.h 
stubs.obj :     attrid.h, const1.h, const2.h, const3.h 
stubs.obj :     attr.h, aunion.h, apityp.h, attr1.h 
stubs.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
stubs.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
stubs.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
stubs.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
stubs.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
stubs.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
stubs.obj :     attr26.h

str1.obj :   str1.c, mm.h, [-.site.vms]lcsite.h, errors.h 
str1.obj :     apirt.h, tkerr.h, tkerr1.h, [-.site.vms]local.h 
str1.obj :     mm1.h, seq.h, apityp.h, const.h 
str1.obj :     attrid.h, const1.h, const2.h, const3.h 
str1.obj :     tags.h, seq1.h, str.h, str1.h

rodif2.obj :  rodif2.c, rodif.h, [-.site.vms]lcsite.h, errors.h 
rodif2.obj :     apirt.h, tkerr.h, tkerr1.h, tags.h 
rodif2.obj :     acd.h, attr.h, aunion.h, apityp.h 
rodif2.obj :     const.h, attrid.h, const1.h, const2.h 
rodif2.obj :     const3.h, attr1.h, attr2.h, attr3.h 
rodif2.obj :     attr4.h, attr5.h, attr6.h, attr7.h 
rodif2.obj :     attr8.h, attr9.h, attr10.h, attr11.h 
rodif2.obj :     attr12.h, attr13.h, attr15.h, attr14.h 
rodif2.obj :     attr16.h, attr17.h, attr18.h, attr19.h 
rodif2.obj :     attr20.h, attr21.h, attr22.h, attr23.h 
rodif2.obj :     attr24.h, attr25.h, attr26.h, doc.h 
rodif2.obj :     doc1.h, seq.h, seq1.h, io.h 
rodif2.obj :     io1.h, parms.h, mm.h, [-.site.vms]local.h 
rodif2.obj :     mm1.h, str.h, str1.h, rasn1.h 
rodif2.obj :     values.h, asn1.h, rasn11.h, rasn12.h 
rodif2.obj :     rasn13.h, rasn14.h, rodif1.h, rodif2.h 
rodif2.obj :     rodif3.h, rodif4.h

rodif1.obj :  rodif1.c, rodif.h, [-.site.vms]lcsite.h, errors.h 
rodif1.obj :     apirt.h, tkerr.h, tkerr1.h, tags.h 
rodif1.obj :     acd.h, attr.h, aunion.h, apityp.h 
rodif1.obj :     const.h, attrid.h, const1.h, const2.h 
rodif1.obj :     const3.h, attr1.h, attr2.h, attr3.h 
rodif1.obj :     attr4.h, attr5.h, attr6.h, attr7.h 
rodif1.obj :     attr8.h, attr9.h, attr10.h, attr11.h 
rodif1.obj :     attr12.h, attr13.h, attr15.h, attr14.h 
rodif1.obj :     attr16.h, attr17.h, attr18.h, attr19.h 
rodif1.obj :     attr20.h, attr21.h, attr22.h, attr23.h 
rodif1.obj :     attr24.h, attr25.h, attr26.h, doc.h 
rodif1.obj :     doc1.h, seq.h, seq1.h, io.h 
rodif1.obj :     io1.h, parms.h, mm.h, [-.site.vms]local.h 
rodif1.obj :     mm1.h, str.h, str1.h, rasn1.h 
rodif1.obj :     values.h, asn1.h, rasn11.h, rasn12.h 
rodif1.obj :     rasn13.h, rasn14.h, rodif1.h, rodif2.h 
rodif1.obj :     rodif3.h, rodif4.h

rasn13.obj :  rasn13.c, rasn1.h, [-.site.vms]lcsite.h, tkerr.h 
rasn13.obj :     errors.h, tkerr1.h, values.h, io.h 
rasn13.obj :     io1.h, acd.h, attr.h, apirt.h 
rasn13.obj :     aunion.h, apityp.h, const.h, attrid.h 
rasn13.obj :     const1.h, const2.h, const3.h, attr1.h 
rasn13.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
rasn13.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
rasn13.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
rasn13.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
rasn13.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
rasn13.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
rasn13.obj :     attr26.h, doc.h, doc1.h, tags.h 
rasn13.obj :     parms.h, seq.h, seq1.h, asn1.h 
rasn13.obj :     rasn11.h, rasn12.h, rasn13.h, rasn14.h 
rasn13.obj :     rodif.h, mm.h, [-.site.vms]local.h, mm1.h 
rasn13.obj :     str.h, str1.h, rodif1.h, rodif2.h 
rasn13.obj :     rodif3.h, rodif4.h

rasn12.obj :  rasn12.c, rasn1.h, [-.site.vms]lcsite.h, tkerr.h 
rasn12.obj :     errors.h, tkerr1.h, values.h, io.h 
rasn12.obj :     io1.h, acd.h, attr.h, apirt.h 
rasn12.obj :     aunion.h, apityp.h, const.h, attrid.h 
rasn12.obj :     const1.h, const2.h, const3.h, attr1.h 
rasn12.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
rasn12.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
rasn12.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
rasn12.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
rasn12.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
rasn12.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
rasn12.obj :     attr26.h, doc.h, doc1.h, tags.h 
rasn12.obj :     parms.h, seq.h, seq1.h, asn1.h 
rasn12.obj :     rasn11.h, rasn12.h, rasn13.h, rasn14.h 
rasn12.obj :     init.h, str.h, str1.h, init1.h 
rasn12.obj :     mm.h, [-.site.vms]local.h, mm1.h, rodif.h 
rasn12.obj :     rodif1.h, rodif2.h, rodif3.h, rodif4.h

rasn11.obj :  rasn11.c, mm.h, [-.site.vms]lcsite.h, errors.h 
rasn11.obj :     apirt.h, tkerr.h, tkerr1.h, [-.site.vms]local.h 
rasn11.obj :     mm1.h, rasn1.h, values.h, io.h 
rasn11.obj :     io1.h, acd.h, attr.h, aunion.h 
rasn11.obj :     apityp.h, const.h, attrid.h, const1.h 
rasn11.obj :     const2.h, const3.h, attr1.h, attr2.h 
rasn11.obj :     attr3.h, attr4.h, attr5.h, attr6.h 
rasn11.obj :     attr7.h, attr8.h, attr9.h, attr10.h 
rasn11.obj :     attr11.h, attr12.h, attr13.h, attr15.h 
rasn11.obj :     attr14.h, attr16.h, attr17.h, attr18.h 
rasn11.obj :     attr19.h, attr20.h, attr21.h, attr22.h 
rasn11.obj :     attr23.h, attr24.h, attr25.h, attr26.h 
rasn11.obj :     doc.h, doc1.h, tags.h, parms.h 
rasn11.obj :     seq.h, seq1.h, asn1.h, rasn11.h 
rasn11.obj :     rasn12.h, rasn13.h, rasn14.h, str.h 
rasn11.obj :     str1.h

label1.obj :  label1.c, parms.h, str.h, [-.site.vms]lcsite.h 
label1.obj :     apityp.h, const.h, errors.h, apirt.h 
label1.obj :     attrid.h, tkerr.h, tkerr1.h, const1.h 
label1.obj :     const2.h, const3.h, tags.h, seq.h 
label1.obj :     seq1.h, str1.h, label.h, label1.h 
label1.obj :     acd.h, attr.h, aunion.h, attr1.h 
label1.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
label1.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
label1.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
label1.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
label1.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
label1.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
label1.obj :     attr26.h, doc.h, doc1.h, fmt.h 
label1.obj :     fmt1.h, debug.h, debug1.h, debug2.h

iter4.obj :  iter4.c, debug.h, [-.site.vms]lcsite.h, errors.h 
iter4.obj :     apirt.h, tkerr.h, tkerr1.h, debug1.h 
iter4.obj :     debug2.h, mm.h, [-.site.vms]local.h, mm1.h 
iter4.obj :     iter.h, acd.h, attr.h, aunion.h 
iter4.obj :     apityp.h, const.h, attrid.h, const1.h 
iter4.obj :     const2.h, const3.h, attr1.h, attr2.h 
iter4.obj :     attr3.h, attr4.h, attr5.h, attr6.h 
iter4.obj :     attr7.h, attr8.h, attr9.h, attr10.h 
iter4.obj :     attr11.h, attr12.h, attr13.h, attr15.h 
iter4.obj :     attr14.h, attr16.h, attr17.h, attr18.h 
iter4.obj :     attr19.h, attr20.h, attr21.h, attr22.h 
iter4.obj :     attr23.h, attr24.h, attr25.h, attr26.h 
iter4.obj :     doc.h, doc1.h, iter1.h, iter2.h 
iter4.obj :     iter3.h, iter4.h

iter3.obj :  iter3.c, mm.h, [-.site.vms]lcsite.h, errors.h 
iter3.obj :     apirt.h, tkerr.h, tkerr1.h, [-.site.vms]local.h 
iter3.obj :     mm1.h, iter.h, acd.h, attr.h 
iter3.obj :     aunion.h, apityp.h, const.h, attrid.h 
iter3.obj :     const1.h, const2.h, const3.h, attr1.h 
iter3.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
iter3.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
iter3.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
iter3.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
iter3.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
iter3.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
iter3.obj :     attr26.h, doc.h, doc1.h, iter1.h 
iter3.obj :     iter2.h, iter3.h, iter4.h, tags.h

iter2.obj :  iter2.c, debug.h, [-.site.vms]lcsite.h, errors.h 
iter2.obj :     apirt.h, tkerr.h, tkerr1.h, debug1.h 
iter2.obj :     debug2.h, mm.h, [-.site.vms]local.h, mm1.h 
iter2.obj :     iter.h, acd.h, attr.h, aunion.h 
iter2.obj :     apityp.h, const.h, attrid.h, const1.h 
iter2.obj :     const2.h, const3.h, attr1.h, attr2.h 
iter2.obj :     attr3.h, attr4.h, attr5.h, attr6.h 
iter2.obj :     attr7.h, attr8.h, attr9.h, attr10.h 
iter2.obj :     attr11.h, attr12.h, attr13.h, attr15.h 
iter2.obj :     attr14.h, attr16.h, attr17.h, attr18.h 
iter2.obj :     attr19.h, attr20.h, attr21.h, attr22.h 
iter2.obj :     attr23.h, attr24.h, attr25.h, attr26.h 
iter2.obj :     doc.h, doc1.h, iter1.h, iter2.h 
iter2.obj :     iter3.h, iter4.h

prior1.obj :  prior1.c, prior.h, [-.site.vms]lcsite.h, acd.h 
prior1.obj :     attr.h, errors.h, apirt.h, tkerr.h 
prior1.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
prior1.obj :     attrid.h, const1.h, const2.h, const3.h 
prior1.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
prior1.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
prior1.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
prior1.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
prior1.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
prior1.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
prior1.obj :     attr25.h, attr26.h, doc.h, doc1.h 
prior1.obj :     tags.h, prior1.h

io1.obj :    io1.c, [-.site.vms]lcsite.h, errors.h, tkerr.h 
io1.obj :     tkerr1.h, [-.site.vms]local.h, io.h, io1.h

mm1.obj :    mm1.c, mm.h, [-.site.vms]lcsite.h, errors.h 
mm1.obj :     apirt.h, tkerr.h, tkerr1.h, [-.site.vms]local.h 
mm1.obj :     mm1.h

doc1.obj :   doc1.c, acd.h, attr.h, [-.site.vms]lcsite.h 
doc1.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
doc1.obj :     aunion.h, apityp.h, const.h, attrid.h 
doc1.obj :     const1.h, const2.h, const3.h, attr1.h 
doc1.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
doc1.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
doc1.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
doc1.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
doc1.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
doc1.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
doc1.obj :     attr26.h, doc.h, doc1.h, mm.h 
doc1.obj :     [-.site.vms]local.h, mm1.h

debug2.obj :  debug2.c, values.h, acd.h, attr.h 
debug2.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
debug2.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
debug2.obj :     attrid.h, const1.h, const2.h, const3.h 
debug2.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
debug2.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
debug2.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
debug2.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
debug2.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
debug2.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
debug2.obj :     attr25.h, attr26.h, doc.h, doc1.h 
debug2.obj :     attr0.h, rnames.h, str.h, tags.h 
debug2.obj :     seq.h, seq1.h, str1.h, io.h 
debug2.obj :     io1.h, debug.h, debug1.h, debug2.h 
debug2.obj :     init.h, init1.h, parms.h, font.h 
debug2.obj :     font1.h

iter1.obj :  iter1.c, iter.h, [-.site.vms]lcsite.h, acd.h 
iter1.obj :     attr.h, errors.h, apirt.h, tkerr.h 
iter1.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
iter1.obj :     attrid.h, const1.h, const2.h, const3.h 
iter1.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
iter1.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
iter1.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
iter1.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
iter1.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
iter1.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
iter1.obj :     attr25.h, attr26.h, doc.h, doc1.h 
iter1.obj :     iter1.h, iter2.h, iter3.h, iter4.h

debug1.obj :  debug1.c, values.h, acd.h, attr.h 
debug1.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
debug1.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
debug1.obj :     attrid.h, const1.h, const2.h, const3.h 
debug1.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
debug1.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
debug1.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
debug1.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
debug1.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
debug1.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
debug1.obj :     attr25.h, attr26.h, doc.h, doc1.h 
debug1.obj :     attr0.h, rnames.h, str.h, tags.h 
debug1.obj :     seq.h, seq1.h, str1.h, io.h 
debug1.obj :     io1.h, debug.h, debug1.h, debug2.h 
debug1.obj :     parms.h

const3.obj :  const3.c, acd.h, attr.h, [-.site.vms]lcsite.h 
const3.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
const3.obj :     aunion.h, apityp.h, const.h, attrid.h 
const3.obj :     const1.h, const2.h, const3.h, attr1.h 
const3.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
const3.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
const3.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
const3.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
const3.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
const3.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
const3.obj :     attr26.h, doc.h, doc1.h, mm.h 
const3.obj :     [-.site.vms]local.h, mm1.h

const2.obj :  const2.c, acd.h, attr.h, [-.site.vms]lcsite.h 
const2.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
const2.obj :     aunion.h, apityp.h, const.h, attrid.h 
const2.obj :     const1.h, const2.h, const3.h, attr1.h 
const2.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
const2.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
const2.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
const2.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
const2.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
const2.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
const2.obj :     attr26.h, doc.h, doc1.h, mm.h 
const2.obj :     [-.site.vms]local.h, mm1.h, values.h

init1.obj :  init1.c, init.h, [-.site.vms]lcsite.h, apityp.h 
init1.obj :     const.h, errors.h, apirt.h, attrid.h 
init1.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
init1.obj :     const3.h, tags.h, acd.h, attr.h 
init1.obj :     aunion.h, attr1.h, attr2.h, attr3.h 
init1.obj :     attr4.h, attr5.h, attr6.h, attr7.h 
init1.obj :     attr8.h, attr9.h, attr10.h, attr11.h 
init1.obj :     attr12.h, attr13.h, attr15.h, attr14.h 
init1.obj :     attr16.h, attr17.h, attr18.h, attr19.h 
init1.obj :     attr20.h, attr21.h, attr22.h, attr23.h 
init1.obj :     attr24.h, attr25.h, attr26.h, doc.h 
init1.obj :     doc1.h, str.h, seq.h, seq1.h 
init1.obj :     str1.h, init1.h, mm.h, [-.site.vms]local.h 
init1.obj :     mm1.h, debug.h, debug1.h, debug2.h 
init1.obj :     label.h, label1.h, atts.h, atts1.h 
init1.obj :     atts2.h, atts3.h, atts4.h

font1.obj :  font1.c, font.h, [-.site.vms]lcsite.h, errors.h 
font1.obj :     apirt.h, apityp.h, const.h, attrid.h 
font1.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
font1.obj :     const3.h, font1.h, seq.h, tags.h 
font1.obj :     seq1.h, mm.h, [-.site.vms]local.h, mm1.h

expr1.obj :  expr1.c, acd.h, attr.h, [-.site.vms]lcsite.h 
expr1.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
expr1.obj :     aunion.h, apityp.h, const.h, attrid.h 
expr1.obj :     const1.h, const2.h, const3.h, attr1.h 
expr1.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
expr1.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
expr1.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
expr1.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
expr1.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
expr1.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
expr1.obj :     attr26.h, doc.h, doc1.h, str.h 
expr1.obj :     tags.h, seq.h, seq1.h, str1.h 
expr1.obj :     expr.h, expr1.h

const1.obj :  const1.c, acd.h, attr.h, [-.site.vms]lcsite.h 
const1.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
const1.obj :     aunion.h, apityp.h, const.h, attrid.h 
const1.obj :     const1.h, const2.h, const3.h, attr1.h 
const1.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
const1.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
const1.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
const1.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
const1.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
const1.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
const1.obj :     attr26.h, doc.h, doc1.h, mm.h 
const1.obj :     [-.site.vms]local.h, mm1.h, attr0.h, rnames.h 
const1.obj :     values.h

fmt1.obj :   fmt1.c, errors.h, tkerr.h, [-.site.vms]lcsite.h 
fmt1.obj :     tkerr1.h, fmt.h, apirt.h, fmt1.h

atts3.obj :  atts3.c, apirt.h, atts.h, [-.site.vms]lcsite.h 
atts3.obj :     errors.h, apityp.h, const.h, attrid.h 
atts3.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
atts3.obj :     const3.h, atts1.h, atts2.h, acd.h 
atts3.obj :     attr.h, aunion.h, attr1.h, attr2.h 
atts3.obj :     attr3.h, attr4.h, attr5.h, attr6.h 
atts3.obj :     attr7.h, attr8.h, attr9.h, attr10.h 
atts3.obj :     attr11.h, attr12.h, attr13.h, attr15.h 
atts3.obj :     attr14.h, attr16.h, attr17.h, attr18.h 
atts3.obj :     attr19.h, attr20.h, attr21.h, attr22.h 
atts3.obj :     attr23.h, attr24.h, attr25.h, attr26.h 
atts3.obj :     doc.h, doc1.h, atts3.h, atts4.h 
atts3.obj :     tags.h, mm.h, [-.site.vms]local.h, mm1.h 
atts3.obj :     seq.h, seq1.h

attr7.obj :  attr7.c, acd.h, attr.h, [-.site.vms]lcsite.h 
attr7.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
attr7.obj :     aunion.h, apityp.h, const.h, attrid.h 
attr7.obj :     const1.h, const2.h, const3.h, attr1.h 
attr7.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
attr7.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
attr7.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
attr7.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
attr7.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
attr7.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
attr7.obj :     attr26.h, doc.h, doc1.h, rnames.h 
attr7.obj :     attr0.h

attr6.obj :  attr6.c, parms.h, acd.h, attr.h 
attr6.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr6.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr6.obj :     attrid.h, const1.h, const2.h, const3.h 
attr6.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr6.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr6.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr6.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr6.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr6.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr6.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr6.obj :     rnames.h, attr0.h

attr5.obj :  attr5.c, rnames.h, apityp.h, [-.site.vms]lcsite.h 
attr5.obj :     const.h, errors.h, apirt.h, attrid.h 
attr5.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
attr5.obj :     const3.h, acd.h, attr.h, aunion.h 
attr5.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr5.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr5.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr5.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr5.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr5.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr5.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr5.obj :     seq.h, tags.h, seq1.h, mm.h 
attr5.obj :     [-.site.vms]local.h, mm1.h, parms.h

attr4.obj :  attr4.c, rnames.h, acd.h, attr.h 
attr4.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr4.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr4.obj :     attrid.h, const1.h, const2.h, const3.h 
attr4.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr4.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr4.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr4.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr4.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr4.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr4.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr4.obj :     seq.h, tags.h, seq1.h, mm.h 
attr4.obj :     [-.site.vms]local.h, mm1.h, parms.h, attr0.h

attr3.obj :  attr3.c, rnames.h, acd.h, attr.h 
attr3.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr3.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr3.obj :     attrid.h, const1.h, const2.h, const3.h 
attr3.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr3.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr3.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr3.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr3.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr3.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr3.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr3.obj :     seq.h, tags.h, seq1.h, mm.h 
attr3.obj :     [-.site.vms]local.h, mm1.h, parms.h, values.h 
attr3.obj :     str.h, str1.h, init.h, init1.h 
attr3.obj :     attr0.h

atts2.obj :  atts2.c, atts.h, [-.site.vms]lcsite.h, errors.h 
atts2.obj :     apirt.h, apityp.h, const.h, attrid.h 
atts2.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
atts2.obj :     const3.h, atts1.h, atts2.h, acd.h 
atts2.obj :     attr.h, aunion.h, attr1.h, attr2.h 
atts2.obj :     attr3.h, attr4.h, attr5.h, attr6.h 
atts2.obj :     attr7.h, attr8.h, attr9.h, attr10.h 
atts2.obj :     attr11.h, attr12.h, attr13.h, attr15.h 
atts2.obj :     attr14.h, attr16.h, attr17.h, attr18.h 
atts2.obj :     attr19.h, attr20.h, attr21.h, attr22.h 
atts2.obj :     attr23.h, attr24.h, attr25.h, attr26.h 
atts2.obj :     doc.h, doc1.h, atts3.h, atts4.h 
atts2.obj :     parms.h

atts1.obj :  atts1.c, atts.h, [-.site.vms]lcsite.h, errors.h 
atts1.obj :     apirt.h, apityp.h, const.h, attrid.h 
atts1.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
atts1.obj :     const3.h, atts1.h, atts2.h, acd.h 
atts1.obj :     attr.h, aunion.h, attr1.h, attr2.h 
atts1.obj :     attr3.h, attr4.h, attr5.h, attr6.h 
atts1.obj :     attr7.h, attr8.h, attr9.h, attr10.h 
atts1.obj :     attr11.h, attr12.h, attr13.h, attr15.h 
atts1.obj :     attr14.h, attr16.h, attr17.h, attr18.h 
atts1.obj :     attr19.h, attr20.h, attr21.h, attr22.h 
atts1.obj :     attr23.h, attr24.h, attr25.h, attr26.h 
atts1.obj :     doc.h, doc1.h, atts3.h, atts4.h 
atts1.obj :     values.h, parms.h

attr25.obj :  attr25.c, rnames.h, acd.h, attr.h 
attr25.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr25.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr25.obj :     attrid.h, const1.h, const2.h, const3.h 
attr25.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr25.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr25.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr25.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr25.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr25.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr25.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr25.obj :     seq.h, tags.h, seq1.h, mm.h 
attr25.obj :     [-.site.vms]local.h, mm1.h, parms.h, attr0.h

attr9.obj :  attr9.c, acd.h, attr.h, [-.site.vms]lcsite.h 
attr9.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
attr9.obj :     aunion.h, apityp.h, const.h, attrid.h 
attr9.obj :     const1.h, const2.h, const3.h, attr1.h 
attr9.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
attr9.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
attr9.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
attr9.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
attr9.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
attr9.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
attr9.obj :     attr26.h, doc.h, doc1.h, seq.h 
attr9.obj :     tags.h, seq1.h

attr8.obj :  attr8.c, acd.h, attr.h, [-.site.vms]lcsite.h 
attr8.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
attr8.obj :     aunion.h, apityp.h, const.h, attrid.h 
attr8.obj :     const1.h, const2.h, const3.h, attr1.h 
attr8.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
attr8.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
attr8.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
attr8.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
attr8.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
attr8.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
attr8.obj :     attr26.h, doc.h, doc1.h, values.h 
attr8.obj :     attr0.h, rnames.h, parms.h

atts4.obj :  atts4.c, apirt.h, atts.h, [-.site.vms]lcsite.h 
atts4.obj :     errors.h, apityp.h, const.h, attrid.h 
atts4.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
atts4.obj :     const3.h, atts1.h, atts2.h, acd.h 
atts4.obj :     attr.h, aunion.h, attr1.h, attr2.h 
atts4.obj :     attr3.h, attr4.h, attr5.h, attr6.h 
atts4.obj :     attr7.h, attr8.h, attr9.h, attr10.h 
atts4.obj :     attr11.h, attr12.h, attr13.h, attr15.h 
atts4.obj :     attr14.h, attr16.h, attr17.h, attr18.h 
atts4.obj :     attr19.h, attr20.h, attr21.h, attr22.h 
atts4.obj :     attr23.h, attr24.h, attr25.h, attr26.h 
atts4.obj :     doc.h, doc1.h, atts3.h, atts4.h 
atts4.obj :     mm.h, [-.site.vms]local.h, mm1.h, parms.h 
atts4.obj :     tags.h, seq.h, seq1.h, str.h 
atts4.obj :     str1.h

attr24.obj :  attr24.c, rnames.h, acd.h, attr.h 
attr24.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr24.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr24.obj :     attrid.h, const1.h, const2.h, const3.h 
attr24.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr24.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr24.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr24.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr24.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr24.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr24.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr24.obj :     seq.h, tags.h, seq1.h, str.h 
attr24.obj :     str1.h, mm.h, [-.site.vms]local.h, mm1.h 
attr24.obj :     attr0.h, parms.h

attr23.obj :  attr23.c, rnames.h, acd.h, attr.h 
attr23.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr23.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr23.obj :     attrid.h, const1.h, const2.h, const3.h 
attr23.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr23.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr23.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr23.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr23.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr23.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr23.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr23.obj :     mm.h, [-.site.vms]local.h, mm1.h, seq.h 
attr23.obj :     tags.h, seq1.h, attr0.h, values.h

attr26.obj :  attr26.c, rnames.h, acd.h, attr.h 
attr26.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr26.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr26.obj :     attrid.h, const1.h, const2.h, const3.h 
attr26.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr26.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr26.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr26.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr26.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr26.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr26.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr26.obj :     seq.h, tags.h, seq1.h, mm.h 
attr26.obj :     [-.site.vms]local.h, mm1.h, parms.h, attr0.h

attr21.obj :  attr21.c, rnames.h, acd.h, attr.h 
attr21.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr21.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr21.obj :     attrid.h, const1.h, const2.h, const3.h 
attr21.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr21.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr21.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr21.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr21.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr21.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr21.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr21.obj :     seq.h, tags.h, seq1.h, str.h 
attr21.obj :     str1.h, mm.h, [-.site.vms]local.h, mm1.h 
attr21.obj :     parms.h, attr0.h

attr20.obj :  attr20.c, rnames.h, acd.h, attr.h 
attr20.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr20.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr20.obj :     attrid.h, const1.h, const2.h, const3.h 
attr20.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr20.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr20.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr20.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr20.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr20.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr20.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr20.obj :     seq.h, tags.h, seq1.h, mm.h 
attr20.obj :     [-.site.vms]local.h, mm1.h, values.h, init.h 
attr20.obj :     str.h, str1.h, init1.h, attr0.h 
attr20.obj :     parms.h

attr2.obj :  attr2.c, acd.h, attr.h, [-.site.vms]lcsite.h 
attr2.obj :     errors.h, apirt.h, tkerr.h, tkerr1.h 
attr2.obj :     aunion.h, apityp.h, const.h, attrid.h 
attr2.obj :     const1.h, const2.h, const3.h, attr1.h 
attr2.obj :     attr2.h, attr3.h, attr4.h, attr5.h 
attr2.obj :     attr6.h, attr7.h, attr8.h, attr9.h 
attr2.obj :     attr10.h, attr11.h, attr12.h, attr13.h 
attr2.obj :     attr15.h, attr14.h, attr16.h, attr17.h 
attr2.obj :     attr18.h, attr19.h, attr20.h, attr21.h 
attr2.obj :     attr22.h, attr23.h, attr24.h, attr25.h 
attr2.obj :     attr26.h, doc.h, doc1.h, attr0.h 
attr2.obj :     rnames.h, str.h, tags.h, seq.h 
attr2.obj :     seq1.h, str1.h, mm.h, [-.site.vms]local.h 
attr2.obj :     mm1.h, parms.h

attr19.obj :  attr19.c, rnames.h, acd.h, attr.h 
attr19.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr19.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr19.obj :     attrid.h, const1.h, const2.h, const3.h 
attr19.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr19.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr19.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr19.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr19.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr19.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr19.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr19.obj :     init.h, tags.h, str.h, seq.h 
attr19.obj :     seq1.h, str1.h, init1.h, values.h

attr18.obj :  attr18.c, parms.h, acd.h, attr.h 
attr18.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr18.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr18.obj :     attrid.h, const1.h, const2.h, const3.h 
attr18.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr18.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr18.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr18.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr18.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr18.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr18.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr18.obj :     str.h, tags.h, seq.h, seq1.h 
attr18.obj :     str1.h, mm.h, [-.site.vms]local.h, mm1.h 
attr18.obj :     attr0.h, rnames.h

attr15.obj :  attr15.c, rnames.h, acd.h, attr.h 
attr15.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr15.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr15.obj :     attrid.h, const1.h, const2.h, const3.h 
attr15.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr15.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr15.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr15.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr15.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr15.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr15.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr15.obj :     seq.h, tags.h, seq1.h, font.h 
attr15.obj :     font1.h

attr17.obj :  attr17.c, parms.h, acd.h, attr.h 
attr17.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr17.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr17.obj :     attrid.h, const1.h, const2.h, const3.h 
attr17.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr17.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr17.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr17.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr17.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr17.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr17.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr17.obj :     str.h, tags.h, seq.h, seq1.h 
attr17.obj :     str1.h, mm.h, [-.site.vms]local.h, mm1.h 
attr17.obj :     attr0.h, rnames.h

attr13.obj :  attr13.c, rnames.h, acd.h, attr.h 
attr13.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr13.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr13.obj :     attrid.h, const1.h, const2.h, const3.h 
attr13.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr13.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr13.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr13.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr13.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr13.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr13.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr13.obj :     mm.h, [-.site.vms]local.h, mm1.h, seq.h 
attr13.obj :     tags.h, seq1.h

attr14.obj :  attr14.c, rnames.h, acd.h, attr.h 
attr14.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr14.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr14.obj :     attrid.h, const1.h, const2.h, const3.h 
attr14.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr14.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr14.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr14.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr14.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr14.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr14.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr14.obj :     seq.h, tags.h, seq1.h

attr12.obj :  attr12.c, rnames.h, acd.h, attr.h 
attr12.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr12.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr12.obj :     attrid.h, const1.h, const2.h, const3.h 
attr12.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr12.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr12.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr12.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr12.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr12.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr12.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr12.obj :     mm.h, [-.site.vms]local.h, mm1.h, parms.h 
attr12.obj :     tags.h, attr0.h, values.h

attr11.obj :  attr11.c, rnames.h, acd.h, attr.h 
attr11.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr11.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr11.obj :     attrid.h, const1.h, const2.h, const3.h 
attr11.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr11.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr11.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr11.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr11.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr11.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr11.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr11.obj :     seq.h, tags.h, seq1.h, mm.h 
attr11.obj :     [-.site.vms]local.h, mm1.h, str.h, str1.h 
attr11.obj :     parms.h, values.h, attr0.h

attr10.obj :  attr10.c, rnames.h, acd.h, attr.h 
attr10.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr10.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr10.obj :     attrid.h, const1.h, const2.h, const3.h 
attr10.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr10.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr10.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr10.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr10.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr10.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr10.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr10.obj :     seq.h, tags.h, seq1.h, mm.h 
attr10.obj :     [-.site.vms]local.h, mm1.h, values.h, str.h 
attr10.obj :     str1.h, attr0.h

attr1.obj :  attr1.c, parms.h, acd.h, attr.h 
attr1.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr1.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr1.obj :     attrid.h, const1.h, const2.h, const3.h 
attr1.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr1.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr1.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr1.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr1.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr1.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr1.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr1.obj :     str.h, tags.h, seq.h, seq1.h 
attr1.obj :     str1.h, mm.h, [-.site.vms]local.h, mm1.h 
attr1.obj :     attr0.h, rnames.h

rasn14.obj :  rasn14.c, font.h, [-.site.vms]lcsite.h, errors.h 
rasn14.obj :     apirt.h, apityp.h, const.h, attrid.h 
rasn14.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
rasn14.obj :     const3.h, font1.h, rasn1.h, values.h 
rasn14.obj :     io.h, io1.h, acd.h, attr.h 
rasn14.obj :     aunion.h, attr1.h, attr2.h, attr3.h 
rasn14.obj :     attr4.h, attr5.h, attr6.h, attr7.h 
rasn14.obj :     attr8.h, attr9.h, attr10.h, attr11.h 
rasn14.obj :     attr12.h, attr13.h, attr15.h, attr14.h 
rasn14.obj :     attr16.h, attr17.h, attr18.h, attr19.h 
rasn14.obj :     attr20.h, attr21.h, attr22.h, attr23.h 
rasn14.obj :     attr24.h, attr25.h, attr26.h, doc.h 
rasn14.obj :     doc1.h, tags.h, parms.h, seq.h 
rasn14.obj :     seq1.h, asn1.h, rasn11.h, rasn12.h 
rasn14.obj :     rasn13.h, rasn14.h, rodif.h, mm.h 
rasn14.obj :     [-.site.vms]local.h, mm1.h, str.h, str1.h 
rasn14.obj :     rodif1.h, rodif2.h, rodif3.h, rodif4.h

attr22.obj :  attr22.c, rnames.h, acd.h, attr.h 
attr22.obj :     [-.site.vms]lcsite.h, errors.h, apirt.h, tkerr.h 
attr22.obj :     tkerr1.h, aunion.h, apityp.h, const.h 
attr22.obj :     attrid.h, const1.h, const2.h, const3.h 
attr22.obj :     attr1.h, attr2.h, attr3.h, attr4.h 
attr22.obj :     attr5.h, attr6.h, attr7.h, attr8.h 
attr22.obj :     attr9.h, attr10.h, attr11.h, attr12.h 
attr22.obj :     attr13.h, attr15.h, attr14.h, attr16.h 
attr22.obj :     attr17.h, attr18.h, attr19.h, attr20.h 
attr22.obj :     attr21.h, attr22.h, attr23.h, attr24.h 
attr22.obj :     attr25.h, attr26.h, doc.h, doc1.h 
attr22.obj :     str.h, tags.h, seq.h, seq1.h 
attr22.obj :     str1.h, mm.h, [-.site.vms]local.h, mm1.h 
attr22.obj :     parms.h, values.h, attr0.h

tkerr1.obj :  tkerr1.c, [-.site.vms]lcsite.h, init.h, apityp.h 
tkerr1.obj :     const.h, errors.h, apirt.h, attrid.h 
tkerr1.obj :     tkerr.h, tkerr1.h, const1.h, const2.h 
tkerr1.obj :     const3.h, tags.h, acd.h, attr.h 
tkerr1.obj :     aunion.h, attr1.h, attr2.h, attr3.h 
tkerr1.obj :     attr4.h, attr5.h, attr6.h, attr7.h 
tkerr1.obj :     attr8.h, attr9.h, attr10.h, attr11.h 
tkerr1.obj :     attr12.h, attr13.h, attr15.h, attr14.h 
tkerr1.obj :     attr16.h, attr17.h, attr18.h, attr19.h 
tkerr1.obj :     attr20.h, attr21.h, attr22.h, attr23.h 
tkerr1.obj :     attr24.h, attr25.h, attr26.h, doc.h 
tkerr1.obj :     doc1.h, str.h, seq.h, seq1.h 
tkerr1.obj :     str1.h, init1.h, io.h, io1.h 
tkerr1.obj :     fmt.h, fmt1.h

liboda.olb :    wasn14.obj, wodif3.obj, wodif2.obj, attr16.obj, wodif4.obj, wasn13.obj 
liboda.olb :    wasn12.obj, mem1.obj, wasn15.obj, wasn11.obj, wodif1.obj, seq1.obj 
liboda.olb :    rodif4.obj, rodif3.obj, stubs2.obj, stubs.obj, str1.obj, rodif2.obj 
liboda.olb :    rodif1.obj, rasn13.obj, rasn12.obj, rasn11.obj, label1.obj, iter4.obj 
liboda.olb :    iter3.obj, iter2.obj, prior1.obj, io1.obj, mm1.obj, doc1.obj 
liboda.olb :    debug2.obj, iter1.obj, debug1.obj, const3.obj, const2.obj, init1.obj 
liboda.olb :    font1.obj, expr1.obj, const1.obj, fmt1.obj, atts3.obj, attr7.obj 
liboda.olb :    attr6.obj, attr5.obj, attr4.obj, attr3.obj, atts2.obj, atts1.obj 
liboda.olb :    attr25.obj, attr9.obj, attr8.obj, atts4.obj, attr24.obj, attr23.obj 
liboda.olb :    attr26.obj, attr21.obj, attr20.obj, attr2.obj, attr19.obj, attr18.obj 
liboda.olb :    attr15.obj, attr17.obj, attr13.obj, attr14.obj, attr12.obj, attr11.obj 
liboda.olb :    attr10.obj, attr1.obj, rasn14.obj, attr22.obj, tkerr1.obj
	   $(AR1) liboda.olb wasn14.obj, wodif3.obj, wodif2.obj, attr16.obj, wodif4.obj 
	   $(AR2) liboda.olb wasn13.obj, wasn12.obj, mem1.obj, wasn15.obj, wasn11.obj 
	   $(AR2) liboda.olb wodif1.obj, seq1.obj, rodif4.obj, rodif3.obj, stubs2.obj 
	   $(AR2) liboda.olb stubs.obj, str1.obj, rodif2.obj, rodif1.obj, rasn13.obj 
	   $(AR2) liboda.olb rasn12.obj, rasn11.obj, label1.obj, iter4.obj, iter3.obj 
	   $(AR2) liboda.olb iter2.obj, prior1.obj, io1.obj, mm1.obj, doc1.obj 
	   $(AR2) liboda.olb debug2.obj, iter1.obj, debug1.obj, const3.obj, const2.obj 
	   $(AR2) liboda.olb init1.obj, font1.obj, expr1.obj, const1.obj, fmt1.obj 
	   $(AR2) liboda.olb atts3.obj, attr7.obj, attr6.obj, attr5.obj, attr4.obj 
	   $(AR2) liboda.olb attr3.obj, atts2.obj, atts1.obj, attr25.obj, attr9.obj 
	   $(AR2) liboda.olb attr8.obj, atts4.obj, attr24.obj, attr23.obj, attr26.obj 
	   $(AR2) liboda.olb attr21.obj, attr20.obj, attr2.obj, attr19.obj, attr18.obj 
	   $(AR2) liboda.olb attr15.obj, attr17.obj, attr13.obj, attr14.obj, attr12.obj 
	   $(AR2) liboda.olb attr11.obj, attr10.obj, attr1.obj, rasn14.obj, attr22.obj 
	   $(AR2) liboda.olb tkerr1.obj
	   purge liboda.olb
