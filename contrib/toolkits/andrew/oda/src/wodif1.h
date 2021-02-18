
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif1.h,v 1.1 89/09/21 06:44:12 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wodif1.h,v $
 * Revision 1.1  89/09/21  06:44:12  mss
 * Initial revision
 * 
 * Revision 1.14  89/09/11  14:41:15  mss
 * removed leading #
 * 
 * Revision 1.13  89/08/31  15:55:31  annm
 * still fixing real names
 * 
 * Revision 1.12  89/08/30  15:58:16  annm
 * added real names for names that are too long
 * 
 * Revision 1.11  88/09/06  13:22:59  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.10  88/09/06  12:06:55  annm
 * 
 * no change
 * 
 * Revision 1.9  88/08/04  14:27:08  mz0k
 * added copyright notice
 * 
*/

#ifndef TK_INCLUDED_WODIF1
#define TK_INCLUDED_WODIF1

#undef EXTERN

#ifdef TK_GVARS_WODIF1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

EXTERN INT_type WriteODIF();

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_WODIF

#ifndef USE_LONG_NAMES

#define WODIF_Init  gwoint	
#define WODIF_Final gwofnl	

#define WriteProcs  gwowpr

#endif

EXTERN INT_type WODIF_Final();
EXTERN INT_type WODIF_Init();


#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_WODIF

#ifndef USE_LONG_NAMES

#define	SequenceOfGenContents			gwosgc
#define	SequenceOfSpeContents			gwossc
#define	SequenceOfLogClass			gwoslc

#define	WODIF_SequenceConstituents		gwosct
#define	WODIF_PresentationAttributes		gwopat
#define	WODIF_LayoutDirectives			gwoldr
#define	WODIF_DefaultValueListsLogical		gwodvl

#define WODIF_DocumentProfile			gwodpr
#define WODIF_LogicalObjDesc			gwolod
#define WODIF_LogicalObjDescBody		gwoldb
#define WODIF_SpecificLogical			gwoslo
#define WODIF_WalkSpecificLogical		gwogsl

#endif

EXTERN	SEQUENCE_CONSTITUENT_type   SequenceOfLogClass;
EXTERN	SEQUENCE_CONSTITUENT_type   SequenceOfGenContents;
EXTERN	SEQUENCE_CONSTITUENT_type   SequenceOfSpeContents;

EXTERN INT_type WODIF_SequenceConstituents();
EXTERN INT_type WODIF_PresentationAttributes();
EXTERN INT_type WODIF_LayoutDirectives();
EXTERN INT_type WODIF_DefaultValueListsLogical();
#endif

#endif
