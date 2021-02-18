
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr12.h,v 1.1 89/09/21 06:17:09 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr12.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr12.h,v $
 * Revision 1.1  89/09/21  06:17:09  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/11  14:33:11  mss
 * removed leading #
 * 
 * Revision 1.5  88/09/21  15:45:51  annm
 * added ckval and free canonical procedures - also added the tkerr stuff
 * 
 * Revision 1.4  88/09/12  12:59:30  annm
 * added support for rg_IMAGE_DIMENSIONS_type
 * 
 * Revision 1.3  88/09/08  15:34:59  annm
 * added support for rg_PEL_SPACING_type and rg_CLIPPING_type
 * 
 * Revision 1.2  88/09/08  08:16:51  annm
 * added real name for the one auxillary procedure for at_DIMENSIONS type
 * 
 * Revision 1.1  88/09/07  18:35:21  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR12
#define TK_INCLUDED_ATTR12

#undef EXTERN

#ifdef TK_GVARS_ATTR12
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for most routines in attr12.c come from rnames.h */

/* here are real names for auxillary routines */

#ifndef USE_LONG_NAMES

#define DIMENSION_type_Copy	    gatdtc
#define DIMENSION_type_CkVal	    gatdck

#endif

/* declarations for routines in attr12.c */

EXTERN INT_type at_DIMENSIONS_Copy();
EXTERN INT_type DIMENSION_type_Copy();
EXTERN POINTER_type at_DIMENSIONS_Alloc();
EXTERN INT_type at_DIMENSIONS_Free();
EXTERN INT_type at_DIMENSIONS_Set();
EXTERN POINTER_type at_DIMENSIONS_Get();
EXTERN INT_type at_DIMENSIONS_CkVal();
EXTERN INT_type DIMENSION_type_CkVal();
EXTERN INT_type at_DIMENSIONS_Deflt();

EXTERN INT_type rg_CLIPPING_Copy();
EXTERN POINTER_type rg_CLIPPING_Alloc();
EXTERN INT_type rg_CLIPPING_Free();
EXTERN INT_type rg_CLIPPING_Set();
EXTERN POINTER_type rg_CLIPPING_Get();
EXTERN INT_type rg_CLIPPING_CkVal();
EXTERN INT_type rg_CLIPPING_Deflt();

EXTERN INT_type rg_CLIPPING_Copy();
EXTERN POINTER_type rg_PEL_SPACING_Alloc();
EXTERN INT_type rg_PEL_SPACING_Free();
EXTERN INT_type rg_PEL_SPACING_Set();
EXTERN POINTER_type rg_PEL_SPACING_Get();
EXTERN INT_type rg_PEL_SPACING_CkVal();
EXTERN INT_type rg_PEL_SPACING_Deflt();

EXTERN INT_type rg_IMAGE_DIMENSIONS_Copy();
EXTERN POINTER_type rg_IMAGE_DIMENSIONS_Alloc();
EXTERN INT_type rg_IMAGE_DIMENSIONS_Free();
EXTERN INT_type rg_IMAGE_DIMENSIONS_Set();
EXTERN POINTER_type rg_IMAGE_DIMENSIONS_Get();
EXTERN INT_type rg_IMAGE_DIMENSIONS_CkVal();
EXTERN INT_type rg_IMAGE_DIMENSIONS_Deflt();

#endif

#endif
