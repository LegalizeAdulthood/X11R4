
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn12.h,v 1.1 89/09/21 06:42:25 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn12.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn12.h,v $
 * Revision 1.1  89/09/21  06:42:25  mss
 * Initial revision
 * 
 * Revision 1.8  89/09/01  10:24:31  mss
 * Made name mapping unique
 * 
 * Revision 1.7  89/08/31  15:30:13  annm
 * still fixing real names
 * 
 * Revision 1.6  89/08/30  15:44:20  annm
 * added real names for names that are too long
 * 
 * Revision 1.5  88/09/06  13:22:40  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/08/26  16:34:48  mz0k
 * Fixed name too long: WASN1_CompositeLogicalAttributes
 * 
 * Revision 1.3  88/08/04  14:24:41  mz0k
 * added copyright notice
 * 
*/

#ifndef TK_INCLUDED_WASN12
#define TK_INCLUDED_WASN12

#undef EXTERN

#ifdef TK_GVARS_WASN12
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

#ifdef TK_WASN1


#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_WASN1

#ifndef USE_LONG_NAMES

#define	WASN1_Strings			    gwsnsg
#define	WASN1_Header			    gwsnhd
#define	WASN1_Constituent_LastId	    gwsncl
#define	WASN1_ConstituentId		    gwsnci
#define	WASN1_Binding_Pair		    gwsnbg
#define	WASN1_Separation		    gwsnsp
#define	WASN1_Offset			    gwsnof
#define	WASN1_LayoutObjectType		    gwsnlo
#define	WASN1_CompLogicalAttributes	    gwsnca
#define	WASN1_BasicLogicalAttributes	    gwsnbl

#define WASN1_ConsitutentId		    gwacid
#define WASN1_LayoutOjbectType		    gwalot

#endif

EXTERN INT_type	    WASN1_Strings();
EXTERN INT_type	    WASN1_Header();
EXTERN INT_type	    WASN1_Constituent_LastId();
EXTERN INT_type	    WASN1_ConstituentId();
EXTERN INT_type	    WASN1_Binding_Pair();
EXTERN INT_type	    WASN1_Separation();
EXTERN INT_type	    WASN1_Offset();
EXTERN INT_type	    WASN1_LayoutObjectType();
EXTERN INT_type	    WASN1_CompLogicalAttributes();
EXTERN INT_type	    WASN1_BasicLogicalAttributes();

#endif

#endif
