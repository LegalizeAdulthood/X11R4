
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr10.h,v 1.1 89/09/21 06:16:28 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr10.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr10.h,v $
 * Revision 1.1  89/09/21  06:16:28  mss
 * Initial revision
 * 
 * Revision 1.8  89/09/11  14:32:48  mss
 * removed leading #
 * 
 * Revision 1.7  89/08/31  14:36:48  annm
 * still fixing realnames
 * 
 * Revision 1.6  89/08/29  13:20:02  annm
 * added real names for too long names
 * 
 * Revision 1.5  89/04/21  18:14:40  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.4  88/09/19  16:54:25  annm
 * added error message formatting, canonical free and ckval routines
 * 
 * Revision 1.3  88/09/06  13:17:46  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/08/26  16:26:16  annm
 * no change
 * 
 * Revision 1.1  88/08/23  14:02:25  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR10
#define TK_INCLUDED_ATTR10

#undef EXTERN

#ifdef TK_GVARS_ATTR10
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for routines in attr10.c come from rnames.h */

#ifndef USE_LONG_NAMES

#define ATTR_CheckId	    gatcid
#define ATTR_CheckTabStop   gatcts
#define ATTR_FreeTabStop    gatfts

#define	VALUE_CkValATValue  gatkat
#define VALUE_CkValCCValue  gatkcc
#define VALUE_CkValDPValue  gatkdp
#define VALUE_CkValRGValue  gatkrg
#define VALUE_CkValValue    gatkvl

#define VALUE_CkValConstituent    gatkco

#define Digits		    gdigit
#define IdChars		    gidchr

#endif

/* declarations for routines in attr10.c */

EXTERN INT_type STRING_Copy();
EXTERN POINTER_type STRING_Alloc();
EXTERN INT_type STRING_Free();
EXTERN INT_type STRING_Set();
EXTERN POINTER_type STRING_Get();
EXTERN INT_type STRING_CkVal();
EXTERN INT_type STRING_Deflt();

EXTERN INT_type ATTR_CheckId();

EXTERN INT_type VALUE_Copy();
EXTERN POINTER_type VALUE_Alloc();
EXTERN INT_type VALUE_Free();
EXTERN INT_type VALUE_Set();
EXTERN POINTER_type VALUE_Get();
EXTERN INT_type VALUE_CkVal();
EXTERN INT_type VALUE_Deflt();

EXTERN INT_type INT_PAIR_Copy();
EXTERN POINTER_type INT_PAIR_Alloc();
EXTERN INT_type INT_PAIR_Free();
EXTERN INT_type INT_PAIR_Set();
EXTERN POINTER_type INT_PAIR_Get();
EXTERN INT_type INT_PAIR_CkVal();
EXTERN INT_type INT_PAIR_Deflt();

EXTERN INT_type SEQUENCE_TAB_STOP_Copy();
EXTERN POINTER_type SEQUENCE_TAB_STOP_Alloc();
EXTERN INT_type SEQUENCE_TAB_STOP_Free();
EXTERN INT_type ATTR_FreeTabStop();
EXTERN INT_type SEQUENCE_TAB_STOP_Set();
EXTERN INT_type SEQUENCE_TAB_STOP_CkVal();

EXTERN INT_type ATTR_CheckTabStop();

EXTERN POINTER_type SEQUENCE_TAB_STOP_Get();

#endif

#endif

