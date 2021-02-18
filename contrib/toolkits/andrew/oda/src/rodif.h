
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif.h,v 1.2 89/09/29 11:41:09 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rodif.h,v $
 * Revision 1.2  89/09/29  11:41:09  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:37:20  mss
 * Initial revision
 * 
 * Revision 1.13  89/09/11  14:39:35  mss
 * removed leading #
 * 
 * Revision 1.12  88/10/21  16:49:08  mz0k
 * included rodif4.h
 * 
 * Revision 1.11  88/09/13  14:51:39  mz0k
 * changed "" to <> & other stuffs
 * 
 * Revision 1.10  88/09/07  14:42:44  mz0k
 * Started coding.
 * 
 * Revision 1.9  88/06/08  16:58:05  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.8  88/06/08  14:37:13  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_RODIF
#define	TK_INCLUDED_RODIF

#ifdef TK_TKI_RODIF
#define TK_RODIF
#else
#ifdef TK_IMPL_RODIF
#define	TK_RODIF
#else
#define AP_RODIF
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>
#include <tkerr.h>
#include <tags.h>
#include <acd.h>
#include <seq.h>
#include <io.h>
#include <parms.h>
#include <mm.h>
#include <str.h>

#include <rasn1.h>

/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_RODIF.

*/

/* 
    put the stuff seen across the user interface here 
    as given in the API.

*/

/* 
    put the stuff seen inside the toolkit after the
    #ifdef TK_<filename>: i.e. TK_TKI or TK_IMPL/TK_GVARS
    sees this section.

*/

#ifdef  TK_RODIF

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_RODIF


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <rodif1.h>
#include <rodif2.h>
#include <rodif3.h>
#include <rodif4.h>

#endif
