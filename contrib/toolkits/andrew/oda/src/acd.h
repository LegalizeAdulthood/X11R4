
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/acd.h,v 1.1 89/09/21 06:14:48 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/acd.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	acd.h,v $
 * Revision 1.1  89/09/21  06:14:48  mss
 * Initial revision
 * 
Revision 1.4  89/09/11  14:30:27  mss
removed leading #

Revision 1.3  88/09/06  13:16:55  annm
changed include ""'s to <>'s

Revision 1.2  88/09/06  12:03:26  annm

no change

Revision 1.1  88/06/08  19:56:31  mss
Initial revision

 * Revision 1.8  88/06/08  16:52:26  annm
 * dealt with line too long in copyright notice and changed
 * #end's to #endif's
 * 
 * Revision 1.7  88/06/08  14:33:40  annm
 * added Copyright notice
 * 
*/


#ifndef TK_INCLUDED_ACD
#define	TK_INCLUDED_ACD

#ifdef TK_TKI_ACD
#define TK_ACD
#else
#ifdef TK_IMPL_ACD
#define	TK_ACD
#else
#define AP_ACD
#endif
#endif


#include <attr.h>
#include <const.h>
#include <doc.h>

/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_ACD.

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

#ifdef  TK_ACD

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_ACD


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/


#endif
