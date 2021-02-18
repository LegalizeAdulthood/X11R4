
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const3.h,v 1.1 89/09/21 06:27:42 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const3.h,v $


(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	const3.h,v $
 * Revision 1.1  89/09/21  06:27:42  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/11  14:30:15  mss
 * removed leading #
 * 
 * Revision 1.4  88/09/30  15:41:05  annm
 * fixing VMS bugs
 * 
 * Revision 1.3  88/09/28  15:18:43  annm
 * added support for moving the old empty from one document list to another on a FillInConstituent
 * 
 * Revision 1.2  88/09/06  13:19:20  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.1  88/08/25  16:14:04  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_CONST3
#define TK_INCLUDED_CONST3

#undef EXTERN

#ifdef TK_GVARS_CONST3
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */


#ifdef TK_IMPL_CONST

#ifndef USE_LONG_NAMES

#define CONST_MakeConstituent		    gcsmct
#define CONST_FillInConstituent		    gcsfic
#define CONST_CollaspeContentPortions	    gcsccp
#define CONST_Chameleon			    gcscha
#define CONST_ChangeContentPortionRefs	    gcsccr
#define CONST_MoveConstituent		    gcsmcs

#endif

EXTERN CONSTITUENT CONST_MakeConstituent();
EXTERN INT_type CONST_FillInConstituent();
EXTERN INT_type CONST_CollaspeContentPortions();
EXTERN INT_type CONST_Chameleon();
EXTERN INT_type CONST_ChangeContentPortionRefs();
EXTERN INT_type CONST_MoveConstituent();

#endif


#endif

