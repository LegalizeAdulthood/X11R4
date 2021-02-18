
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif3.h,v 1.1 89/09/21 06:45:00 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wodif3.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wodif3.h,v $
 * Revision 1.1  89/09/21  06:45:00  mss
 * Initial revision
 * 
 * Revision 1.9  89/09/11  14:41:40  mss
 * removed leading #
 * 
 * Revision 1.8  89/09/01  07:21:33  mss
 * Made unique names for remapping
 * 
 * Revision 1.7  89/08/31  23:25:08  mss
 * Made short names unqiue
 * 
 * Revision 1.6  89/08/31  15:55:37  annm
 * still fixing real names
 * 
 * Revision 1.5  89/08/30  16:10:34  annm
 * added real names for names that are too long
 * 
 * 
 * Revision 1.4  88/10/07  11:20:04  mz0k
 * USE_LONG_NAME --> USE_LONG_NAMES
 * 
 * Revision 1.3  88/09/06  13:23:08  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/08/04  14:28:04  mz0k
 * added copyright notice
 * 
*/

#ifndef TK_INCLUDED_WODIF3
#define TK_INCLUDED_WODIF3

#undef EXTERN

#ifdef TK_GVARS_WODIF3
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


#define	WODIF_GenericLayout	    gwoglo
#define	WODIF_WalkGenericLayout	    gwowkl	
#define	WODIF_SpecificLayout	    gwosly

#define WODIF_DefaultValueListsLayout	gwodll
#define WODIF_LayoutObjClassDesc	gwoloc
#define WODIF_LayoutObjClassDescBody	gwolod
#define WODIF_LayoutObjDesc		gwolob
#define WODIF_LayoutObjDescBody		gwoldb
#define WODIF_WalkSpecificLayout	gwowsy
		
#endif


EXTERN INT_type			WODIF_GenericLayout();
EXTERN INT_type			WODIF_WalkGenericLayout();
EXTERN INT_type			WODIF_SpecificLayout();


#endif

#endif
