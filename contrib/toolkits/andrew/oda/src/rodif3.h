
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif3.h,v 1.1 89/09/21 06:38:28 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rodif3.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rodif3.h,v $
 * Revision 1.1  89/09/21  06:38:28  mss
 * Initial revision
 * 
 * Revision 1.8  89/09/11  14:40:25  mss
 * removed leading #
 * 
 * Revision 1.7  89/08/31  22:58:55  mss
 * fixed typos in layoutobject
 * 
 * Revision 1.6  89/08/31  22:17:52  mss
 * Made unique short nams
 * 
 * Revision 1.5  89/08/30  16:30:02  annm
 * real names now start with g
 * 
 * Revision 1.4  89/08/29  17:29:19  annm
 * added short real names for names that are too long
 * 
 * Revision 1.3  88/10/21  12:28:55  mz0k
 * moved RODIF_DocumentProfile to rodif4.h
 * 
 * Revision 1.2  88/10/07  11:21:17  mz0k
 * 'ifdef USE_LONG_NAMES' --> 'ifndef USE_LONG_NAMES'
 * 
 * Revision 1.1  88/09/07  14:51:23  mz0k
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_RODIF3
#define TK_INCLUDED_RODIF3

#undef EXTERN

#ifdef TK_GVARS_RODIF3
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

#ifdef TK_TKI_RODIF


#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_RODIF

#ifndef USE_LONG_NAMES

#define	RODIF_PresentationStyle		gropst
#define	RODIF_LayoutObjectClass		groyoc
#define	RODIF_LayoutStyle		groyst

#define RODIF_DefaultValueListsLayout	grodll
#define RODIF_LayoutDirectives		groldr
#define RODIF_LayoutObjClassBody	grolcb
#define RODIF_LayoutObject		grolbj

#endif

EXTERN INT_type RODIF_PresentationStyle();
EXTERN INT_type RODIF_LayoutObjectClass();
EXTERN INT_type	RODIF_LayoutStyle();
EXTERN INT_type RODIF_LayoutObject();

#endif

#endif
