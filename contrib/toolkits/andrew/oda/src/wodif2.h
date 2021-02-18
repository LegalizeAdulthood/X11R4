
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif2.h,v 1.1 89/09/21 06:44:38 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif2.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wodif2.h,v $
 * Revision 1.1  89/09/21  06:44:38  mss
 * Initial revision
 * 
 * Revision 1.9  89/09/11  14:41:29  mss
 * removed leading #
 * 
 * Revision 1.8  89/09/01  10:24:45  mss
 * Made name mapping unique
 * 
 * Revision 1.7  89/08/30  16:10:25  annm
 * added real names for names that are too long
 * 
 * 
 * Revision 1.6  89/08/30  16:02:27  annm
 * added real names for names that are too long
 * 
 * Revision 1.5  88/10/07  11:19:39  mz0k
 * USE_LONG_NAME --> USE_LONG_NAMES
 * 
 * Revision 1.4  88/09/06  13:23:04  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.3  88/08/04  14:27:58  mz0k
 * added copyright notice
 * 
*/

#ifndef TK_INCLUDED_WODIF2
#define TK_INCLUDED_WODIF2

#undef EXTERN

#ifdef TK_GVARS_WODIF2
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

#ifdef TK_WODIF

#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_WODIF

#ifndef USE_LONG_NAMES

#define	WODIF_CharacterAttributes	    gwocha
#define	WODIF_SpecificContent		    gwoscn
#define	WODIF_GenericContent		    gwogct
#define	WODIF_PresentationStyle		    gwopst
#define	WODIF_LayoutStyle		    gwolst
#define	WODIF_GenericLogical		    gwoglg
#define	WODIF_WalkConstructionExpr	    gwowce
#define	WODIF_WalkConstructionTerm	    gwowct
#define	WODIF_WriteGenerator		    gwowge
#define	WODIF_WriteConstructionExpr	    gworce
#define	WODIF_WriteConstructionTerm	    gworct
#define	WODIF_WriteConstructionFactor	    gworcf

#define WODIF_ContentPortions		    gwocpt
#define WODIF_LayoutStyleDescriptor	    gwolsd
#define WODIF_LogicalObjClassDesc	    gwoloc
#define WODIF_LogicalObjClassDescBody	    gwolcb
#define WODIF_PresentationStyleDesc	    gwopsd
#define WODIF_WalkGenericLogical	    gwowgl

#endif

EXTERN INT_type			WODIF_CharacterAttributes();
EXTERN INT_type			WODIF_SpecificContent();
EXTERN INT_type			WODIF_GenericContent();
EXTERN INT_type			WODIF_PresentationStyle();
EXTERN INT_type			WODIF_LayoutStyle();
EXTERN INT_type			WODIF_GenericLogical();
EXTERN INT_type			WODIF_WalkConstructionExpr();
EXTERN INT_type			WODIF_WalkConstructionTerm();
EXTERN INT_type			WODIF_WriteGenerator();
EXTERN INT_type			WODIF_WriteConstructionExpr();
EXTERN INT_type			WODIF_WriteConstructionTerm();
EXTERN INT_type			WODIF_WriteConstructionFactor();


#endif

#endif
