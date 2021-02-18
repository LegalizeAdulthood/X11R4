
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr15.h,v 1.1 89/09/21 06:18:13 mss Exp $

$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr15.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr15.h,v $
 * Revision 1.1  89/09/21  06:18:13  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/11  14:33:31  mss
 * removed leading #
 * 
 * Revision 1.2  89/09/01  14:30:27  annm
 * fixing USES_LONG_NAMES problems
 * 
 * Revision 1.1  88/11/01  16:02:51  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR15
#define TK_INCLUDED_ATTR15

#undef EXTERN

#ifdef TK_GVARS_ATTR15
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for most routines in attr15.c come from rnames.h */

#ifndef USE_LONG_NAMES


#endif

/* declarations for routines in attr15.c */

EXTERN INT_type SEQUENCE_FONT_PAIR_CkVal();

#ifndef USE_LONG_NAMES

#define ATTR_FontPairCkVal		gfrkfp
#define ATTR_Ck_Fields			gfrkcf
#define ATTR_CkVal_Dfnpre		gfrkdf
#define ATTR_CkVal_Structured_Name	gfrksn
#define ATTR_CkVal_ISO9541_Id_Type	gfrkis
#define ATTR_CkVal_Int_Type		gfrkit
#define ATTR_CkVal_Int_Pair_Type	gfrkip
#define ATTR_CkVal_Sequence_Int_Type	gfrksi
#define ATTR_CkVal_Writing_Modes_Type	gfrkwm
#define ATTR_CkVal_Modal_Font_Attr_Type	gfrkmf
#define ATTR_CkVal_Tabesc		gfrktb
#define ATTR_CkVal_Scores_Type		gfrkst
#define ATTR_CkVal_Variant_Scripts_Type	gfrkvs
#define ATTR_CkVal_Score_Type		gfrksc
#define ATTR_CkVal_Variant_Script_Type	gfrkvt

#endif

/* declarations for type specific check value canonical procedures */

EXTERN INT_type ATTR_FontPairCkVal();
EXTERN INT_type ATTR_Ck_Fields();
EXTERN INT_type ATTR_CkVal_Dfnpre();
EXTERN INT_type ATTR_CkVal_Structured_Name();
EXTERN INT_type ATTR_CkVal_ISO9541_Id_Type();
EXTERN INT_type ATTR_CkVal_Int_Type();
EXTERN INT_type ATTR_CkVal_Int_Pair_Type();
EXTERN INT_type ATTR_CkVal_Sequence_Int_Type();
EXTERN INT_type ATTR_CkVal_Writing_Modes_Type();
EXTERN INT_type ATTR_CkVal_Modal_Font_Attr_Type();
EXTERN INT_type ATTR_CkVal_Tabesc();
EXTERN INT_type ATTR_CkVal_Scores_Type();
EXTERN INT_type ATTR_CkVal_Variant_Scripts_Type();
EXTERN INT_type ATTR_CkVal_Score_Type();
EXTERN INT_type ATTR_CkVal_Variant_Script_Type();

#endif

#endif
