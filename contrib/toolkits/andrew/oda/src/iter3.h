
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter3.h,v 1.1 89/09/21 06:32:48 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter3.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	iter3.h,v $
 * Revision 1.1  89/09/21  06:32:48  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/11  14:38:35  mss
 * removed leading #
 * 
 * Revision 1.2  89/08/31  15:00:43  annm
 * co -l label1.h
 * rm iter2 iter3
 * 
 * Revision 1.1  88/12/23  16:54:37  mss
 * Initial revision
 * 

*/

#ifndef TK_INCLUDED_ITER3
#define TK_INCLUDED_ITER3

#undef EXTERN

#ifdef TK_GVARS_ITER3
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

#ifdef TK_ITER

#endif

/*
    Declarations seen within the iter module go here.
*/

#ifdef TK_IMPL_ITER

/* Private definitions */
#ifndef USE_LONG_NAMES

#define ITER_AddGenericKids gitagk
#define ITER_WalkConsTerm   gitwct

#define ITER_GetObjectID	gitgid
#define ITER_WalkConsExpr	gitwce
#define ITER_WalkConsFactor	gitwcf
#define ITER_WalkSeqConsTerm	gitwsq

#endif

EXTERN INT_type ITER_AddGenericKids(/*iter*/);

#endif

#endif
