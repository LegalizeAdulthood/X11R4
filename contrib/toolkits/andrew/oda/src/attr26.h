
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr26.h,v 1.1 89/09/21 06:22:22 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr26.h,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr26.h,v $
 * Revision 1.1  89/09/21  06:22:22  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  14:36:14  mss
 * removed leading #
 * 
 * Revision 1.1  89/04/11  15:39:22  annm
 * Initial revision
 * 
*/


#ifndef TK_INCLUDED_ATTR26
#define TK_INCLUDED_ATTR26

#undef EXTERN

#ifdef TK_GVARS_ATTR26
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */

#ifdef TK_IMPL_ATTR

#ifndef USE_LONG_NAMES

#define ATTR_CopyBindingPair	gatcbp
#define ATTR_CopyStringExpr	gatcse

#define ATTR_AllocBindingPair	gatabp
#define ATTR_AllocStringExpr	gatase

#define ATTR_FreeBindingPair	gatfbp
#define ATTR_FreeStringExpr	gatfse

#define ATTR_CkValBindingPair	gatkbp
#define ATTR_CheckStringExpr	gatkse

#endif

EXTERN INT_type SEQUENCE_BINDING_PAIR_Copy();
EXTERN INT_type ATTR_CopyBindingPair();
EXTERN INT_type ATTR_CopyStringExpr();

EXTERN POINTER_type SEQUENCE_BINDING_PAIR_Alloc();
EXTERN INT_type ATTR_AllocBindingPair();
EXTERN INT_type ATTR_AllocStringExpr();

EXTERN INT_type SEQUENCE_BINDING_PAIR_Free();
EXTERN INT_type ATTR_FreeBindingPair();
EXTERN INT_type ATTR_FreeStringExpr();

EXTERN INT_type SEQUENCE_BINDING_PAIR_Set();
EXTERN POINTER_type SEQUENCE_BINDING_PAIR_Get();

EXTERN INT_type SEQUENCE_BINDING_PAIR_CkVal();
EXTERN INT_type ATTR_CkValBindingPair();
EXTERN INT_type ATTR_CheckStringExpr();

EXTERN INT_type SEQUENCE_BINDING_PAIR_Deflt();

#endif

#endif
