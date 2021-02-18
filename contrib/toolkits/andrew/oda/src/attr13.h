
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr13.h,v 1.1 89/09/21 06:17:29 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr13.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr13.h,v $
 * Revision 1.1  89/09/21  06:17:29  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/11  14:33:21  mss
 * removed leading #
 * 
 * Revision 1.3  89/04/21  18:14:57  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.2  88/10/14  16:08:54  annm
 * copy support for font monster - also empty ckvals
 * 
 * Revision 1.1  88/09/30  15:44:49  annm
 * Initial revision
 * 

*/


#ifndef TK_INCLUDED_ATTR13
#define TK_INCLUDED_ATTR13

#undef EXTERN

#ifdef TK_GVARS_ATTR13
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for most routines in attr13.c come from rnames.h */

#ifndef USE_LONG_NAMES

#define ATTR_AllocFontPairBody	    gatfpb
#define ATTR_AllocStructuredName    gatasn
#define ATTR_AllocISO9541Id	    gatiid

#endif

/* declarations for routines in attr13.c */

EXTERN POINTER_type SEQUENCE_FONT_PAIR_Alloc();
EXTERN INT_type SEQUENCE_FONT_PAIR_Free();
EXTERN INT_type SEQUENCE_FONT_PAIR_Set();
EXTERN POINTER_type SEQUENCE_FONT_PAIR_Get();

EXTERN INT_type ATTR_AllocFontPairBody();
EXTERN INT_type ATTR_AllocStructuredName();
EXTERN INT_type ATTR_AllocISO9541Id();

#endif

#endif
