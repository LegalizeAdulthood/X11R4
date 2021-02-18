
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mem1.h,v 1.1 89/09/21 06:34:14 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mem1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	mem1.h,v $
 * Revision 1.1  89/09/21  06:34:14  mss
 * Initial revision
 * 
 * Revision 1.14  89/09/11  14:38:59  mss
 * removed leading #
 * 
 * Revision 1.13  89/08/30  17:11:15  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.12  88/09/30  17:26:41  annm
 * looking for what turned out to be a very dumb bug (missing ;)
 * 
 * Revision 1.11  88/09/30  15:41:16  annm
 * fixing VMS bugs
 * 
 * Revision 1.10  88/09/06  13:21:22  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.9  88/09/06  12:05:41  annm
 * 
 * no change
 * 
 * Revision 1.8  88/06/08  16:57:26  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.7  88/06/08  14:36:34  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_MEM1
#define TK_INCLUDED_MEM1

#undef EXTERN

#ifdef TK_GVARS_MEM1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

EXTERN INT_type HoldAttr();
EXTERN INT_type HoldConstituent();
EXTERN INT_type HoldString();
EXTERN INT_type ReleaseAttr();
EXTERN INT_type ReleaseConstituent();
EXTERN INT_type ReleaseString();

/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_MEM

#ifndef USE_LONG_NAMES

#define MEM_Init    gmeint	
#define MEM_Final   gmefnl	

#endif

EXTERN INT_type MEM_Final();
EXTERN INT_type MEM_Init();


#endif

/*
    Declarations seen within the mem module go here.
*/

#ifdef TK_IMPL_MEM

#endif

#endif
