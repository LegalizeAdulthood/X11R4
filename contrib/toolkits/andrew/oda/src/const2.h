
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const2.h,v 1.1 89/09/21 06:27:23 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const2.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	const2.h,v $
 * Revision 1.1  89/09/21  06:27:23  mss
 * Initial revision
 * 
 * Revision 1.8  89/08/30  17:08:13  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.7  89/01/17  16:57:43  annm
 * changes for DVL defaulting
 * 
 * Revision 1.6  88/09/06  13:19:15  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.5  88/09/06  12:04:14  annm
 * 
 * no change
 * 
 * Revision 1.4  88/08/25  16:13:26  annm
 * added remove from document routines for support
 * for fill in constituent, collaspe content portions
 * and delete constituent
 * 
 * Revision 1.3  88/06/08  16:55:54  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.2  88/06/08  14:35:02  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_CONST2
#define TK_INCLUDED_CONST2

#undef EXTERN

#ifdef TK_GVARS_CONST2
#define EXTERN
#else
#define EXTERN extern
#endif

/* Put stuff that everyone sees here */

#ifdef TK_IMPL_CONST

/* Put stuff that only the toolkit sees here */

#ifndef USE_LONG_NAMES

#define CONST_SetComponentWhatKind	gcswk
#define CONST_DoSetComponentWhatKind	gcdswk
#define CONST_SetOtherFields		gcsofs
#define CONST_InsertConstituent		gcsics
#define CONST_AddSpecialConstituent	gcsasc
#define CONST_AddDoublyLinked		gcsadl
#define CONST_CleanUpConstituent	gcscuc
#define CONST_RemoveConstituent		gcsrct
#define CONST_RemoveDoublyLinked	gcsrdl
#define CONST_RemoveSpecialConstituent	gcsrsc
#define CONST_SetDefComponentWhatKind	gcsdwk
#define CONST_SetIndepWhatKind		gcsiwk
#define CONST_SetDepWhatKind		gcspwk

#endif

EXTERN INT_type CONST_SetComponentWhatKind();
EXTERN INT_type CONST_DoSetComponentWhatKind();
EXTERN INT_type CONST_SetOtherFields();
EXTERN INT_type CONST_InsertConstituent();
EXTERN INT_type CONST_AddSpecialConstituent();
EXTERN INT_type CONST_AddDoublyLinked();
EXTERN INT_type CONST_CleanUpConstituent();
EXTERN INT_type CONST_RemoveConstituent();
EXTERN INT_type CONST_RemoveDoublyLinked();
EXTERN INT_type CONST_RemoveSpecialConstituent();
EXTERN INT_type CONST_SetDefComponentWhatKind();
EXTERN INT_type CONST_SetIndepWhatKind();
EXTERN INT_type CONST_SetDepWhatKind();

#endif


#endif
