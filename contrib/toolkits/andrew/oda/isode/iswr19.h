
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/iswr19.h,v 1.1 89/09/21 05:43:23 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/iswr19.h,v $

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

$Log:	iswr19.h,v $
 * Revision 1.1  89/09/21  05:43:23  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:47:48  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/11  14:55:41  mss
 * removed leading #
 * 
 * Revision 1.5  89/08/28  16:48:40  jl3p
 * added copyright notice
 * 
 * Revision 1.4  89/08/16  09:55:34  mss
 * Arranged everything to use global odaiso.h
 * 
 * Revision 1.3  89/08/15  17:07:34  mss
 * Removed procedure renaming for toolkit visible names
 * 
 * Revision 1.2  89/08/11  11:22:36  jl3p
 * added declarations for routines used to fix the ISODE Content Portion 
 * Write routines.
 * 
 * Revision 1.1  89/08/03  15:40:25  mss
 * Initial revision
 * 
 * Revision 1.1  89/06/21  16:52:16  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ISWR19
#define TK_INCLUDED_ISWR19

#undef EXTERN

#ifdef TK_GVARS_ISWR19
#define EXTERN
#else
#define EXTERN extern
#endif

/* declarations that everyone sees */

EXTERN INT_type isode_WriteODIF();

#ifdef TK_IMPL_ISWR

/* real names for procedures */

#ifndef USE_LONG_NAMES

#define ISODE_WriteDocProfile	gidwdp
#define ISODE_WriteStructure	gidwst
#define ISODE_WriteStyles	gidwsy
#define ISODE_WriteText		gidwtx
#define ISODE_FindContent	gidfct
#define ISODE_Addcontent	gidact
#define ISODE_ContentsEqual	gidceq
#define ISODE_MatchIDs		gidmid

#endif

/* declarations seen by the implementer */

EXTERN INT_type ISODE_WriteDocProfile();
EXTERN INT_type ISODE_WriteStructure();
EXTERN INT_type ISODE_WriteStyles();
EXTERN INT_type ISODE_WriteText();
EXTERN INT_type ISODE_FindContent();
EXTERN INT_type ISODE_AddContent();
EXTERN BOOL_type ISODE_ContentsEqual();
EXTERN BOOL_type ISODE_MatchIDs();

#endif

#endif
