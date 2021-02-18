
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts2.h,v 1.1 89/09/21 06:25:42 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts2.h,v $

(c) Copyright 1989 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	atts2.h,v $
 * Revision 1.1  89/09/21  06:25:42  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/11  14:37:00  mss
 * removed leading #
 * 
 * Revision 1.4  89/08/29  15:22:37  annm
 * added real names for too long names
 * 
 * Revision 1.3  89/08/29  15:20:12  annm
 * no change
 * 
 * Revision 1.2  89/04/28  21:33:46  mss
 * Added SetPresAttr and SetLayoutAttr
 * 
 * Revision 1.1  89/04/25  16:30:23  mss
 * Initial revision
 * 


*/


#ifndef TK_INCLUDED_ATTS2
#define TK_INCLUDED_ATTS2

/* 

    model for the subordinate .h file

    put the global comment of the .h file here

*/

#ifdef EXTERN
#undef EXTERN
#endif
#ifdef TK_GVARS_ATTS2
#define EXTERN
#else
#define EXTERN extern
#endif

#include <acd.h>

/*
    Declarations seen by everyone go here.
*/

/* Everyone sees the API routines in ATTR */

EXTERN	DOCUMENT_type	GetDocument();
EXTERN	INT_type	SetLayoutAttr();
EXTERN	INT_type	SetPresAttr();

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_ATTS

#ifndef USE_LONG_NAMES

#define SetStyleAttr	gasssa

#endif


#endif

/*
    Declarations seen within the Attribute Support module go here.
*/

#ifdef TK_IMPL_ATTS

#endif

#endif
