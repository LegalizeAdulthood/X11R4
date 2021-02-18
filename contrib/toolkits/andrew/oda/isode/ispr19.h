
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ispr19.h,v 1.1 89/09/21 05:27:35 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ispr19.h,v $

 *
 * (C) Copyright 1989 by Carnegie Mellon University
 *
 * Permission to use, copy, modify, and distribute these programs
 * and their documentation for any purpose and without fee is
 * hereby granted, provided that this copyright and permission
 * notice appear on all copies and supporting documentation, and
 * that the name of Carnegie Mellon University not be used in
 * advertising or publicity pertaining to distribution of the
 * programs without specific prior permission and notice be given
 * in supporting documentation that copying and distribution is
 * by permission of Carnegie Mellon University.
 *
 * Carnegie Mellon University makes no representations about the
 * suitability of this software for any purpose.  It is provided
 * as is, without express or implied warranty.
 *
 * Software by Ann Marks and James T. Lui,
 * Information Technology Center, Carnegie Mellon University,
 * except where noted.
 *

$Log:	ispr19.h,v $
 * Revision 1.1  89/09/21  05:27:35  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:31:07  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/11  14:56:10  mss
 * removed leading #
 * 
 * Revision 1.2  89/08/28  16:54:56  jl3p
 * added copyright notice
 * 
 * Revision 1.1  89/08/15  17:19:59  mss
 * Initial revision
 * 
* 
*/

#ifndef TK_INCLUDED_ISPR19
#define TK_INCLUDED_ISPR19

#undef EXTERN

#ifdef TK_GVARS_ISPR19
#define EXTERN
#else
#define EXTERN extern
#endif

/* declarations that everyone sees */
#include <apirt.h>

EXTERN INT_type isode_PrintODIF();

#ifdef TK_IMPL_ISPR

/* real names for procedures */

#ifndef USE_LONG_NAMES

#endif

/* declarations seen by the implementer */


#endif

#endif
