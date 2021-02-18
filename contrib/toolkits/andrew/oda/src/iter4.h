
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter4.h,v 1.1 89/09/21 06:33:08 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter4.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	iter4.h,v $
 * Revision 1.1  89/09/21  06:33:08  mss
 * Initial revision
 * 
 * Revision 1.2  89/08/30  17:11:05  annm
 * no change
 * 
 * Revision 1.1  89/04/24  14:30:17  mss
 * Initial revision
 * 

*/

#ifndef TK_INCLUDED_ITER4
#define TK_INCLUDED_ITER4

#undef EXTERN

#ifdef TK_GVARS_ITER4
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

EXTERN	ITERATOR_type	MakeStylesIterator();

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_ITER

#endif

/*
    Declarations seen within the iter module go here.
*/

#ifdef TK_IMPL_ITER

/* Private definitions */


#endif

#endif
