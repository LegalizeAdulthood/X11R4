
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/pcdos/RCS/vlocal.h,v 1.1 89/09/21 06:13:46 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/pcdos/RCS/vlocal.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	vlocal.h,v $
 * Revision 1.1  89/09/21  06:13:46  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/06  17:20:58  annm
 * fixing problems with names
 * 
 * Revision 1.2  88/10/05  23:42:15  mss
 * CHanged "s to <>s
 * 
 * Revision 1.1  88/06/14  14:27:36  jr
 * Initial revision
 * 
 * Revision 1.3  88/06/08  17:31:21  annm
 * dealt with line too long in copyright notice
 * and changed #end to #endif
 * 
 * Revision 1.2  88/06/08  15:35:50  annm
 * added copyright notice
 * 
*/

/*

	Include file for site-dependent validation routines.

*/

#ifdef INCLUDED_VLOCAL
#define INCLUDED_VLOCAL

#include <odatk.h>

#ifdef VLOCAL_IMPL
#define EXTERN

#else
#define EXTERN extern

#endif

#ifndef USE_LONG_NAMES

#define VALID_GetStdout	vlstd
#define VALID_ReadOpen	vlrdo
#define VALID_WriteOpen	vlwro

#endif

EXTERN INT_type VALID_GetStdout();
EXTERN INT_type VALID_ReadOpen();
EXTERN INT_type VALID_WriteOpen();

#endif
