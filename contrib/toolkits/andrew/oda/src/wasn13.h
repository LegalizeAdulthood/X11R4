     
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn13.h,v 1.2 89/10/31 14:56:59 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn13.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn13.h,v $
 * Revision 1.2  89/10/31  14:56:59  jr
 * Casting problems.
 * 
 * Revision 1.1  89/09/21  06:42:48  mss
 * Initial revision
 * 
 * Revision 1.9  89/08/31  15:30:18  annm
 * still fixing real names
 * 
 * Revision 1.8  89/08/30  15:48:39  annm
 * added real names for names that are too long
 * 
 * Revision 1.7  88/10/21  16:41:51  mz0k
 * added WASN1_Real
 * added WASN1_SeqBytesInStruct
 * 
 * Revision 1.6  88/09/22  14:56:22  mz0k
 * MSS checked in initially working version w/
 * some attributes
 * 
 * Revision 1.5  88/09/06  13:22:46  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/08/04  14:24:44  mz0k
 * added copyright notice
 * 
*/

#ifndef TK_INCLUDED_WASN13
#define TK_INCLUDED_WASN13

#undef EXTERN

#ifdef TK_GVARS_WASN13
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_WASN13




#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_WASN1

#ifndef USE_LONG_NAMES

#define	WASN1_IntPair			gwsnip
#define	WASN1_Real			gwsnre
#define	WASN1_IntType			gwsnit
#define	WASN1_LineLayoutTable		gwsnll
#define	WASN1_GraphicRendition		gwsngr
#define	WASN1_CharacterFonts		gwsncf
#define	WASN1_Itemisation		gwsnim
#define	WASN1_ContentPortionAttributes	gwsncp
#define	WASN1_SequenceBytes		gwasby
#define	WASN1_SeqBytesInStruct		gwsnsb
#define	WASN1_Asn1ObjectId		gwsnao
#define	WASN1_Border			gwsnbd

#define WASN1_SequenceStrings		gwasst

#define reference			gwaref

#endif

EXTERN INT_type	    WASN1_IntPair();
EXTERN INT_type	    WASN1_Real();
EXTERN INT_type	    WASN1_IntType();
EXTERN INT_type	    WASN1_LineLayoutTable();
EXTERN INT_type	    WASN1_GraphicRendition();
EXTERN INT_type	    WASN1_CharacterFonts();
EXTERN INT_type	    WASN1_Itemisation();
EXTERN INT_type	    WASN1_ContentPortionAttributes();
EXTERN INT_type	    WASN1_SequenceBytes();
EXTERN INT_type	    WASN1_SeqBytesInStruct();
EXTERN INT_type	    WASN1_Asn1ObjectId();
EXTERN INT_type	    WASN1_Border();

EXTERN INT_type	    WASN1_SequenceStrings();

#endif

#endif
