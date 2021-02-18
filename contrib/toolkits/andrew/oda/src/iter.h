
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter.h,v 1.2 89/09/29 11:40:37 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	iter.h,v $
 * Revision 1.2  89/09/29  11:40:37  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:31:43  mss
 * Initial revision
 * 
 * Revision 1.16  89/09/11  14:38:24  mss
 * removed leading #
 * 
 * Revision 1.15  89/04/24  14:26:17  mss
 * Added iter4.h inclusion
 * 
 * Revision 1.14  88/12/23  16:56:40  mss
 * Added iter3.h inclusion
 * 
 * Revision 1.13  88/09/06  13:20:54  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.12  88/09/06  12:05:08  annm
 * 
 * no change
 * 
 * Revision 1.11  88/08/25  15:10:29  mss
 * Added iter2.h inclusion
 * 
 * Revision 1.10  88/06/08  16:57:06  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.9  88/06/08  14:36:14  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_ITER
#define	TK_INCLUDED_ITER

#ifdef TK_TKI_ITER
#define TK_ITER
#else
#ifdef TK_IMPL_ITER
#define	TK_ITER
#else
#define AP_ITER
#endif
#endif


#include <lcsite.h>

#include <acd.h>

#include <errors.h>

#include <tkerr.h>

#include <apirt.h>

/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_ITER.

*/

/* 
    put the stuff seen across the user interface here 
    as given in the API.

*/

/* This must be fixed for truth and visibility */

typedef POINTER_type ITERATOR_type;

/* 
    put the stuff seen inside the toolkit after the
    #ifdef TK_<filename>: i.e. TK_TKI or TK_IMPL/TK_GVARS
    sees this section.

*/

/* everyone sees this */
#define ERROR_ITERATOR ((ITERATOR_type) 0)

#ifdef  TK_ITER

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_ITER

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <iter1.h>
#include <iter2.h>
#include <iter3.h>
#include <iter4.h>

#endif
