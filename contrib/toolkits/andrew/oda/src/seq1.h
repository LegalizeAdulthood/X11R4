
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/seq1.h,v 1.2 89/10/31 12:51:40 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/seq1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	seq1.h,v $
 * Revision 1.2  89/10/31  12:51:40  jr
 * Missing declaration of DeleteSequenceDeeply
 * 
 * Revision 1.1  89/09/21  06:39:25  mss
 * Initial revision
 * 
 * Revision 1.28  89/09/01  14:24:08  annm
 * fixing USES_LONG_NAMES problems
 * 
 * Revision 1.27  89/08/29  17:41:08  annm
 * added short real names for names that are too long
 * 
 * Revision 1.26  89/03/27  12:35:51  mss
 * Made undef'ing of EXTERN conditional on it being
 * already defined.
 * 
 * Revision 1.25  89/03/16  15:30:26  annm
 * changes for SEQUENCE_REVISION_HISTORY_type and at_SAME_LAYOUT_OBJECT_type
 * 
 * Revision 1.24  88/12/13  19:56:43  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.23  88/09/06  14:54:39  annm
 * no change
 * 
 * Revision 1.22  88/09/06  12:06:26  annm
 * 
 * no change
 * 
 * Revision 1.21  88/08/16  02:52:18  mss
 * Added SEQ_MakeSequence
 * 
 * Revision 1.20  88/08/03  13:23:01  jr
 * Declare SEQ_CopySeqElts2
 * 
 * Revision 1.19  88/07/26  13:32:50  jr
 * Add appendsequencew
 * 
 * Revision 1.18  88/07/15  18:39:56  annm
 * made CopySequenceElements visible outside the SEQ module
 * 
 * Revision 1.17  88/06/17  15:07:28  annm
 * wrote ContractSequence; buffed rough edges in ExpandSequence (TKError setting on can't realloc); can not copy sequences of resource maps
 * 
 * Revision 1.16  88/06/08  16:58:27  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.15  88/06/08  14:37:33  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_SEQ1
#define TK_INCLUDED_SEQ1

#ifdef EXTERN
#undef EXTERN
#endif

#ifdef TK_GVARS_SEQ1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

EXTERN INT_type		AppendSequence();
EXTERN INT_type		ContractSequence();
EXTERN SEQUENCE_type	CopySequence();
EXTERN INT_type		DeleteSequenceDeeply();
EXTERN INT_type		DeleteSequence();
EXTERN INT_type		ExpandSequence();
EXTERN SEQUENCE_type	MakeSequence();

/* The variable for the error string */

#ifndef USE_LONG_NAMES

#define ERROR_SEQUENCE	gerseq

#else

#define ERROR_SEQUENCE Error_Sequence

#endif

EXTERN SEQUENCE_type ERROR_SEQUENCE;

#ifndef USE_LONG_NAMES

#define NULL_SEQUENCE	gnlseq

#else

#define NULL_SEQUENCE Null_Sequence

#endif

EXTERN SEQUENCE_type	NULL_SEQUENCE;

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_SEQ

#ifndef USE_LONG_NAMES

#define SEQ_Init    gsqint
#define SEQ_Final   gsqfnl

#define SEQ_CopySequenceElements  gsqcse
#define SEQ_CopySeqElts2	  gsqcp2
#define SEQ_MakeSequence	  gsqmks

#define GetSeqValueField	  gsqgsv
#define SEQ_CopyTabStop		  gsqcts
#define SequenceElementSize	  gsqses  
#define SetSeqValueField	  gsqsvf

#define error_sequence		  gsqerr
#define null_sequence		  gsqnul

#endif

EXTERN INT_type SEQ_Final();
EXTERN INT_type SEQ_Init();

EXTERN INT_type SEQ_CopySequenceElements();
EXTERN INT_type SEQ_CopySeqElts2();
EXTERN SEQUENCE_type SEQ_MakeSequence(/*type, length, dataPtr*/);
#endif

/*
    Declarations seen within the SEQ module go here.
*/

#ifdef TK_IMPL_SEQ

#endif

#endif
