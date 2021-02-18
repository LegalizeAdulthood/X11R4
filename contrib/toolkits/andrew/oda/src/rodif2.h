
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif2.h,v 1.1 89/09/21 06:38:03 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif2.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rodif2.h,v $
 * Revision 1.1  89/09/21  06:38:03  mss
 * Initial revision
 * 
 * Revision 1.9  89/09/11  14:40:13  mss
 * removed leading #
 * 
 * Revision 1.8  89/08/31  22:17:36  mss
 * Made unique short nams
 * 
 * Revision 1.7  89/08/30  16:29:29  annm
 * real names now start with g
 * 
 * Revision 1.6  89/08/29  17:26:47  annm
 * added short real names for names that are too long
 * 
 * Revision 1.5  88/10/07  13:43:58  mz0k
 * rdfloc --> rdflgc for the RODIF_LogicalObjectClass
 * 
 * Revision 1.4  88/10/07  11:21:11  mz0k
 * 'ifdef USE_LONG_NAMES' --> 'ifndef USE_LONG_NAMES'
 * 
 * Revision 1.3  88/09/27  18:23:02  mz0k
 * added the RODIF_GenForSubordinates
 * 
 * Revision 1.2  88/09/22  14:55:51  mz0k
 * MSS checked in initially working version w/
 * some attributes
 * 
 * Revision 1.1  88/09/07  14:51:20  mz0k
 * Initial revision
 * 
*/


#ifndef TK_INCLUDED_RODIF2
#define TK_INCLUDED_RODIF2

#undef EXTERN

#ifdef TK_GVARS_RODIF2
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

#ifdef TK_RODIF


#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_RODIF

#ifndef USE_LONG_NAMES

#define	RODIF_DefaultValueListsLogical	    grodvl
#define	RODIF_ContentPortions		    grocpt
#define	RODIF_LogicalObjectClass	    groloc
#define	RODIF_GenForSubordinates	    grogfs
#define	RODIF_ConstrExpression		    grocep
#define	RODIF_ConstrTerm		    groctm
#define	RODIF_ConstrFactor		    grocft

#define RODIF_LogicalObjClassBody	    grolcl

#define Contents			    grocon

#define GenForSub			    grogsu

#define contids				    grocid



#endif

EXTERN INT_type RODIF_DefaultValueListsLogical();
EXTERN INT_type RODIF_ContentPortions();
EXTERN INT_type RODIF_LogicalObjectClass();
EXTERN INT_type RODIF_GenForSubordinates();
EXTERN INT_type RODIF_ConstrExpression();
EXTERN INT_type RODIF_ConstrTerm();
EXTERN INT_type RODIF_ConstrFactor();

#endif

#endif
