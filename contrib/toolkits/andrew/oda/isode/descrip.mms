
!
!		(c) Copyright Carnegie Mellon University
!
! See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
! CMU-ITC-071) for full text & permitted uses.
!

!
! Makefile for site type 'vms', directory 'isode'
! Generated on Thu Nov  2 10:21:01 1989
!

! Std definitions
CC = cc
INCLUDES = /include_directory=([],[-.valid],[-.prog],[-.raster],[-.src],[-.site.vms])
CFLAGS = $(INCLUDES)/standard=portable
LDFLAGS = 
AR1 = library/create
AR2 = library/insert

! Default compilation rule
.c.obj :
	   $(CC) $(CFLAGS) $*.c
	   purge $*.obj



! Rules for LIB "libisode.olb"

vprint.obj :  vprint.c, psap.h, manifest.h, config.h 
vprint.obj :     [-.site.vms]lcsite.h, general.h, logger.h

tailor.obj :  tailor.c, general.h, config.h, [-.site.vms]lcsite.h 
tailor.obj :     manifest.h, isoaddrs.h, tailor.h, logger.h

str2vec.obj :  str2vec.c, general.h, config.h, [-.site.vms]lcsite.h 
str2vec.obj :     manifest.h

str2prim.obj :  str2prim.c, psap.h, manifest.h, config.h 
str2prim.obj :     [-.site.vms]lcsite.h, general.h

str2oid.obj :  str2oid.c, psap.h, manifest.h, config.h 
str2oid.obj :     [-.site.vms]lcsite.h, general.h

str2elem.obj :  str2elem.c, general.h, config.h, [-.site.vms]lcsite.h 
str2elem.obj :     manifest.h

std2ps.obj :  std2ps.c, psap.h, manifest.h, config.h 
std2ps.obj :     [-.site.vms]lcsite.h, general.h

sprintoid.obj :  sprintoid.c, psap.h, manifest.h, config.h 
sprintoid.obj :     [-.site.vms]lcsite.h, general.h

set_find.obj :  set_find.c, psap.h, manifest.h, config.h 
set_find.obj :     [-.site.vms]lcsite.h, general.h

set_add.obj :  set_add.c, psap.h, manifest.h, config.h 
set_add.obj :     [-.site.vms]lcsite.h, general.h

serror.obj :  serror.c, general.h, config.h, [-.site.vms]lcsite.h 
serror.obj :     manifest.h

seq_find.obj :  seq_find.c, psap.h, manifest.h, config.h 
seq_find.obj :     [-.site.vms]lcsite.h, general.h

seq_del.obj :  seq_del.c, psap.h, manifest.h, config.h 
seq_del.obj :     [-.site.vms]lcsite.h, general.h

seq_addon.obj :  seq_addon.c, psap.h, manifest.h, config.h 
seq_addon.obj :     [-.site.vms]lcsite.h, general.h

seq_add.obj :  seq_add.c, psap.h, manifest.h, config.h 
seq_add.obj :     [-.site.vms]lcsite.h, general.h

py_advise.obj :  py_advise.c

ps_prime.obj :  ps_prime.c, psap.h, manifest.h, config.h 
ps_prime.obj :     [-.site.vms]lcsite.h, general.h

ps_io.obj :  ps_io.c, psap.h, manifest.h, config.h 
ps_io.obj :     [-.site.vms]lcsite.h, general.h

ps_get_abs.obj :  ps_get_abs.c, psap.h, manifest.h, config.h 
ps_get_abs.obj :     [-.site.vms]lcsite.h, general.h

ps_free.obj :  ps_free.c, psap.h, manifest.h, config.h 
ps_free.obj :     [-.site.vms]lcsite.h, general.h

ps_flush.obj :  ps_flush.c, psap.h, manifest.h, config.h 
ps_flush.obj :     [-.site.vms]lcsite.h, general.h

ps_alloc.obj :  ps_alloc.c, psap.h, manifest.h, config.h 
ps_alloc.obj :     [-.site.vms]lcsite.h, general.h

ps2pe.obj :  ps2pe.c, psap.h, manifest.h, config.h 
ps2pe.obj :     [-.site.vms]lcsite.h, general.h, tailor.h, logger.h

prim2str.obj :  prim2str.c, psap.h, manifest.h, config.h 
prim2str.obj :     [-.site.vms]lcsite.h, general.h

prim2set.obj :  prim2set.c, psap.h, manifest.h, config.h 
prim2set.obj :     [-.site.vms]lcsite.h, general.h

prim2oid.obj :  prim2oid.c, psap.h, manifest.h, config.h 
prim2oid.obj :     [-.site.vms]lcsite.h, general.h

prim2num.obj :  prim2num.c, psap.h, manifest.h, config.h 
prim2num.obj :     [-.site.vms]lcsite.h, general.h

prim2flag.obj :  prim2flag.c, psap.h, manifest.h, config.h 
prim2flag.obj :     [-.site.vms]lcsite.h, general.h

prim2bit.obj :  prim2bit.c, psap.h, manifest.h, config.h 
prim2bit.obj :     [-.site.vms]lcsite.h, general.h

pl_tables.obj :  pl_tables.c, psap.h, manifest.h, config.h 
pl_tables.obj :     [-.site.vms]lcsite.h, general.h

pe_free.obj :  pe_free.c, psap.h, manifest.h, config.h 
pe_free.obj :     [-.site.vms]lcsite.h, general.h

pe_error.obj :  pe_error.c, psap.h, manifest.h, config.h 
pe_error.obj :     [-.site.vms]lcsite.h, general.h

