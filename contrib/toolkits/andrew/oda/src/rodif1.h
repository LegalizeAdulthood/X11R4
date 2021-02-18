
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif1.h,v 1.1 89/09/21 06:37:41 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rodif1.h,v $
 * Revision 1.1  89/09/21  06:37:41  mss
 * Initial revision
 * 
 * Revision 1.17  89/09/11  14:39:59  mss
 * removed leading #
 * 
 * Revision 1.16  89/08/31  23:09:45  mss
 * GOt rid fo redundant definition of RODIF_LayoutObject
 * 
 * Revision 1.15  89/08/31  22:11:27  mss
 * Changed rodif1.h to use distinct mapping names
 * 
 * Revision 1.14  89/08/30  16:29:18  annm
 * real names now start with g
 * 
 * Revision 1.13  89/08/29  17:22:58  annm
 * added short real names for names that are too long
 * 
 * Revision 1.12  88/10/26  19:03:33  mz0k
 * RODIF_SequenceInts(Bytes) --> RODIF_GetSequenceInts(Bytes)
 * 
 * Revision 1.11  88/10/07  11:20:31  mz0k
 * 'ifdef USE_LONG_NAMES' --> 'ifndef USE_LONG_NAMES'
 * 
 * Revision 1.10  88/09/13  14:59:29  mz0k
 * changed "" to <> & forward referenced constituents
 * 
 * Revision 1.9  88/09/07  14:43:43  mz0k
 * Started coding.
 * 
 * Revision 1.8  88/06/08  16:58:12  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.7  88/06/08  14:37:20  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_RODIF1
#define TK_INCLUDED_RODIF1

#undef EXTERN

#ifdef TK_GVARS_RODIF1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

EXTERN DOCUMENT_type ReadODIF();


/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_RODIF

#ifndef USE_LONG_NAMES

#define RODIF_Init  grdint	
#define RODIF_Final grdfnl	

#define RODIF_CharacterAttributes	grocat
#define RODIF_LogObjDescriptorBody	groldb

#define Subordinates			grosub
    
#define identifier			groidn
	
#define realconstituent			grorco

#endif

EXTERN INT_type RODIF_Final();
EXTERN INT_type RODIF_Init();

#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_RODIF

#ifndef USE_LONG_NAMES

#define	RODIF_SeqTableStop		    grosts
#define	RODIF_GetSequenceInt		    grogsi
#define	RODIF_GetSequenceByte		    grogsb
#define	RODIF_SequenceIntPair		    grosip
#define	RODIF_SequenceConstituent	    grosct
#define	RODIF_PresentationAttributes	    gropat
#define	RODIF_LogicalObject		    grolgo

#endif



EXTERN INT_type hashit();
EXTERN INT_type	RODIF_SeqTableStop();
EXTERN INT_type RODIF_GetSequenceInt();
EXTERN INT_type	RODIF_GetSequenceByte();
EXTERN INT_type RODIF_SequenceIntPair();
EXTERN SEQUENCE_CONSTITUENT_type RODIF_SequenceConstituent();
EXTERN INT_type	RODIF_PresentationAttributes();
EXTERN INT_type	RODIF_LogicalObject();

#endif

#endif
