
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr5.h,v 1.1 89/09/21 06:23:27 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr5.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log :$

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr5.h,v $
 * Revision 1.1  89/09/21  06:23:27  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/11  14:31:57  mss
 * removed leading #
 * 
 * Revision 1.5  89/08/30  17:04:25  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.4  88/09/19  16:57:24  annm
 * added error message formatting, canonical free and ckval routines
 * 
 * Revision 1.3  88/09/06  13:18:15  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/08/22  19:44:33  annm
 * tweeking canonical procedure dcls
 * 
 * Revision 1.1  88/07/21  16:37:02  annm
 * Initial revision
 * 
 * Revision 1.1  88/07/20  19:25:58  annm
 * Initial revision
 * 
*/


#ifndef TK_INCLUDED_ATTR5
#define TK_INCLUDED_ATTR5

#undef EXTERN

#ifdef TK_GVARS_ATTR5
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for routines in attr5.c come from rnames.h */
/* except for the term and factor utility routines */

#ifndef USE_LONG_NAMES

#define CONSTRUCTION_TERM_Copy	    gctcpy
#define CONSTRUCTION_FACTOR_Copy    gcfcpy
#define CONSTRUCTION_TERM_Alloc	    gctall
#define CONSTRUCTION_FACTOR_Alloc   gcfall
#define CONSTRUCTION_TERM_Free	    gctfre
#define CONSTRUCTION_FACTOR_Free    gcffre
#define CONSTRUCTION_TERM_CkVal	    gctckv
#define CONSTRUCTION_FACTOR_CkVal   gcfckv

#endif


/* declarations for routines in attr5.c */

EXTERN INT_type CONSTRUCTION_EXPR_Copy();
EXTERN POINTER_type CONSTRUCTION_EXPR_Alloc();
EXTERN INT_type CONSTRUCTION_EXPR_Free();
EXTERN INT_type CONSTRUCTION_EXPR_Set();
EXTERN POINTER_type CONSTRUCTION_EXPR_Get();
EXTERN INT_type CONSTRUCTION_EXPR_CkVal();
EXTERN INT_type CONSTRUCTION_EXPR_Deflt();

EXTERN INT_type CONSTRUCTION_TERM_Copy();
EXTERN INT_type CONSTRUCTION_FACTOR_Copy();
EXTERN INT_type CONSTRUCTION_TERM_Alloc();
EXTERN INT_type CONSTRUCTION_FACTOR_Alloc();
EXTERN INT_type CONSTRUCTION_TERM_Free();
EXTERN INT_type CONSTRUCTION_FACTOR_Free();
EXTERN INT_type CONSTRUCTION_TERM_CkVal();
EXTERN INT_type CONSTRUCTION_FACTOR_CkVal();

#endif

#endif
