
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr23.h,v 1.1 89/09/21 06:21:02 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr23.h,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr23.h,v $
 * Revision 1.1  89/09/21  06:21:02  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/11  14:35:35  mss
 * removed leading #
 * 
 * Revision 1.3  89/03/16  15:28:44  annm
 * changes for SEQUENCE_REVISION_HISTORY_type and at_SAME_LAYOUT_OBJECT_type
 * 
 * Revision 1.2  89/03/09  15:52:16  annm
 * added aunion.h canonical procedures for the new dp_DOC_APP_PROF_DEFAULTS_type
 * 
 * Revision 1.1  89/02/09  17:45:28  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR23
#define TK_INCLUDED_ATTR23

#undef EXTERN

#ifdef TK_GVARS_ATTR23
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */

#ifdef TK_IMPL_ATTR

#ifndef USE_LONG_NAMES

#endif

EXTERN INT_type cc_ITEMISATION_Copy();
EXTERN POINTER_type cc_ITEMISATION_Alloc();
EXTERN INT_type cc_ITEMISATION_Free();
EXTERN INT_type cc_ITEMISATION_Set();
EXTERN POINTER_type cc_ITEMISATION_Get();
EXTERN INT_type cc_ITEMISATION_CkVal();
EXTERN INT_type cc_ITEMISATION_Deflt();

EXTERN INT_type dp_DOC_APP_PROF_DEFAULTS_Copy();
EXTERN POINTER_type dp_DOC_APP_PROF_DEFAULTS_Alloc();
EXTERN INT_type dp_DOC_APP_PROF_DEFAULTS_Free();
EXTERN INT_type dp_DOC_APP_PROF_DEFAULTS_Set();
EXTERN POINTER_type dp_DOC_APP_PROF_DEFAULTS_Get();
EXTERN INT_type dp_DOC_APP_PROF_DEFAULTS_CkVal();
EXTERN INT_type dp_DOC_APP_PROF_DEFAULTS_Deflt();

#endif

#endif


