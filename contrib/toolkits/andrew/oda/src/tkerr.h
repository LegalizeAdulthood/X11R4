
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/tkerr.h,v 1.2 89/09/29 11:41:39 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/tkerr.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	tkerr.h,v $
 * Revision 1.2  89/09/29  11:41:39  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:40:57  mss
 * Initial revision
 * 
 * Revision 1.9  89/09/11  14:29:21  mss
 * removed leading #
 * 
 * Revision 1.8  88/09/06  13:22:26  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.7  88/09/06  12:06:40  annm
 * 
 * no change
 * 
 * Revision 1.6  88/06/08  16:58:43  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.5  88/06/08  14:37:50  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_TKERR
#define	TK_INCLUDED_TKERR

#ifdef TK_TKI_TKERR
#define TK_TKERR
#else
#ifdef TK_IMPL_TKERR
#define	TK_TKERR
#else
#define AP_TKERR
#endif
#endif

#include <lcsite.h>
#include <errors.h>

/* 
    NOTE: If called with -DTK_GVARS_TKERR1 then must also call with
    -DTK_IMPL_TKERR.

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

#ifdef  TK_TKERR

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_TKERR


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <tkerr1.h>

#endif
