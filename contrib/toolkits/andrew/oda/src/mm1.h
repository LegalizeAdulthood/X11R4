
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mm1.h,v 1.1 89/09/21 06:34:43 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mm1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	mm1.h,v $
 * Revision 1.1  89/09/21  06:34:43  mss
 * Initial revision
 * 
 * Revision 1.7  89/08/30  17:11:26  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.6  88/09/30  15:41:28  annm
 * fixing VMS bugs
 * 
 * Revision 1.5  88/09/06  13:21:40  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/09/06  12:05:59  annm
 * 
 * no change
 * 
 * Revision 1.3  88/06/08  16:57:45  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.2  88/06/08  14:36:51  annm
 * added Copyright notice
 * 
*/


#ifndef TK_INCLUDED_MM1
#define TK_INCLUDED_MM1

#undef EXTERN

#ifdef TK_GVARS_MM1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/*
    Declarations for the tool kit are here
*/

#ifdef TK_MM

#ifndef USE_LONG_NAMES

#define MM_Init		gmmini
#define MM_Final	gmmfin
#define MM_Malloc	gmmmal
#define MM_Realloc	gmmrel
#define MM_Free		gmmfre

#endif

EXTERN INT_type		MM_Init();
EXTERN INT_type		MM_Final();
EXTERN POINTER_type	MM_Malloc();
EXTERN POINTER_type	MM_Realloc();
EXTERN INT_type		MM_Free();

#endif

/*
    Declarations seen within the mm1.c file go here.
*/

#ifdef TK_IMPL_MM

#endif

#endif
