
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr7.h,v 1.2 89/10/26 14:07:04 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr7.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr7.h,v $
 * Revision 1.2  89/10/26  14:07:04  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:24:08  mss
 * Initial revision
 * 
 * Revision 1.7  89/08/30  17:04:35  annm
 * added USE_LONG_NAMES
 * 
 * Revision 1.6  88/09/30  15:40:52  annm
 * fixing VMS bugs
 * 
 * Revision 1.5  88/09/19  17:06:13  jr
 * Declare ATTR_GetDefaults
 * 
 * Revision 1.4  88/09/12  13:00:10  annm
 * buffed rough edge in end of defaulting condition handling for related parms.
 * 
 * Revision 1.3  88/09/06  13:18:45  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/09/01  14:20:43  annm
 * restructured the constituent inherit routine to put the code that actually
 * does the inherit in its own subr. Dealing with the termination of inheritance is still done in in the constituent inherit routine.
 * 
 * Revision 1.1  88/08/10  15:43:17  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR7
#define TK_INCLUDED_ATTR7

#undef EXTERN

#ifdef TK_GVARS_ATTR7
#define EXTERN
#else
#define EXTERN extern
#endif

/* stuff everyone sees */


#ifdef TK_ATTR

#endif

#ifdef TK_IMPL_ATTR

/*
     return codes for defaulting - note the TIME_TO_STOP is
    used ONLY with related parameters when something has been found 
    but NOT the entire value - this means go get the defaults
*/
#define	ALL_DONE	((INT_type)1)
#define NOT_DONE	((INT_type)2)
#define TIME_TO_STOP	((INT_type)3)

#ifndef USE_LONG_NAMES

#define ATTR_InheritFromObjClass	gatioc
#define	ATTR_InheritFromConstAndStyle	gatics
#define ATTR_InheritFromStyle		gatifs
#define ATTR_InheritFromConst		gatifc
#define ATTR_LocateConstAttr		gatlca
#define ATTR_LoadDestination		gatldt
#define ATTR_AreWeDone			gatawd
#define ATTR_SetUpReturnToUser		gatsru
#define ATTR_GetDefaults		gatgdf

#endif

EXTERN INT_type ATTR_InheritFromObjClass();
EXTERN INT_type ATTR_InheritFromConstAndStyle();
EXTERN INT_type ATTR_InheritFromStyle();
EXTERN INT_type ATTR_InheritFromConst();
EXTERN INT_type ATTR_LocateConstAttr();
EXTERN INT_type ATTR_LoadDestination();
EXTERN INT_type ATTR_AreWeDone();
EXTERN INT_type ATTR_SetUpReturnToUser();
EXTERN INT_type ATTR_GetDefaults();

#endif

#endif

