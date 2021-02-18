
/*

$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/label.h,v 1.2 89/09/29 11:40:41 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/label.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	label.h,v $
 * Revision 1.2  89/09/29  11:40:41  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:33:16  mss
 * Initial revision
 * 
 * Revision 1.9  88/09/06  13:21:07  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.8  88/09/06  12:05:18  annm
 * 
 * no change
 * 
 * Revision 1.7  88/07/05  16:56:58  jr
 * Does lots more
 * 
*/

#ifndef TK_INCLUDED_LABEL
#define	TK_INCLUDED_LABEL

#ifdef TK_TKI_LABEL
#define TK_LABEL
#else
#ifdef TK_IMPL_LABEL
#define	TK_LABEL
#else
#define AP_LABEL
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>
#include <tkerr.h>

/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_LABEL.

*/

/* Everyone sees these definitions */

/* 
    put the stuff seen inside the toolkit after the
    #ifdef TK_<filename>: i.e. TK_TKI or TK_IMPL/TK_GVARS
    sees this section.
*/

#ifdef  TK_LABEL
#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.
*/

#ifdef TK_IMPL_LABEL
#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter
*/

#include <label1.h>

#endif
