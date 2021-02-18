
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn12.h,v 1.1 89/09/21 06:36:23 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn12.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rasn12.h,v $
 * Revision 1.1  89/09/21  06:36:23  mss
 * Initial revision
 * 
 * Revision 1.12  89/08/31  15:10:38  annm
 * still fixing real names
 * 
 * Revision 1.11  89/08/30  16:18:20  annm
 * fixed real names to start with g
 * 
 * Revision 1.10  89/08/29  17:12:34  annm
 * added short real names for names that are too long
 * 
 * Revision 1.9  88/11/17  15:04:59  mz0k
 * added RASN1_Separation
 * 
 * Revision 1.8  88/11/03  10:48:38  mz0k
 * Added copyright
 * 
*/

#ifndef TK_INCLUDED_RASN12
#define TK_INCLUDED_RASN12

#undef EXTERN

#ifdef TK_GVARS_RASN12
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


#ifdef TK_RASN1


#endif
/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_RASN1

#ifndef USE_LONG_NAMES

#define	RASN1_Asn1ObjectId		    graaoi
#define	RASN1_TabStop			    gratst
#define	RASN1_FontType			    grafty
#define	RASN1_SaveSeqStrings		    grasss
#define	RASN1_CompLogicalAttributes	    gracla
#define	RASN1_String2Constituent	    gras2c
#define	RASN1_BasicLogicalAttributes	    grabla
#define	RASN1_DefaultPresAttributes	    gradpa
#define	RASN1_ContentAttributes		    gracat
#define	RASN1_Offset			    graoff
#define	RASN1_Separation		    grasep

#define RANS1_DefaultPresAttributes	    gradpa
#define TheContent			    gratcn
#define addnode				    graand

#define Identical			    graide

#endif

EXTERN CONSTITUENT		TheContent; /* content portion */

EXTERN INT_type	    RASN1_Asn1ObjectId();
EXTERN INT_type	    RASN1_TabStop();
EXTERN INT_type	    RASN1_FontType();
EXTERN SEQUENCE_STRING_type RASN1_SaveSeqStrings();
EXTERN INT_type	    RASN1_CompLogicalAttributes();
EXTERN INT_type	    RASN1_String2Constituent();
EXTERN INT_type	    RASN1_BasicLogicalAttributes();
EXTERN INT_type	    RASN1_DefaultPresAttributes();
EXTERN INT_type	    RASN1_ContentAttributes();
EXTERN HASH_type    *lookup();
EXTERN INT_type	    addnode();
EXTERN BOOL_type    Identical();
EXTERN INT_type	    RASN1_Offset();
EXTERN INT_type	    RASN1_Separation();

#endif


#endif
