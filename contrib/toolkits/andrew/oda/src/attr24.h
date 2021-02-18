
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr24.h,v 1.1 89/09/21 06:21:22 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr24.h,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr24.h,v $
 * Revision 1.1  89/09/21  06:21:22  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/11  14:35:49  mss
 * removed leading #
 * 
 * Revision 1.2  89/04/21  18:16:45  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.1  89/03/16  15:32:28  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR24
#define TK_INCLUDED_ATTR24

#undef EXTERN

#ifdef TK_GVARS_ATTR24
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */

#ifdef TK_IMPL_ATTR

#ifndef USE_LONG_NAMES

#define ATTR_CopyRevisionHistory	    gatcrh
#define ATTR_CopySequenceRevisers	    gatcsr
#define ATTR_CopyReviser		    gatcrv
#define ATTR_CopySequencePersonalNames	    gatcsp

#define ATTR_AllocRevisionHistory	    gatarh
#define ATTR_AllocSequenceRevisers	    gatasr
#define ATTR_AllocReviser		    gatarv
#define ATTR_AllocSequencePersonalNames	    gatasp

#define ATTR_FreeRevisionHistory	    gatfrh
#define ATTR_FreeReviser		    gatfrv

#endif

EXTERN INT_type SEQUENCE_REVISION_HISTORY_Copy();
EXTERN INT_type ATTR_CopyRevisionHistory();
EXTERN INT_type ATTR_CopySequenceRevisers();
EXTERN INT_type ATTR_CopyReviser();
EXTERN INT_type ATTR_CopySequencePersonalNames();

EXTERN POINTER_type SEQUENCE_REVISION_HISTORY_Alloc();
EXTERN INT_type ATTR_AllocRevisionHistory();
EXTERN POINTER_type ATTR_AllocSequenceRevisers();
EXTERN INT_type ATTR_AllocReviser();
EXTERN POINTER_type ATTR_AllocSequencePersonalNames();

EXTERN INT_type SEQUENCE_REVISION_HISTORY_Free();
EXTERN INT_type ATTR_FreeRevisionHistory();
EXTERN INT_type ATTR_FreeReviser();

EXTERN INT_type SEQUENCE_REVISION_HISTORY_Set();
EXTERN POINTER_type SEQUENCE_REVISION_HISTORY_Get();
EXTERN INT_type SEQUENCE_REVISION_HISTORY_CkVal();
EXTERN INT_type SEQUENCE_REVISION_HISTORY_Deflt();

#endif

#endif
