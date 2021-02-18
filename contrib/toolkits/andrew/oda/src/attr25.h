
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr25.h,v 1.1 89/09/21 06:22:02 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr25.h,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr25.h,v $
 * Revision 1.1  89/09/21  06:22:02  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  14:36:02  mss
 * removed leading #
 * 
 * Revision 1.1  89/04/11  15:38:56  annm
 * Initial revision
 * 
*/


#ifndef TK_INCLUDED_ATTR25
#define TK_INCLUDED_ATTR25

#undef EXTERN

#ifdef TK_GVARS_ATTR25
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */

#ifdef TK_IMPL_ATTR

#ifndef USE_LONG_NAMES

#define ATTR_CopyAtomicStrExpr	    gatcas
#define ATTR_CopyRefBindingValue    gatrgv
#define ATTR_CopyObjectRef	    gatcor
#define ATTR_CopyObjectIdExpr	    gatoie
#define ATTR_CopyCurrInstanceParms  gatcip
#define ATTR_CopyStringFunction	    gatcsf
#define ATTR_CopyNumericExpr	    gatcne
#define ATTR_CopyNumericFunction    gatcnf

#define ATTR_AllocAtomicStrExpr	    gataas
#define ATTR_AllocRefBindingValue   gatarb
#define ATTR_AllocObjectRef	    gataor
#define ATTR_AllocObjectIdExpr	    gataoi
#define ATTR_AllocCurrInstanceParms gataci
#define ATTR_AllocStringFunction    gatasf
#define ATTR_AllocNumericExpr	    gatane
#define ATTR_AllocNumericFunction   gatanf

#define ATTR_FreeAtomicStrExpr	    gatfas
#define ATTR_FreeRefBindingValue    gatfrb
#define ATTR_FreeObjectRef	    gatfor
#define ATTR_FreeObjectIdExpr	    gatfoi
#define ATTR_FreeCurrInstanceParms  gatfci
#define ATTR_FreeStringFunction	    gatfsf
#define ATTR_FreeNumericExpr	    gatfne
#define ATTR_FreeNumericFunction    gatfnf

#define ATTR_CkValAtomicStrExpr	    gatkas
#define ATTR_CkValRefBindingValue   gatkrb
#define ATTR_CkValObjectRef	    gatkor
#define ATTR_CkValObjectIdExpr	    gatkoi
#define ATTR_CkValCurrInstanceParms gatkci
#define ATTR_CkValStringFunction    gatksf
#define ATTR_CkValNumericExpr	    gatkne
#define ATTR_CkValNumericFunction   gatknf

#endif

EXTERN INT_type STRING_EXPR_Copy();
EXTERN INT_type ATTR_CopyAtomicStrExpr();
EXTERN INT_type ATTR_CopyRefBindingValue();
EXTERN INT_type ATTR_CopyObjectRef();
EXTERN INT_type ATTR_CopyObjectIdExpr();
EXTERN INT_type ATTR_CopyCurrInstanceParms();
EXTERN INT_type ATTR_CopyStringFunction();
EXTERN INT_type ATTR_CopyNumericExpr();
EXTERN INT_type ATTR_CopyNumericFunction();

EXTERN POINTER_type STRING_EXPR_Alloc();
EXTERN INT_type ATTR_AllocAtomicStrExpr();
EXTERN INT_type ATTR_AllocRefBindingValue();
EXTERN INT_type ATTR_AllocObjectRef();
EXTERN INT_type ATTR_AllocObjectIdExpr();
EXTERN INT_type ATTR_AllocCurrInstanceParms();
EXTERN INT_type ATTR_AllocStringFunction();
EXTERN INT_type ATTR_AllocNumericExpr();
EXTERN INT_type ATTR_AllocNumericFunction();

EXTERN INT_type STRING_EXPR_Free();
EXTERN INT_type ATTR_FreeAtomicStrExpr();
EXTERN INT_type ATTR_FreeRefBindingValue();
EXTERN INT_type ATTR_FreeObjectRef();
EXTERN INT_type ATTR_FreeObjectIdExpr();
EXTERN INT_type ATTR_FreeCurrInstanceParms();
EXTERN INT_type ATTR_FreeStringFunction();
EXTERN INT_type ATTR_FreeNumericExpr();
EXTERN INT_type ATTR_FreeNumericFunction();

EXTERN INT_type STRING_EXPR_Set();
EXTERN POINTER_type STRING_EXPR_Get();

EXTERN INT_type STRING_EXPR_CkVal();
EXTERN INT_type ATTR_CkValAtomicStrExpr();
EXTERN INT_type ATTR_CkValRefBindingValue();
EXTERN INT_type ATTR_CkValObjectRef();
EXTERN INT_type ATTR_CkValObjectIdExpr();
EXTERN INT_type ATTR_CkValCurrInstanceParms();
EXTERN INT_type ATTR_CkValStringFunction();
EXTERN INT_type ATTR_CkValNumericExpr();
EXTERN INT_type ATTR_CkValNumericFunction();

EXTERN INT_type STRING_EXPR_Deflt();

#endif

#endif
