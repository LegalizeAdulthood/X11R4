
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mm.h,v 1.2 89/09/29 11:40:51 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mm.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	mm.h,v $
 * Revision 1.2  89/09/29  11:40:51  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:34:24  mss
 * Initial revision
 * 
 * Revision 1.5  88/09/06  13:21:36  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/09/06  12:05:55  annm
 * 
 * no change
 * 
 * Revision 1.3  88/06/08  16:57:37  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.2  88/06/08  14:36:44  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_MM
#define	TK_INCLUDED_MM

#ifdef TK_TKI_MM
#define TK_MM
#else
#ifdef TK_IMPL_MM
#define	TK_MM
#else
#define AP_MM
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>
#include <tkerr.h>
#include <local.h>

/* 
    Put the stuff seen across the user interface here 
    as given in the API.
*/

/* For now, there's nothing here -- but, I think stuff will end up here, eventually */

#ifdef  TK_MM

#endif

/*
    Put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.
*/

#ifdef TK_IMPL_MM


#endif

/* 
    Include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <mm1.h>

#endif