pe_alloc.obj :  pe_alloc.c, psap.h, manifest.h, config.h 
pe_alloc.obj :     [-.site.vms]lcsite.h, general.h

pe2ps.obj :  pe2ps.c, psap.h, manifest.h, config.h 
pe2ps.obj :     [-.site.vms]lcsite.h, general.h, tailor.h, logger.h

oid_free.obj :  oid_free.c, psap.h, manifest.h, config.h 
oid_free.obj :     [-.site.vms]lcsite.h, general.h

oid_cpy.obj :  oid_cpy.c, psap.h, manifest.h, config.h 
oid_cpy.obj :     [-.site.vms]lcsite.h, general.h

oid_cmp.obj :  oid_cmp.c, psap.h, manifest.h, config.h 
oid_cmp.obj :     [-.site.vms]lcsite.h, general.h

oid2ode.obj :  oid2ode.c, psap.h, manifest.h, config.h 
oid2ode.obj :     [-.site.vms]lcsite.h, general.h, tailor.h, logger.h

objectbyoid.obj :  objectbyoid.c, psap.h, manifest.h, config.h 
objectbyoid.obj :     [-.site.vms]lcsite.h, general.h, tailor.h, logger.h

obj2prim.obj :  obj2prim.c, psap.h, manifest.h, config.h 
obj2prim.obj :     [-.site.vms]lcsite.h, general.h

num2prim.obj :  num2prim.c, psap.h, manifest.h, config.h 
num2prim.obj :     [-.site.vms]lcsite.h, general.h

logger.obj :  logger.c, general.h, config.h, [-.site.vms]lcsite.h 
logger.obj :     manifest.h, logger.h, tailor.h

iswr9.obj :  iswr9.c, psap.h, manifest.h, config.h 
iswr9.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr9.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr9.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr9.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr9.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr9.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr9.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr9.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr9.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr9.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr9.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr9.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr9.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr9.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr9.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr9.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr9.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr9.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr9.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr9.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr9.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr9.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr9.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr9.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr9.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr9.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr9.obj :     iswr19.h, iswr20.h

iswr8.obj :  iswr8.c, psap.h, manifest.h, config.h 
iswr8.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr8.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr8.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr8.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr8.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr8.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr8.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr8.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr8.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr8.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr8.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr8.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr8.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr8.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr8.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr8.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr8.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr8.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr8.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr8.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr8.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr8.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr8.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr8.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr8.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr8.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr8.obj :     iswr19.h, iswr20.h

iswr7.obj :  iswr7.c, psap.h, manifest.h, config.h 
iswr7.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr7.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr7.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr7.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr7.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr7.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr7.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr7.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr7.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr7.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr7.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr7.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr7.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr7.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr7.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr7.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr7.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr7.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr7.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr7.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr7.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr7.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr7.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr7.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr7.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr7.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr7.obj :     iswr19.h, iswr20.h

iswr6.obj :  iswr6.c, psap.h, manifest.h, config.h 
iswr6.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr6.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr6.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr6.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr6.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr6.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr6.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr6.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr6.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr6.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr6.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr6.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr6.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr6.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr6.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr6.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr6.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr6.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr6.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr6.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr6.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr6.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr6.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr6.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr6.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr6.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr6.obj :     iswr19.h, iswr20.h

iswr5.obj :  iswr5.c, psap.h, manifest.h, config.h 
iswr5.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr5.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr5.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr5.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr5.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr5.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr5.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr5.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr5.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr5.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr5.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr5.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr5.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr5.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr5.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr5.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr5.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr5.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr5.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr5.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr5.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr5.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr5.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr5.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr5.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr5.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr5.obj :     iswr19.h, iswr20.h

iswr4.obj :  iswr4.c, psap.h, manifest.h, config.h 
iswr4.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr4.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr4.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr4.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr4.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr4.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr4.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr4.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr4.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr4.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr4.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr4.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr4.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr4.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr4.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr4.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr4.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr4.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr4.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr4.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr4.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr4.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr4.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr4.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr4.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr4.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr4.obj :     iswr19.h, iswr20.h

iswr3.obj :  iswr3.c, psap.h, manifest.h, config.h 
iswr3.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr3.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr3.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr3.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr3.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr3.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr3.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr3.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr3.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr3.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr3.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr3.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr3.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr3.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr3.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr3.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr3.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr3.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr3.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr3.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr3.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr3.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr3.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr3.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr3.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr3.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr3.obj :     iswr19.h, iswr20.h

