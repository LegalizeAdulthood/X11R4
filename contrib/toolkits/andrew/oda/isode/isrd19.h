
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/isrd19.h,v 1.1 89/09/21 05:35:19 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/isrd19.h,v $

 *
 * (C) Copyright 1989 by Carnegie Mellon University
 *
 * Permission to use, copy, modify, and distribute these programs
 * and their documentation for any purpose and without fee is
 * hereby granted, provided that this copyright and permission
 * notice appear on all copies and supporting documentation, and
 * that the name of Carnegie Mellon University not be used in
 * advertising or publicity pertaining to distribution of the
 * programs without specific prior permission and notice be given
 * in supporting documentation that copying and distribution is
 * by permission of Carnegie Mellon University.
 *
 * Carnegie Mellon University makes no representations about the
 * suitability of this software for any purpose.  It is provided
 * as is, without express or implied warranty.
 *
 * Software by Ann Marks and James T. Lui,
 * Information Technology Center, Carnegie Mellon University,
 * except where noted.
 *

$Log:	isrd19.h,v $
 * Revision 1.1  89/09/21  05:35:19  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:39:05  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/11  14:55:06  mss
 * removed leading #
 * 
 * Revision 1.4  89/08/28  16:51:58  jl3p
 * added copyright notice
 * 
 * Revision 1.3  89/08/16  09:55:29  mss
 * Arranged everything to use global odaiso.h
 * 
 * Revision 1.2  89/08/15  17:08:09  mss
 * Removed procedure renaming for toolkit visible names
 * 
 * Revision 1.1  89/08/03  15:45:58  mss
 * Initial revision
 * 
 * Revision 1.1  89/06/21  17:37:47  annm
 * Initial revision
 * 
 * Revision 1.1  89/06/21  16:55:18  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ISRD19
#define TK_INCLUDED_ISRD19

#undef EXTERN

#ifdef TK_GVARS_ISRD19
#define EXTERN
#else
#define EXTERN extern
#endif

/* declarations that everyone sees */


EXTERN DOCUMENT_type isode_ReadODIF();

#ifdef TK_IMPL_ISRD

/* real names for procedures */

#ifndef USE_LONG_NAMES

#define ISODE_Hash			gishsh
#define ISODE_HashInit			gishhi
#define ISODE_LookUpId			gislui
#define ISODE_STRING_Equal		gisseq
#define ISODE_GetHashStructure		gisghs
#define ISODE_AddToHash			gisath
#define ISODE_FillInConstituent		gisfic
#define ISODE_MakeSequenceConstituent	gismsc

#endif

EXTERN INT_type ISODE_Hash();
EXTERN INT_type ISODE_HashInit();
EXTERN struct hash_t *ISODE_LookUpId();
EXTERN BOOL_type ISODE_STRING_Equal();
EXTERN struct hash_t *ISODE_GetHashStructure();
EXTERN INT_type ISODE_AddToHash();
EXTERN INT_type ISODE_FillInConstituent();
EXTERN SEQUENCE_CONSTITUENT_type ISODE_MakeSequenceConstituent();

#endif

#endif
