
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr11.h,v 1.1 89/09/21 06:16:47 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr11.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr11.h,v $
 * Revision 1.1  89/09/21  06:16:47  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/11  14:32:59  mss
 * removed leading #
 * 
 * Revision 1.5  89/08/30  16:58:00  annm
 * no change
 * 
 * Revision 1.4  88/09/21  15:45:28  annm
 * added ckval and free canonical procedures - also added the tkerr stuff
 * 
 * Revision 1.3  88/09/06  13:17:51  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/09/01  14:17:11  annm
 * added canonical procedures for at_POSITION
 * 
 * Revision 1.1  88/08/26  16:29:30  annm
 * Initial revision
 *  
*/

#ifndef TK_INCLUDED_ATTR11
#define TK_INCLUDED_ATTR11

#undef EXTERN

#ifdef TK_GVARS_ATTR11
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for routines in attr11.c come from rnames.h */


/* declarations for routines in attr11.c */

EXTERN INT_type SEQUENCE_INT_PAIR_Copy();
EXTERN POINTER_type SEQUENCE_INT_PAIR_Alloc();
EXTERN INT_type SEQUENCE_INT_PAIR_Free();
EXTERN INT_type SEQUENCE_INT_PAIR_Set();
EXTERN POINTER_type SEQUENCE_INT_PAIR_Get();
EXTERN INT_type SEQUENCE_INT_PAIR_CkVal();
EXTERN INT_type SEQUENCE_INT_PAIR_Deflt();

EXTERN INT_type dp_ODA_VERSION_Copy();
EXTERN POINTER_type dp_ODA_VERSION_Alloc();
EXTERN INT_type dp_ODA_VERSION_Free();
EXTERN INT_type dp_ODA_VERSION_Set();
EXTERN POINTER_type dp_ODA_VERSION_Get();
EXTERN INT_type dp_ODA_VERSION_CkVal();
EXTERN INT_type dp_ODA_VERSION_Deflt();

EXTERN INT_type at_POSITION_Copy();
EXTERN POINTER_type at_POSITION_Alloc();
EXTERN INT_type at_POSITION_Free();
EXTERN INT_type at_POSITION_Set();
EXTERN POINTER_type at_POSITION_Get();
EXTERN INT_type at_POSITION_CkVal();
EXTERN INT_type at_POSITION_Deflt();

#endif

#endif

