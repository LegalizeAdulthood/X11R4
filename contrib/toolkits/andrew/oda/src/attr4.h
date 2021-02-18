
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr4.h,v 1.1 89/09/21 06:23:09 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr4.h,v $


(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr4.h,v $
 * Revision 1.1  89/09/21  06:23:09  mss
 * Initial revision
 * 
 * Revision 1.10  89/09/11  14:31:45  mss
 * removed leading #
 * 
 * Revision 1.9  89/08/30  17:04:11  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.8  89/04/21  18:17:09  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.7  88/09/19  16:57:01  annm
 * added error message formatting, canonical free and ckval routines
 * 
 * Revision 1.6  88/09/06  13:18:10  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.5  88/09/06  12:03:57  annm
 * 
 * no change
 * 
 * Revision 1.4  88/08/22  19:44:12  annm
 * tweeking the blasted masks
 * 
 * Revision 1.3  88/07/26  18:41:50  annm
 * added copy, allocate, get and set canonical procedures for at_OFFSET_type and at_SEPARATION_type
 * 
 * Revision 1.2  88/07/21  14:53:44  annm
 * changing 3's to 4's - result of clone edit
 * 
 * Revision 1.1  88/07/20  19:25:58  annm
 * Initial revision
 * 
*/


#ifndef TK_INCLUDED_ATTR4
#define TK_INCLUDED_ATTR4

#undef EXTERN

#ifdef TK_GVARS_ATTR4
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for routines in attr4.c come from rnames.h */

#ifndef USE_LONG_NAMES

#define ATTR_Free_Some_String    gatfst

#define ATTR_FreeStrings	gatfsr

#endif

/* declarations for routines in attr4.c */

EXTERN INT_type LAYOUT_OBJECT_Copy();
EXTERN POINTER_type LAYOUT_OBJECT_Alloc();
EXTERN INT_type LAYOUT_OBJECT_Free();
EXTERN INT_type LAYOUT_OBJECT_Set();
EXTERN POINTER_type LAYOUT_OBJECT_Get();
EXTERN INT_type LAYOUT_OBJECT_CkVal();
EXTERN INT_type LAYOUT_OBJECT_Deflt();

EXTERN INT_type at_OFFSET_Copy();
EXTERN POINTER_type at_OFFSET_Alloc();
EXTERN INT_type at_OFFSET_Free();
EXTERN INT_type at_OFFSET_Set();
EXTERN POINTER_type at_OFFSET_Get();
EXTERN INT_type at_OFFSET_CkVal();
EXTERN INT_type at_OFFSET_Deflt();

EXTERN INT_type at_SEPARATION_Copy();
EXTERN POINTER_type at_SEPARATION_Alloc();
EXTERN INT_type at_SEPARATION_Free();
EXTERN INT_type at_SEPARATION_Set();
EXTERN POINTER_type at_SEPARATION_Get();
EXTERN INT_type at_SEPARATION_CkVal();
EXTERN INT_type at_SEPARATION_Deflt();

EXTERN INT_type SEQUENCE_STRING_Copy();
EXTERN POINTER_type SEQUENCE_STRING_Alloc();
EXTERN INT_type ATTR_Free_Some_String();
EXTERN INT_type ATTR_Free_String();
EXTERN INT_type ATTR_FreeStrings();
EXTERN INT_type SEQUENCE_STRING_Set();
EXTERN POINTER_type SEQUENCE_STRING_Get();
EXTERN INT_type SEQUENCE_STRING_CkVal();
EXTERN INT_type SEQUENCE_STRING_Deflt();

#endif

#endif