iswr20.obj :  iswr20.c, psap.h, manifest.h, config.h 
iswr20.obj :     [-.site.vms]lcsite.h, general.h, [-.src]apityp.h, [-.src]const.h 
iswr20.obj :     [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h, [-.src]tkerr.h 
iswr20.obj :     [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h, [-.src]const3.h 
iswr20.obj :     iswr.h, iswr19.h, iswr20.h, [-.src]str.h 
iswr20.obj :     [-.src]tags.h, [-.src]seq.h, [-.src]seq1.h, [-.src]str1.h 
iswr20.obj :     [-.src]mm.h, [-.site.vms]local.h, [-.src]mm1.h

iswr2.obj :  iswr2.c, psap.h, manifest.h, config.h 
iswr2.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr2.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr2.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr2.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr2.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr2.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr2.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr2.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr2.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr2.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr2.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr2.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr2.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr2.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr2.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr2.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr2.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr2.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr2.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr2.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr2.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr2.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr2.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr2.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr2.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr2.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr2.obj :     iswr19.h, iswr20.h

iswr19.obj :  iswr19.c, psap.h, manifest.h, config.h 
iswr19.obj :     [-.site.vms]lcsite.h, general.h, [-.src]values.h, [-.src]apityp.h 
iswr19.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr19.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr19.obj :     [-.src]const3.h, [-.src]init.h, [-.src]tags.h, [-.src]acd.h 
iswr19.obj :     [-.src]attr.h, [-.src]aunion.h, [-.src]attr1.h, [-.src]attr2.h 
iswr19.obj :     [-.src]attr3.h, [-.src]attr4.h, [-.src]attr5.h, [-.src]attr6.h 
iswr19.obj :     [-.src]attr7.h, [-.src]attr8.h, [-.src]attr9.h, [-.src]attr10.h 
iswr19.obj :     [-.src]attr11.h, [-.src]attr12.h, [-.src]attr13.h, [-.src]attr15.h 
iswr19.obj :     [-.src]attr14.h, [-.src]attr16.h, [-.src]attr17.h, [-.src]attr18.h 
iswr19.obj :     [-.src]attr19.h, [-.src]attr20.h, [-.src]attr21.h, [-.src]attr22.h 
iswr19.obj :     [-.src]attr23.h, [-.src]attr24.h, [-.src]attr25.h, [-.src]attr26.h 
iswr19.obj :     [-.src]doc.h, [-.src]doc1.h, [-.src]str.h, [-.src]seq.h 
iswr19.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr19.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr19.obj :     iswr.h, iswr19.h, iswr20.h

iswr18.obj :  iswr18.c, psap.h, manifest.h, config.h 
iswr18.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr18.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr18.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr18.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr18.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr18.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr18.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr18.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr18.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr18.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr18.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr18.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr18.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr18.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr18.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr18.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr18.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr18.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr18.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr18.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr18.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr18.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr18.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr18.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr18.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr18.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
iswr18.obj :     isrd19.h, isrd20.h, ishsh.h

iswr17.obj :  iswr17.c, psap.h, manifest.h, config.h 
iswr17.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr17.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr17.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr17.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr17.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr17.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr17.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr17.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr17.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr17.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr17.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr17.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr17.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr17.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr17.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr17.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr17.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr17.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr17.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr17.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr17.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr17.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr17.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr17.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr17.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr17.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr17.obj :     iswr19.h, iswr20.h

iswr16.obj :  iswr16.c, psap.h, manifest.h, config.h 
iswr16.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr16.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr16.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr16.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr16.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr16.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr16.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr16.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr16.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr16.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr16.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr16.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr16.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr16.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr16.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr16.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr16.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr16.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr16.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr16.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr16.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr16.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr16.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr16.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr16.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr16.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr16.obj :     iswr19.h, iswr20.h

iswr15.obj :  iswr15.c, psap.h, manifest.h, config.h 
iswr15.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr15.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr15.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr15.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr15.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr15.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr15.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr15.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr15.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr15.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr15.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr15.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr15.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr15.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr15.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr15.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr15.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr15.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr15.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr15.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr15.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr15.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr15.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr15.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr15.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr15.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr15.obj :     iswr19.h, iswr20.h

iswr14.obj :  iswr14.c, psap.h, manifest.h, config.h 
iswr14.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr14.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr14.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr14.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr14.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr14.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr14.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr14.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr14.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr14.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr14.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr14.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr14.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr14.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr14.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr14.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr14.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr14.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr14.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr14.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr14.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr14.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr14.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr14.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr14.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr14.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr14.obj :     iswr19.h, iswr20.h

iswr13.obj :  iswr13.c, psap.h, manifest.h, config.h 
iswr13.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr13.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr13.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr13.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr13.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr13.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr13.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr13.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr13.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr13.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr13.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr13.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr13.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr13.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr13.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr13.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr13.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr13.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr13.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr13.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr13.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr13.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr13.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr13.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr13.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr13.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr13.obj :     iswr19.h, iswr20.h

iswr12.obj :  iswr12.c, psap.h, manifest.h, config.h 
iswr12.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr12.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr12.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr12.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr12.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr12.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr12.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr12.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr12.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr12.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr12.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr12.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr12.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr12.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr12.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr12.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr12.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr12.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr12.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr12.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr12.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr12.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr12.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr12.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr12.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr12.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr12.obj :     iswr19.h, iswr20.h

iswr11.obj :  iswr11.c, psap.h, manifest.h, config.h 
iswr11.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr11.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr11.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr11.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr11.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr11.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr11.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr11.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr11.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr11.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr11.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr11.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr11.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr11.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr11.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr11.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr11.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr11.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr11.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr11.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr11.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr11.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr11.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr11.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr11.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr11.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr11.obj :     iswr19.h, iswr20.h

iswr10.obj :  iswr10.c, psap.h, manifest.h, config.h 
iswr10.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr10.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr10.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr10.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr10.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr10.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr10.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr10.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr10.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr10.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr10.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr10.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr10.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr10.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr10.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr10.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr10.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr10.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr10.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr10.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr10.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr10.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr10.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr10.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr10.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr10.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr10.obj :     iswr19.h, iswr20.h

iswr1.obj :  iswr1.c, psap.h, manifest.h, config.h 
iswr1.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
iswr1.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
iswr1.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
iswr1.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
iswr1.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
iswr1.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
iswr1.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
iswr1.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
iswr1.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
iswr1.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
iswr1.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
iswr1.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
iswr1.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
iswr1.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
iswr1.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
iswr1.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
iswr1.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
iswr1.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
iswr1.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
iswr1.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
iswr1.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
iswr1.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
iswr1.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
iswr1.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
iswr1.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
iswr1.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, iswr.h 
iswr1.obj :     iswr19.h, iswr20.h

isrd9.obj :  isrd9.c, psap.h, manifest.h, config.h 
isrd9.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd9.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd9.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd9.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd9.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd9.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd9.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd9.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd9.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd9.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd9.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd9.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd9.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd9.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd9.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd9.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd9.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd9.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd9.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd9.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd9.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd9.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd9.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd9.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd9.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd9.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd9.obj :     isrd19.h, isrd20.h, ishsh.h

isrd8.obj :  isrd8.c, psap.h, manifest.h, config.h 
isrd8.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd8.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd8.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd8.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd8.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd8.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd8.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd8.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd8.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd8.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd8.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd8.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd8.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd8.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd8.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd8.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd8.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd8.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd8.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd8.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd8.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd8.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd8.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd8.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd8.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd8.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd8.obj :     isrd19.h, isrd20.h, ishsh.h

isrd7.obj :  isrd7.c, psap.h, manifest.h, config.h 
isrd7.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd7.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd7.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd7.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd7.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd7.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd7.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd7.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd7.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd7.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd7.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd7.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd7.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd7.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd7.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd7.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd7.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd7.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd7.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd7.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd7.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd7.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd7.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd7.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd7.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd7.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd7.obj :     isrd19.h, isrd20.h, ishsh.h

isrd6.obj :  isrd6.c, psap.h, manifest.h, config.h 
isrd6.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd6.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd6.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd6.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd6.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd6.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd6.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd6.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd6.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd6.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd6.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd6.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd6.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd6.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd6.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd6.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd6.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd6.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd6.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd6.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd6.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd6.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd6.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd6.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd6.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd6.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd6.obj :     isrd19.h, isrd20.h, ishsh.h

isrd5.obj :  isrd5.c, psap.h, manifest.h, config.h 
isrd5.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd5.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd5.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd5.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd5.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd5.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd5.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd5.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd5.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd5.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd5.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd5.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd5.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd5.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd5.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd5.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd5.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd5.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd5.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd5.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd5.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd5.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd5.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd5.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd5.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd5.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd5.obj :     isrd19.h, isrd20.h, ishsh.h

isrd4.obj :  isrd4.c, psap.h, manifest.h, config.h 
isrd4.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd4.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd4.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd4.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd4.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd4.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd4.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd4.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd4.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd4.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd4.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd4.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd4.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd4.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd4.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd4.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd4.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd4.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd4.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd4.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd4.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd4.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd4.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd4.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd4.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd4.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd4.obj :     isrd19.h, isrd20.h, ishsh.h

isrd3.obj :  isrd3.c, psap.h, manifest.h, config.h 
isrd3.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd3.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd3.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd3.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd3.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd3.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd3.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd3.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd3.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd3.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd3.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd3.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd3.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd3.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd3.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd3.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd3.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd3.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd3.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd3.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd3.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd3.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd3.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd3.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd3.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd3.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd3.obj :     isrd19.h, isrd20.h, ishsh.h

isrd20.obj :  isrd20.c, [-.src]values.h, [-.src]apityp.h, [-.site.vms]lcsite.h 
isrd20.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd20.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd20.obj :     [-.src]const3.h, [-.src]init.h, [-.src]tags.h, [-.src]acd.h 
isrd20.obj :     [-.src]attr.h, [-.src]aunion.h, [-.src]attr1.h, [-.src]attr2.h 
isrd20.obj :     [-.src]attr3.h, [-.src]attr4.h, [-.src]attr5.h, [-.src]attr6.h 
isrd20.obj :     [-.src]attr7.h, [-.src]attr8.h, [-.src]attr9.h, [-.src]attr10.h 
isrd20.obj :     [-.src]attr11.h, [-.src]attr12.h, [-.src]attr13.h, [-.src]attr15.h 
isrd20.obj :     [-.src]attr14.h, [-.src]attr16.h, [-.src]attr17.h, [-.src]attr18.h 
isrd20.obj :     [-.src]attr19.h, [-.src]attr20.h, [-.src]attr21.h, [-.src]attr22.h 
isrd20.obj :     [-.src]attr23.h, [-.src]attr24.h, [-.src]attr25.h, [-.src]attr26.h 
isrd20.obj :     [-.src]doc.h, [-.src]doc1.h, [-.src]str.h, [-.src]seq.h 
isrd20.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd20.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd20.obj :     isrd.h, isrd19.h, isrd20.h, ishsh.h 
isrd20.obj :     [-.src]mm.h, [-.site.vms]local.h, [-.src]mm1.h, psap.h 
isrd20.obj :     manifest.h, config.h, general.h

isrd2.obj :  isrd2.c, psap.h, manifest.h, config.h 
isrd2.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd2.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd2.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd2.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd2.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd2.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd2.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd2.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd2.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd2.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd2.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd2.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd2.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd2.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd2.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd2.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd2.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd2.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd2.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd2.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd2.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd2.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd2.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd2.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd2.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd2.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd2.obj :     isrd19.h, isrd20.h, ishsh.h

isrd19.obj :  isrd19.c, [-.src]values.h, [-.src]apityp.h, [-.site.vms]lcsite.h 
isrd19.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd19.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd19.obj :     [-.src]const3.h, [-.src]init.h, [-.src]tags.h, [-.src]acd.h 
isrd19.obj :     [-.src]attr.h, [-.src]aunion.h, [-.src]attr1.h, [-.src]attr2.h 
isrd19.obj :     [-.src]attr3.h, [-.src]attr4.h, [-.src]attr5.h, [-.src]attr6.h 
isrd19.obj :     [-.src]attr7.h, [-.src]attr8.h, [-.src]attr9.h, [-.src]attr10.h 
isrd19.obj :     [-.src]attr11.h, [-.src]attr12.h, [-.src]attr13.h, [-.src]attr15.h 
isrd19.obj :     [-.src]attr14.h, [-.src]attr16.h, [-.src]attr17.h, [-.src]attr18.h 
isrd19.obj :     [-.src]attr19.h, [-.src]attr20.h, [-.src]attr21.h, [-.src]attr22.h 
isrd19.obj :     [-.src]attr23.h, [-.src]attr24.h, [-.src]attr25.h, [-.src]attr26.h 
isrd19.obj :     [-.src]doc.h, [-.src]doc1.h, [-.src]str.h, [-.src]seq.h 
isrd19.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd19.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd19.obj :     isrd.h, isrd19.h, isrd20.h, ishsh.h 
isrd19.obj :     [-.src]mm.h, [-.site.vms]local.h, [-.src]mm1.h, psap.h 
isrd19.obj :     manifest.h, config.h, general.h

isrd18.obj :  isrd18.c, psap.h, manifest.h, config.h 
isrd18.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd18.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd18.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd18.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd18.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd18.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd18.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd18.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd18.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd18.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd18.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd18.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd18.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd18.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd18.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd18.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd18.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd18.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd18.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd18.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd18.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd18.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd18.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd18.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd18.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd18.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd18.obj :     isrd19.h, isrd20.h, ishsh.h

isrd17.obj :  isrd17.c, psap.h, manifest.h, config.h 
isrd17.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd17.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd17.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd17.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd17.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd17.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd17.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd17.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd17.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd17.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd17.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd17.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd17.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd17.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd17.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd17.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd17.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd17.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd17.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd17.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd17.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd17.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd17.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd17.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd17.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd17.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd17.obj :     isrd19.h, isrd20.h, ishsh.h

isrd16.obj :  isrd16.c, psap.h, manifest.h, config.h 
isrd16.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd16.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd16.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd16.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd16.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd16.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd16.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd16.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd16.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd16.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd16.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd16.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd16.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd16.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd16.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd16.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd16.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd16.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd16.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd16.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd16.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd16.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd16.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd16.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd16.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd16.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd16.obj :     isrd19.h, isrd20.h, ishsh.h

isrd15.obj :  isrd15.c, psap.h, manifest.h, config.h 
isrd15.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd15.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd15.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd15.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd15.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd15.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd15.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd15.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd15.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd15.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd15.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd15.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd15.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd15.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd15.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd15.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd15.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd15.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd15.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd15.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd15.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd15.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd15.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd15.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd15.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd15.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd15.obj :     isrd19.h, isrd20.h, ishsh.h

isrd14.obj :  isrd14.c, psap.h, manifest.h, config.h 
isrd14.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd14.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd14.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd14.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd14.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd14.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd14.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd14.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd14.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd14.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd14.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd14.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd14.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd14.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd14.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd14.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd14.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd14.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd14.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd14.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd14.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd14.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd14.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd14.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd14.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd14.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd14.obj :     isrd19.h, isrd20.h, ishsh.h

isrd13.obj :  isrd13.c, psap.h, manifest.h, config.h 
isrd13.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd13.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd13.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd13.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd13.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd13.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd13.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd13.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd13.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd13.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd13.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd13.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd13.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd13.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd13.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd13.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd13.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd13.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd13.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd13.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd13.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd13.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd13.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd13.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd13.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd13.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd13.obj :     isrd19.h, isrd20.h, ishsh.h

isrd12.obj :  isrd12.c, psap.h, manifest.h, config.h 
isrd12.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd12.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd12.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd12.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd12.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd12.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd12.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd12.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd12.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd12.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd12.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd12.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd12.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd12.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd12.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd12.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd12.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd12.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd12.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd12.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd12.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd12.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd12.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd12.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd12.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd12.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd12.obj :     isrd19.h, isrd20.h, ishsh.h

isrd11.obj :  isrd11.c, psap.h, manifest.h, config.h 
isrd11.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd11.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd11.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd11.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd11.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd11.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd11.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd11.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd11.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd11.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd11.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd11.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd11.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd11.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd11.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd11.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd11.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd11.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd11.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd11.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd11.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd11.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd11.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd11.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd11.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd11.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd11.obj :     isrd19.h, isrd20.h, ishsh.h

isrd10.obj :  isrd10.c, psap.h, manifest.h, config.h 
isrd10.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd10.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd10.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd10.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd10.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd10.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd10.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd10.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd10.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd10.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd10.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd10.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd10.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd10.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd10.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd10.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd10.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd10.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd10.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd10.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd10.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd10.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd10.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd10.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd10.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd10.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd10.obj :     isrd19.h, isrd20.h, ishsh.h

isrd1.obj :  isrd1.c, psap.h, manifest.h, config.h 
isrd1.obj :     [-.site.vms]lcsite.h, general.h, [-.src]odatk.h, [-.src]apityp.h 
isrd1.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
isrd1.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
isrd1.obj :     [-.src]const3.h, [-.src]aunion.h, [-.src]acd.h, [-.src]attr.h 
isrd1.obj :     [-.src]attr1.h, [-.src]attr2.h, [-.src]attr3.h, [-.src]attr4.h 
isrd1.obj :     [-.src]attr5.h, [-.src]attr6.h, [-.src]attr7.h, [-.src]attr8.h 
isrd1.obj :     [-.src]attr9.h, [-.src]attr10.h, [-.src]attr11.h, [-.src]attr12.h 
isrd1.obj :     [-.src]attr13.h, [-.src]attr15.h, [-.src]attr14.h, [-.src]attr16.h 
isrd1.obj :     [-.src]attr17.h, [-.src]attr18.h, [-.src]attr19.h, [-.src]attr20.h 
isrd1.obj :     [-.src]attr21.h, [-.src]attr22.h, [-.src]attr23.h, [-.src]attr24.h 
isrd1.obj :     [-.src]attr25.h, [-.src]attr26.h, [-.src]doc.h, [-.src]doc1.h 
isrd1.obj :     [-.src]atts.h, [-.src]atts1.h, [-.src]atts2.h, [-.src]atts3.h 
isrd1.obj :     [-.src]atts4.h, [-.src]debug.h, [-.src]debug1.h, [-.src]debug2.h 
isrd1.obj :     [-.src]expr.h, [-.src]expr1.h, [-.src]font.h, [-.src]font1.h 
isrd1.obj :     [-.src]init.h, [-.src]tags.h, [-.src]str.h, [-.src]seq.h 
isrd1.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, [-.src]iter.h 
isrd1.obj :     [-.src]iter1.h, [-.src]iter2.h, [-.src]iter3.h, [-.src]iter4.h 
isrd1.obj :     [-.src]label.h, [-.src]label1.h, [-.site.vms]local.h, [-.src]mem.h 
isrd1.obj :     [-.src]mem1.h, [-.src]parms.h, [-.src]prior.h, [-.src]prior1.h 
isrd1.obj :     [-.src]rodif.h, [-.src]io.h, [-.src]io1.h, [-.src]mm.h 
isrd1.obj :     [-.src]mm1.h, [-.src]rasn1.h, [-.src]values.h, [-.src]asn1.h 
isrd1.obj :     [-.src]rasn11.h, [-.src]rasn12.h, [-.src]rasn13.h, [-.src]rasn14.h 
isrd1.obj :     [-.src]rodif1.h, [-.src]rodif2.h, [-.src]rodif3.h, [-.src]rodif4.h 
isrd1.obj :     [-.src]wodif.h, [-.src]wasn1.h, [-.src]wasn11.h, [-.src]wasn12.h 
isrd1.obj :     [-.src]wasn13.h, [-.src]wasn14.h, [-.src]wasn15.h, [-.src]wodif1.h 
isrd1.obj :     [-.src]wodif2.h, [-.src]wodif3.h, [-.src]wodif4.h, isrd.h 
isrd1.obj :     isrd19.h, isrd20.h, ishsh.h

ispr9.obj :  ispr9.c, psap.h, manifest.h, config.h 
ispr9.obj :     [-.site.vms]lcsite.h, general.h

ispr8.obj :  ispr8.c, psap.h, manifest.h, config.h 
ispr8.obj :     [-.site.vms]lcsite.h, general.h

ispr7.obj :  ispr7.c, psap.h, manifest.h, config.h 
ispr7.obj :     [-.site.vms]lcsite.h, general.h

ispr6.obj :  ispr6.c, psap.h, manifest.h, config.h 
ispr6.obj :     [-.site.vms]lcsite.h, general.h

ispr5.obj :  ispr5.c, psap.h, manifest.h, config.h 
ispr5.obj :     [-.site.vms]lcsite.h, general.h

ispr4.obj :  ispr4.c, psap.h, manifest.h, config.h 
ispr4.obj :     [-.site.vms]lcsite.h, general.h

ispr3.obj :  ispr3.c, psap.h, manifest.h, config.h 
ispr3.obj :     [-.site.vms]lcsite.h, general.h

ispr2.obj :  ispr2.c, psap.h, manifest.h, config.h 
ispr2.obj :     [-.site.vms]lcsite.h, general.h

ispr19.obj :  ispr19.c, [-.src]values.h, [-.src]apityp.h, [-.site.vms]lcsite.h 
ispr19.obj :     [-.src]const.h, [-.src]errors.h, [-.src]apirt.h, [-.src]attrid.h 
ispr19.obj :     [-.src]tkerr.h, [-.src]tkerr1.h, [-.src]const1.h, [-.src]const2.h 
ispr19.obj :     [-.src]const3.h, [-.src]init.h, [-.src]tags.h, [-.src]acd.h 
ispr19.obj :     [-.src]attr.h, [-.src]aunion.h, [-.src]attr1.h, [-.src]attr2.h 
ispr19.obj :     [-.src]attr3.h, [-.src]attr4.h, [-.src]attr5.h, [-.src]attr6.h 
ispr19.obj :     [-.src]attr7.h, [-.src]attr8.h, [-.src]attr9.h, [-.src]attr10.h 
ispr19.obj :     [-.src]attr11.h, [-.src]attr12.h, [-.src]attr13.h, [-.src]attr15.h 
ispr19.obj :     [-.src]attr14.h, [-.src]attr16.h, [-.src]attr17.h, [-.src]attr18.h 
ispr19.obj :     [-.src]attr19.h, [-.src]attr20.h, [-.src]attr21.h, [-.src]attr22.h 
ispr19.obj :     [-.src]attr23.h, [-.src]attr24.h, [-.src]attr25.h, [-.src]attr26.h 
ispr19.obj :     [-.src]doc.h, [-.src]doc1.h, [-.src]str.h, [-.src]seq.h 
ispr19.obj :     [-.src]seq1.h, [-.src]str1.h, [-.src]init1.h, ispr.h 
ispr19.obj :     ispr19.h, psap.h, manifest.h, config.h 
ispr19.obj :     general.h

ispr18.obj :  ispr18.c, psap.h, manifest.h, config.h 
ispr18.obj :     [-.site.vms]lcsite.h, general.h

ispr17.obj :  ispr17.c, psap.h, manifest.h, config.h 
ispr17.obj :     [-.site.vms]lcsite.h, general.h

ispr16.obj :  ispr16.c, psap.h, manifest.h, config.h 
ispr16.obj :     [-.site.vms]lcsite.h, general.h

ispr15.obj :  ispr15.c, psap.h, manifest.h, config.h 
ispr15.obj :     [-.site.vms]lcsite.h, general.h

ispr14.obj :  ispr14.c, psap.h, manifest.h, config.h 
ispr14.obj :     [-.site.vms]lcsite.h, general.h

ispr13.obj :  ispr13.c, psap.h, manifest.h, config.h 
ispr13.obj :     [-.site.vms]lcsite.h, general.h

ispr12.obj :  ispr12.c, psap.h, manifest.h, config.h 
ispr12.obj :     [-.site.vms]lcsite.h, general.h

ispr11.obj :  ispr11.c, psap.h, manifest.h, config.h 
ispr11.obj :     [-.site.vms]lcsite.h, general.h

ispr10.obj :  ispr10.c, psap.h, manifest.h, config.h 
ispr10.obj :     [-.site.vms]lcsite.h, general.h

ispr1.obj :  ispr1.c, psap.h, manifest.h, config.h 
ispr1.obj :     [-.site.vms]lcsite.h, general.h

isofiles.obj :  isofiles.c, general.h, config.h, [-.site.vms]lcsite.h 
isofiles.obj :     manifest.h, tailor.h, logger.h

isobject.obj :  isobject.c, psap.h, manifest.h, config.h 
isobject.obj :     [-.site.vms]lcsite.h, general.h, tailor.h, logger.h

bit_ops.obj :  bit_ops.c, psap.h, manifest.h, config.h 
bit_ops.obj :     [-.site.vms]lcsite.h, general.h

asprintf.obj :  asprintf.c, general.h, config.h, [-.site.vms]lcsite.h 
asprintf.obj :     manifest.h

UPEPY-52.obj :  UPEPY-52.c, psap.h, manifest.h, config.h 
UPEPY-52.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-51.obj :  UPEPY-51.c, psap.h, manifest.h, config.h 
UPEPY-51.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-50.obj :  UPEPY-50.c, psap.h, manifest.h, config.h 
UPEPY-50.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-49.obj :  UPEPY-49.c, psap.h, manifest.h, config.h 
UPEPY-49.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-48.obj :  UPEPY-48.c, psap.h, manifest.h, config.h 
UPEPY-48.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-47.obj :  UPEPY-47.c, psap.h, manifest.h, config.h 
UPEPY-47.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-46.obj :  UPEPY-46.c, psap.h, manifest.h, config.h 
UPEPY-46.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-36.obj :  UPEPY-36.c, psap.h, manifest.h, config.h 
UPEPY-36.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-35.obj :  UPEPY-35.c, psap.h, manifest.h, config.h 
UPEPY-35.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-34.obj :  UPEPY-34.c, psap.h, manifest.h, config.h 
UPEPY-34.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-12.obj :  UPEPY-12.c, psap.h, manifest.h, config.h 
UPEPY-12.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-11.obj :  UPEPY-11.c, psap.h, manifest.h, config.h 
UPEPY-11.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-10.obj :  UPEPY-10.c, psap.h, manifest.h, config.h 
UPEPY-10.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-09.obj :  UPEPY-09.c, psap.h, manifest.h, config.h 
UPEPY-09.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-08.obj :  UPEPY-08.c, psap.h, manifest.h, config.h 
UPEPY-08.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-07.obj :  UPEPY-07.c, psap.h, manifest.h, config.h 
UPEPY-07.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-03.obj :  UPEPY-03.c, psap.h, manifest.h, config.h 
UPEPY-03.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-02.obj :  UPEPY-02.c, psap.h, manifest.h, config.h 
UPEPY-02.obj :     [-.site.vms]lcsite.h, general.h

UPEPY-01.obj :  UPEPY-01.c, psap.h, manifest.h, config.h 
UPEPY-01.obj :     [-.site.vms]lcsite.h, general.h

libisode.olb :    vprint.obj, tailor.obj, str2vec.obj, str2prim.obj, str2oid.obj, str2elem.obj 
libisode.olb :    std2ps.obj, sprintoid.obj, set_find.obj, set_add.obj, serror.obj, seq_find.obj 
libisode.olb :    seq_del.obj, seq_addon.obj, seq_add.obj, py_advise.obj, ps_prime.obj, ps_io.obj 
libisode.olb :    ps_get_abs.obj, ps_free.obj, ps_flush.obj, ps_alloc.obj, ps2pe.obj, prim2str.obj 
libisode.olb :    prim2set.obj, prim2oid.obj, prim2num.obj, prim2flag.obj, prim2bit.obj, pl_tables.obj 
libisode.olb :    pe_free.obj, pe_error.obj, pe_alloc.obj, pe2ps.obj, oid_free.obj, oid_cpy.obj 
libisode.olb :    oid_cmp.obj, oid2ode.obj, objectbyoid.obj, obj2prim.obj, num2prim.obj, logger.obj 
libisode.olb :    iswr9.obj, iswr8.obj, iswr7.obj, iswr6.obj, iswr5.obj, iswr4.obj 
libisode.olb :    iswr3.obj, iswr20.obj, iswr2.obj, iswr19.obj, iswr18.obj, iswr17.obj 
libisode.olb :    iswr16.obj, iswr15.obj, iswr14.obj, iswr13.obj, iswr12.obj, iswr11.obj 
libisode.olb :    iswr10.obj, iswr1.obj, isrd9.obj, isrd8.obj, isrd7.obj, isrd6.obj 
libisode.olb :    isrd5.obj, isrd4.obj, isrd3.obj, isrd20.obj, isrd2.obj, isrd19.obj 
libisode.olb :    isrd18.obj, isrd17.obj, isrd16.obj, isrd15.obj, isrd14.obj, isrd13.obj 
libisode.olb :    isrd12.obj, isrd11.obj, isrd10.obj, isrd1.obj, ispr9.obj, ispr8.obj 
libisode.olb :    ispr7.obj, ispr6.obj, ispr5.obj, ispr4.obj, ispr3.obj, ispr2.obj 
libisode.olb :    ispr19.obj, ispr18.obj, ispr17.obj, ispr16.obj, ispr15.obj, ispr14.obj 
libisode.olb :    ispr13.obj, ispr12.obj, ispr11.obj, ispr10.obj, ispr1.obj, isofiles.obj 
libisode.olb :    isobject.obj, bit_ops.obj, asprintf.obj, UPEPY-52.obj, UPEPY-51.obj, UPEPY-50.obj 
libisode.olb :    UPEPY-49.obj, UPEPY-48.obj, UPEPY-47.obj, UPEPY-46.obj, UPEPY-36.obj, UPEPY-35.obj 
libisode.olb :    UPEPY-34.obj, UPEPY-12.obj, UPEPY-11.obj, UPEPY-10.obj, UPEPY-09.obj, UPEPY-08.obj 
libisode.olb :    UPEPY-07.obj, UPEPY-03.obj, UPEPY-02.obj, UPEPY-01.obj
	   $(AR1) libisode.olb vprint.obj, tailor.obj, str2vec.obj, str2prim.obj, str2oid.obj 
	   $(AR2) libisode.olb str2elem.obj, std2ps.obj, sprintoid.obj, set_find.obj, set_add.obj 
	   $(AR2) libisode.olb serror.obj, seq_find.obj, seq_del.obj, seq_addon.obj, seq_add.obj 
	   $(AR2) libisode.olb py_advise.obj, ps_prime.obj, ps_io.obj, ps_get_abs.obj, ps_free.obj 
	   $(AR2) libisode.olb ps_flush.obj, ps_alloc.obj, ps2pe.obj, prim2str.obj, prim2set.obj 
	   $(AR2) libisode.olb prim2oid.obj, prim2num.obj, prim2flag.obj, prim2bit.obj, pl_tables.obj 
	   $(AR2) libisode.olb pe_free.obj, pe_error.obj, pe_alloc.obj, pe2ps.obj, oid_free.obj 
	   $(AR2) libisode.olb oid_cpy.obj, oid_cmp.obj, oid2ode.obj, objectbyoid.obj, obj2prim.obj 
	   $(AR2) libisode.olb num2prim.obj, logger.obj, iswr9.obj, iswr8.obj, iswr7.obj 
	   $(AR2) libisode.olb iswr6.obj, iswr5.obj, iswr4.obj, iswr3.obj, iswr20.obj 
	   $(AR2) libisode.olb iswr2.obj, iswr19.obj, iswr18.obj, iswr17.obj, iswr16.obj 
	   $(AR2) libisode.olb iswr15.obj, iswr14.obj, iswr13.obj, iswr12.obj, iswr11.obj 
	   $(AR2) libisode.olb iswr10.obj, iswr1.obj, isrd9.obj, isrd8.obj, isrd7.obj 
	   $(AR2) libisode.olb isrd6.obj, isrd5.obj, isrd4.obj, isrd3.obj, isrd20.obj 
	   $(AR2) libisode.olb isrd2.obj, isrd19.obj, isrd18.obj, isrd17.obj, isrd16.obj 
	   $(AR2) libisode.olb isrd15.obj, isrd14.obj, isrd13.obj, isrd12.obj, isrd11.obj 
	   $(AR2) libisode.olb isrd10.obj, isrd1.obj, ispr9.obj, ispr8.obj, ispr7.obj 
	   $(AR2) libisode.olb ispr6.obj, ispr5.obj, ispr4.obj, ispr3.obj, ispr2.obj 
	   $(AR2) libisode.olb ispr19.obj, ispr18.obj, ispr17.obj, ispr16.obj, ispr15.obj 
	   $(AR2) libisode.olb ispr14.obj, ispr13.obj, ispr12.obj, ispr11.obj, ispr10.obj 
	   $(AR2) libisode.olb ispr1.obj, isofiles.obj, isobject.obj, bit_ops.obj, asprintf.obj 
	   $(AR2) libisode.olb UPEPY-52.obj, UPEPY-51.obj, UPEPY-50.obj, UPEPY-49.obj, UPEPY-48.obj 
	   $(AR2) libisode.olb UPEPY-47.obj, UPEPY-46.obj, UPEPY-36.obj, UPEPY-35.obj, UPEPY-34.obj 
	   $(AR2) libisode.olb UPEPY-12.obj, UPEPY-11.obj, UPEPY-10.obj, UPEPY-09.obj, UPEPY-08.obj 
	   $(AR2) libisode.olb UPEPY-07.obj, UPEPY-03.obj, UPEPY-02.obj, UPEPY-01.obj
	   purge libisode.olb
