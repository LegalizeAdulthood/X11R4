
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts3.h,v 1.1 89/09/21 06:26:04 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts3.h,v $

$Log:	atts3.h,v $
 * Revision 1.1  89/09/21  06:26:04  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/11  14:37:13  mss
 * removed leading #
 * 
 * Revision 1.2  89/08/29  15:24:08  annm
 * added real names for too long names
 * 
 * Revision 1.1  89/07/19  21:04:19  mss
 * Initial revision
 * 

*/


#ifndef TK_INCLUDED_ATTS3
#define TK_INCLUDED_ATTS3

/* 

    subordinate .h file for deletion support


*/

#ifdef EXTERN
#undef EXTERN
#endif
#ifdef TK_GVARS_ATTS3
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

/* Everyone sees the API routines in ATTR */

EXTERN  INT_type	DeleteConstExpr();
EXTERN  INT_type	DeleleFontReference();


/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_ATTS

#ifndef USE_LONG_NAMES

#define DeleteConstTerm	    gasdct

#endif


#endif

/*
    Declarations seen within the Attribute Support module go here.
*/

#ifdef TK_IMPL_ATTS

#endif

#endif
