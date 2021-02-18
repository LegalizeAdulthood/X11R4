
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr16.h,v 1.3 89/10/13 17:08:03 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr16.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr16.h,v $
 * Revision 1.3  89/10/13  17:08:03  jr
 * Fix some ifdef/endif typos.
 * 
 * Revision 1.2  89/10/13  11:31:28  jr
 * Hacks for buggy VMS loader.
 * 
 * Revision 1.1  89/09/21  06:18:30  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/11  14:34:01  mss
 * removed leading #
 * 
 * Revision 1.2  89/08/30  16:58:25  annm
 * no changes
 * 
 * Revision 1.1  89/02/01  17:04:53  mss
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR16
#define TK_INCLUDED_ATTR16

#undef EXTERN

#ifdef TK_GVARS_ATTR16
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

#define ATTR16_Dummy	ga16dm

#endif

EXTERN INT_type ATTR16_Dummy();

#endif

#endif

