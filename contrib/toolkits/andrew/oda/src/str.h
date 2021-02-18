
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/str.h,v 1.2 89/09/29 11:41:19 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/str.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	str.h,v $
 * Revision 1.2  89/09/29  11:41:19  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:39:39  mss
 * Initial revision
 * 
 * Revision 1.10  88/12/13  19:56:49  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.9  88/09/06  13:22:13  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.8  88/09/06  12:06:31  annm
 * 
 * no change
 * 
 * Revision 1.7  88/07/15  15:38:09  mss
 * Added include of tags.h
 * 
 * Revision 1.6  88/06/08  16:58:32  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.5  88/06/08  14:37:38  annm
 * added Copyright notice
 * 
*/
    
#ifndef TK_INCLUDED_STR
#define	TK_INCLUDED_STR

#ifdef TK_TKI_STR
#define TK_STR
#else
#ifdef TK_IMPL_STR
#define	TK_STR
#else
#define AP_STR
#endif
#endif

#include <lcsite.h>
#include <apityp.h>
#include <tags.h>
#include <seq.h>
#include <errors.h>
#include <apirt.h>
#include <tkerr.h>

/* 
    Put the stuff seen across the user interface here 
    as given in the API.
*/

#ifdef  TK_STR

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_STR


#endif

/*
     the  STRING's defined in the API. Note that storage for the 
    ERROR_STRING is allocated in STR_Init.
*/


/* 
    Include(s) for the subordinate .h file(s)
*/

#include <str1.h>

#endif
