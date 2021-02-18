
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter1.h,v 1.1 89/09/21 06:32:01 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	iter1.h,v $
 * Revision 1.1  89/09/21  06:32:01  mss
 * Initial revision
 * 
 * Revision 1.17  89/08/29  16:07:53  annm
 * added real names for names that are too long
 * 
 * Revision 1.16  89/04/24  14:28:29  mss
 * Moved style iterator into iter4.[ch]
 * 
 * Revision 1.15  89/01/10  11:44:08  jr
 * Coerce root-locating constants
 * correctly.
 * 
 * Revision 1.14  89/01/09  13:52:34  jr
 * Align with new API.
 * 
 * Revision 1.13  88/12/23  16:52:05  mss
 * Added conditional use of name mapping
 * 
 * Revision 1.12  88/09/06  13:20:57  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.11  88/09/06  12:05:13  annm
 * 
 * no change
 * 
 * Revision 1.10  88/08/25  15:10:44  mss
 * Removed make, delete and nextconstituent procs
 * 
 * Revision 1.9  88/06/08  16:57:14  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.8  88/06/08  14:36:21  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_ITER1
#define TK_INCLUDED_ITER1

#undef EXTERN

#ifdef TK_GVARS_ITER1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

    /* Definitions for VALUE_types */
#define SPECIFIC_DOC_LOGICAL_ROOT   ((VALUE_type) 1)
#define GENERIC_DOC_LOGICAL_ROOT    ((VALUE_type) 2)
#define SPECIFIC_DOC_LAYOUT_ROOT    ((VALUE_type) 3)
#define GENERIC_DOC_LAYOUT_ROOT	    ((VALUE_type) 4)

EXTERN	CONSTITUENT	FindDocumentProfile();
EXTERN	CONSTITUENT	FindDocumentRoot();

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_ITER

#ifndef USE_LONG_NAMES

#define ITER_Init   gitint
#define ITER_Final  gitfnl

#define DOC_CheckRoot	gitcrt

#endif

EXTERN INT_type ITER_Init();
EXTERN INT_type ITER_Final();

#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_ITER

#endif

#endif
