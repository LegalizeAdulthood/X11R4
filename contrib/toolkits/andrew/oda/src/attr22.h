
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr22.h,v 1.1 89/09/21 06:20:45 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr22.h,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr22.h,v $
 * Revision 1.1  89/09/21  06:20:45  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/11  14:35:20  mss
 * removed leading #
 * 
 * Revision 1.4  89/04/21  18:16:21  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.3  89/02/09  15:08:36  annm
 * added canonical procedures for dp_LOCAL_FILE_REFERENCE_type and at_BORDER_type
 * 
 * Revision 1.2  89/02/06  16:14:22  annm
 * added canonical procedures for SEQUENCE_STRING_OR_ID_type
 * 
 * Revision 1.1  89/02/02  16:47:12  annm
 * Initial revision
 * 
*/


#ifndef TK_INCLUDED_ATTR22
#define TK_INCLUDED_ATTR22

#undef EXTERN

#ifdef TK_GVARS_ATTR22
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */

#ifdef TK_IMPL_ATTR

#ifndef USE_LONG_NAMES

#define ATTR_Alloc_String_or_id_value	gatasi
#define ATTR_Free_String_or_id_value	gatfsi
#define ATTR_Ck_String_or_id_value	gatcsi

#define ATTR_BORDER_INFO_Copy		gatbic
#define ATTR_BORDER_INFO_CkVal		gatbik
#define ATTR_BORDER_INFO_Deflt		gatbid

#endif

EXTERN INT_type STRING_OR_ID_Copy();
EXTERN POINTER_type STRING_OR_ID_Alloc();
EXTERN INT_type ATTR_Alloc_String_or_id_value();
EXTERN INT_type STRING_OR_ID_Free();
EXTERN INT_type ATTR_Free_String_or_id_value();
EXTERN INT_type STRING_OR_ID_Set();
EXTERN POINTER_type STRING_OR_ID_Get();
EXTERN INT_type STRING_OR_ID_CkVal();
EXTERN INT_type ATTR_Ck_String_or_id_value();
EXTERN INT_type STRING_OR_ID_Deflt();

EXTERN INT_type SEQUENCE_STRING_OR_ID_Copy();
EXTERN POINTER_type SEQUENCE_STRING_OR_ID_Alloc();
EXTERN INT_type SEQUENCE_STRING_OR_ID_Free();
EXTERN INT_type SEQUENCE_STRING_OR_ID_Set();
EXTERN POINTER_type SEQUENCE_STRING_OR_ID_Get();
EXTERN INT_type SEQUENCE_STRING_OR_ID_CkVal();
EXTERN INT_type SEQUENCE_STRING_OR_ID_Deflt();

EXTERN INT_type dp_LOCAL_FILE_REFERENCE_Copy();
EXTERN POINTER_type dp_LOCAL_FILE_REFERENCE_Alloc();
EXTERN INT_type dp_LOCAL_FILE_REFERENCE_Free();
EXTERN INT_type dp_LOCAL_FILE_REFERENCE_Set();
EXTERN POINTER_type dp_LOCAL_FILE_REFERENCE_Get();
EXTERN INT_type dp_LOCAL_FILE_REFERENCE_CkVal();
EXTERN INT_type dp_LOCAL_FILE_REFERENCE_Deflt();

EXTERN INT_type at_BORDER_Copy();
EXTERN INT_type ATTR_BORDER_INFO_Copy();
EXTERN POINTER_type at_BORDER_Alloc();
EXTERN INT_type at_BORDER_Free();
EXTERN INT_type at_BORDER_Set();
EXTERN POINTER_type at_BORDER_Get();
EXTERN INT_type at_BORDER_CkVal();
EXTERN INT_type ATTR_BORDER_INFO_CkVal();
EXTERN INT_type at_BORDER_Deflt();
EXTERN INT_type ATTR_BORDER_INFO_Deflt();

#endif

#endif

