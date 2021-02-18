
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr18.h,v 1.4 89/10/13 17:08:34 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr18.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr18.h,v $
 * Revision 1.4  89/10/13  17:08:34  jr
 * Fix some ifdef/endif typos.
 * 
 * Revision 1.3  89/10/13  13:40:38  jr
 * Remove extra #endif
 * 
 * Revision 1.2  89/10/13  11:33:11  jr
 * Hacks for buggy VMS loader.
 * 
 * Revision 1.1  89/09/21  06:19:05  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/11  14:34:29  mss
 * removed leading #
 * 
 * Revision 1.2  89/08/30  16:58:51  annm
 * no change
 * 
 * Revision 1.1  89/02/01  17:05:52  mss
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR18
#define TK_INCLUDED_ATTR18

#undef EXTERN

#ifdef TK_GVARS_ATTR18
#define EXTERN
#else
#define EXTERN extern
#endif

/* Everyone sees the API */

#ifdef TK_TKI_ATTR
#endif

#ifdef TK_IMPL_ATTR
/* Visible to attribute module */
#endif

#ifdef TK_ATTR
/* Visible to toolkit as a whole */

#ifndef USE_LONG_NAMES

#define ATTR18_Dummy	ga18dm

#endif

EXTERN INT_type ATTR18_Dummy();

#endif

#endif
