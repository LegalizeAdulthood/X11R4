
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr20.h,v 1.2 89/10/31 14:28:26 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr20.h,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr20.h,v $
 * Revision 1.2  89/10/31  14:28:26  jr
 * Add decl of missing routine.
 * 
 * Revision 1.1  89/09/21  06:20:03  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/11  14:34:57  mss
 * removed leading #
 * 
 * Revision 1.5  89/04/21  18:15:23  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.4  89/02/01  14:56:42  annm
 * added #ifndef USE_LONG_NAMES
 * around real name #define's
 * 
 * Revision 1.3  89/01/27  17:30:43  annm
 * added short real name for auxilary procedure
 * 
 * Revision 1.2  89/01/26  18:18:59  annm
 * added canonical procedures for SEQUENCE_ASN1_OBJECT_ID_type
 * 
 * Revision 1.1  89/01/26  12:36:57  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR20
#define TK_INCLUDED_ATTR20

#undef EXTERN

#ifdef TK_GVARS_ATTR20
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */

#ifdef TK_IMPL_ATTR

#ifndef USE_LONG_NAMES 

#define ATTR_Check_at_MEDIUM_TYPE   gatcmt
#define ATTR_AllocASN1ObjectId	    gataao

#define	ATTR_FreeAsn1ObjectId    gatfa1

#endif

EXTERN INT_type SEQUENCE_VALUE_Copy();
EXTERN POINTER_type SEQUENCE_VALUE_Alloc();
EXTERN INT_type SEQUENCE_VALUE_Free();
EXTERN INT_type SEQUENCE_VALUE_Set();
EXTERN POINTER_type SEQUENCE_VALUE_Get();
EXTERN INT_type SEQUENCE_VALUE_CkVal();
EXTERN INT_type SEQUENCE_VALUE_Deflt();

EXTERN INT_type at_MEDIUM_TYPE_Copy();
EXTERN POINTER_type at_MEDIUM_TYPE_Alloc();
EXTERN INT_type at_MEDIUM_TYPE_Free();
EXTERN INT_type at_MEDIUM_TYPE_Set();
EXTERN POINTER_type at_MEDIUM_TYPE_Get();
EXTERN INT_type at_MEDIUM_TYPE_CkVal();
EXTERN INT_type ATTR_Check_at_MEDIUM_TYPE();
EXTERN INT_type at_MEDIUM_TYPE_Deflt();

EXTERN INT_type SEQUENCE_at_MEDIUM_TYPE_Copy();
EXTERN POINTER_type SEQUENCE_at_MEDIUM_TYPE_Alloc();
EXTERN INT_type SEQUENCE_at_MEDIUM_TYPE_Free();
EXTERN INT_type SEQUENCE_at_MEDIUM_TYPE_Set();
EXTERN POINTER_type SEQUENCE_at_MEDIUM_TYPE_Get();
EXTERN INT_type SEQUENCE_at_MEDIUM_TYPE_CkVal();
EXTERN INT_type SEQUENCE_at_MEDIUM_TYPE_Deflt();

EXTERN INT_type SEQUENCE_ASN1_OBJECT_ID_Copy();
EXTERN POINTER_type SEQUENCE_ASN1_OBJECT_ID_Alloc();
EXTERN INT_type ATTR_AllocASN1ObjectId();
EXTERN INT_type SEQUENCE_ASN1_OBJECT_ID_Free();
EXTERN INT_type ATTR_FreeAsn1ObjectId();
EXTERN INT_type SEQUENCE_ASN1_OBJECT_ID_Set();
EXTERN POINTER_type SEQUENCE_ASN1_OBJECT_ID_Get();
EXTERN INT_type SEQUENCE_ASN1_OBJECT_ID_CkVal();
EXTERN INT_type SEQUENCE_ASN1_OBJECT_ID_Deflt();

EXTERN INT_type ATTR_ASN1_OBJECT_ID_Free();

#endif

#endif
