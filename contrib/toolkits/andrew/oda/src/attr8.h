
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr8.h,v 1.1 89/09/21 06:24:28 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr8.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr8.h,v $
 * Revision 1.1  89/09/21  06:24:28  mss
 * Initial revision
 * 
 * Revision 1.11  89/09/11  14:32:23  mss
 * removed leading #
 * 
 * Revision 1.10  89/08/29  13:25:08  annm
 * added real names for too long names
 * 
 * Revision 1.9  89/03/29  13:07:52  mss
 * Fixed typo in ATTR_RummageThruDAPD
 * 
 * Revision 1.8  89/03/09  15:53:59  annm
 * new DAPD defaulting
 * 
 * Revision 1.7  89/02/23  20:39:41  annm
 * new DVL stuff
 * 
 * Revision 1.6  89/01/19  12:06:41  mss
 * Moved ATTR_FindContentArchClass from EXTERN in .h file
 * to static in .c file
 * 
 * Revision 1.5  89/01/17  16:56:59  annm
 * changes for DVL defaulting
 * 
 * Revision 1.4  88/09/12  17:04:18  annm
 * buffing rough edges relating to finding content architecture type
 * 
 * Revision 1.3  88/09/06  13:18:51  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/08/16  18:13:11  annm
 * 
 * 
 * Revision 1.1  88/08/15  18:07:36  annm
 * Initial revision
 * 
*/


#ifndef TK_INCLUDED_ATTR8
#define TK_INCLUDED_ATTR8

#undef EXTERN

#ifdef TK_GVARS_ATTR8
#define EXTERN
#else
#define EXTERN extern
#endif

/* stuff everyone sees */


#ifdef TK_ATTR

#endif

#ifdef TK_IMPL_ATTR

#ifndef USE_LONG_NAMES

#define	    ATTR_GetDocProfDefaults	gatgdp
#define	    ATTR_RummageThruDAPD	gatrtd
#define	    ATTR_CheckDAPDCOnstituents	gatcdc
#define	    ATTR_IsCCPresentation	gatccp
#define	    ATTR_IsCCCoding		gaticc
#define	    ATTR_IsRGPresentation	gatirp
#define	    ATTR_IsRGCoding		gatirc
#define	    ATTR_IsGGPresentation	gatigp
#define	    ATTR_IsGGCoding		gatigc

#define ATTR_CheckDAPDConstituents	gatdpa

#endif

EXTERN INT_type ATTR_GetDocProfDefaults();
EXTERN INT_type ATTR_RummageThruDAPD();
EXTERN INT_type ATTR_CheckDAPDConstituents();
EXTERN BOOL_type ATTR_IsCCPresentation();
EXTERN BOOL_type ATTR_IsCCCoding();
EXTERN BOOL_type ATTR_IsRGPresentation();
EXTERN BOOL_type ATTR_IsRGCoding();
EXTERN BOOL_type ATTR_IsGGPresentation();
EXTERN BOOL_type ATTR_IsGGCoding();

#endif

#endif

