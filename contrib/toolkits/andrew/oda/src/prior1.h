
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/prior1.h,v 1.1 89/09/21 06:35:33 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/prior1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	prior1.h,v $
 * Revision 1.1  89/09/21  06:35:33  mss
 * Initial revision
 * 
 * Revision 1.12  89/09/11  14:39:22  mss
 * removed leading #
 * 
 * Revision 1.11  89/09/01  15:47:02  annm
 * fixing USE_LONG_NAMES problems
 * 
 * Revision 1.10  88/09/06  13:22:01  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.9  88/09/06  12:06:15  annm
 * 
 * no change
 * 
 * Revision 1.8  88/06/08  16:58:00  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.7  88/06/08  14:37:09  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_PRIOR1
#define TK_INCLUDED_PRIOR1

#undef EXTERN

#ifdef TK_GVARS_PRIOR1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

EXTERN INT_type AttrDocProfPriority();
EXTERN INT_type ComponentAttrPriority();
EXTERN INT_type ContentAttrPriority();
EXTERN INT_type StyleAttrPriority();


/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_PRIOR

#ifndef USE_LONG_NAMES

#define PRIOR_Init  gprint	
#define PRIOR_Final gprfnl	

#endif

EXTERN INT_type PRIOR_Final();
EXTERN INT_type PRIOR_Init();

#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_PRIOR

#endif

#endif
