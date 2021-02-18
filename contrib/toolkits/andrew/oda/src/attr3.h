
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr3.h,v 1.1 89/09/21 06:22:49 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr3.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr3.h,v $
 * Revision 1.1  89/09/21  06:22:49  mss
 * Initial revision
 * 
 * Revision 1.15  89/09/11  14:31:32  mss
 * removed leading #
 * 
 * Revision 1.14  89/08/30  16:59:48  annm
 * no change
 * 
 * Revision 1.13  88/09/19  16:55:35  annm
 * added error message formatting, canonical free and ckval routines
 * 
 * Revision 1.12  88/09/06  13:18:03  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.11  88/09/06  12:03:52  annm
 * 
 * no change
 * 
 * Revision 1.10  88/08/22  19:43:46  annm
 * moved canonical procedures for STRING_type and VALUE_type to the new attr10
 * 
 * Revision 1.9  88/07/15  18:29:08  annm
 * canonical aunion.h procs for 7 types
 * 
 * Revision 1.8  88/07/14  17:49:19  annm
 * changes to use canonical procedures for operating on aunion.h types for the SetAttr stuff
 * 
 * Revision 1.7  88/06/17  15:04:05  annm
 * changes to Set/Get Attr to:
 * 	1) on set attr side: plug in
 * 		more attributes whose
 * 		types are already there.
 * 	2) on Get side: Get now does all
 * 	the types and attributes that 
 * 	Set does
 * 
 * Revision 1.6  88/06/08  16:55:06  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.5  88/06/08  14:34:26  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_ATTR3
#define TK_INCLUDED_ATTR3

#undef EXTERN

#ifdef TK_GVARS_ATTR3
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for routines in attr3.c come from rnames.h */


/* declarations for routines in attr3.c */

EXTERN INT_type CONSTITUENT_Copy();
EXTERN POINTER_type CONSTITUENT_Alloc();
EXTERN INT_type CONSTITUENT_Free();
EXTERN INT_type CONSTITUENT_Set();
EXTERN POINTER_type CONSTITUENT_Get();
EXTERN INT_type CONSTITUENT_CkVal();
EXTERN INT_type CONSTITUENT_Deflt();

EXTERN INT_type INT_Copy();
EXTERN POINTER_type INT_Alloc();
EXTERN INT_type INT_Free();
EXTERN INT_type INT_Set();
EXTERN POINTER_type INT_Get();
EXTERN INT_type INT_CkVal();
EXTERN INT_type INT_Deflt();

EXTERN INT_type SEQUENCE_BYTE_Copy();
EXTERN POINTER_type SEQUENCE_BYTE_Alloc();
EXTERN INT_type SEQUENCE_BYTE_Free();
EXTERN INT_type SEQUENCE_BYTE_Set();
EXTERN POINTER_type SEQUENCE_BYTE_Get();
EXTERN INT_type SEQUENCE_BYTE_CkVal();
EXTERN INT_type SEQUENCE_BYTE_Deflt();

EXTERN INT_type SEQUENCE_CONSTITUENT_Copy();
EXTERN POINTER_type SEQUENCE_CONSTITUENT_Alloc();
EXTERN INT_type SEQUENCE_CONSTITUENT_Free();
EXTERN INT_type SEQUENCE_CONSTITUENT_Set();
EXTERN POINTER_type SEQUENCE_CONSTITUENT_Get();
EXTERN INT_type SEQUENCE_CONSTITUENT_CkVal();
EXTERN INT_type SEQUENCE_CONSTITUENT_Deflt();

EXTERN INT_type SEQUENCE_INT_Copy();
EXTERN POINTER_type SEQUENCE_INT_Alloc();
EXTERN INT_type SEQUENCE_INT_Free();
EXTERN INT_type SEQUENCE_INT_Set();
EXTERN POINTER_type SEQUENCE_INT_Get();
EXTERN INT_type SEQUENCE_INT_CkVal();
EXTERN INT_type SEQUENCE_INT_Deflt();

#endif

#endif

