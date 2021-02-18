
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr6.h,v 1.1 89/09/21 06:23:47 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr6.h,v $


(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr6.h,v $
 * Revision 1.1  89/09/21  06:23:47  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/11  14:32:11  mss
 * removed leading #
 * 
 * Revision 1.5  89/08/30  17:04:30  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.4  88/09/30  15:42:03  annm
 * no change
 * 
 * Revision 1.3  88/09/06  13:18:38  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/08/01  18:46:30  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.1  88/08/01  13:44:32  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR6
#define TK_INCLUDED_ATTR6

#undef EXTERN

#ifdef TK_GVARS_ATTR6
#define EXTERN
#else
#define EXTERN extern
#endif

/* stuff everyone sees */


#ifdef TK_ATTR

#endif

#ifdef TK_IMPL_ATTR

#ifndef USE_LONG_NAMES

#define ATTR_FindLocal			gatflc
#define ATTR_InhertForObject		gatifo
#define ATTR_InheritForContentPortion	gaticp

#endif

EXTERN INT_type ATTR_FindLocal();
EXTERN INT_type ATTR_InhertForObject();
EXTERN INT_type ATTR_InheritForContentPortion();

#endif


#endif

