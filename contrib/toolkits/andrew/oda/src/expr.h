
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/expr.h,v 1.2 89/09/29 11:40:10 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/expr.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	expr.h,v $
 * Revision 1.2  89/09/29  11:40:10  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:29:27  mss
 * Initial revision
 * 
 * Revision 1.12  89/09/11  14:37:35  mss
 * removed leading #
 * 
 * Revision 1.11  88/09/06  13:19:49  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.10  88/09/06  12:04:40  annm
 * 
 * no change
 * 
 * Revision 1.9  88/06/08  17:44:28  annm
 * removed suprious RCSID dcl
 * 
 * Revision 1.8  88/06/08  16:56:30  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.7  88/06/08  14:35:35  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_EXPR
#define	TK_INCLUDED_EXPR

#ifdef TK_TKI_EXPR
#define TK_EXPR
#else
#ifdef TK_IMPL_EXPR
#define	TK_EXPR
#else
#define AP_EXPR
#endif
#endif

#include <lcsite.h>

#include <errors.h>
#include <apirt.h>

#include <tkerr.h>

/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_EXPR.

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

#ifdef  TK_EXPR

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_EXPR


#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <expr1.h>

#endif
