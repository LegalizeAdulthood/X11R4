
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif.h,v 1.2 89/09/29 11:41:54 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wodif.h,v $
 * Revision 1.2  89/09/29  11:41:54  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:43:49  mss
 * Initial revision
 * 
 * Revision 1.13  89/09/11  14:40:49  mss
 * removed leading #
 * 
 * Revision 1.12  88/10/13  22:03:53  mz0k
 * included <wodif4.h>
 * 
 * Revision 1.11  88/09/06  13:22:55  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.10  88/09/06  12:06:50  annm
 * 
 * no change
 * 
 * Revision 1.9  88/08/04  14:28:10  mz0k
 * added copyright notice
 * 
*/


#ifndef TK_INCLUDED_WODIF
#define	TK_INCLUDED_WODIF

#ifdef TK_TKI_WODIF
#define TK_WODIF
#else
#ifdef TK_IMPL_WODIF
#define	TK_WODIF
#else
#define AP_WODIF
#endif
#endif

#include <lcsite.h>

#include <acd.h>

#include <debug.h>
#include <values.h>
#include <io.h>
#include <errors.h>
#include <apirt.h>
#include <tkerr.h>

#include <asn1.h>
#include <wasn1.h>
/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_WODIF.

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

#ifdef  TK_WODIF

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_WODIF


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <wodif1.h>
#include <wodif2.h>
#include <wodif3.h>
#include <wodif4.h>

#endif
