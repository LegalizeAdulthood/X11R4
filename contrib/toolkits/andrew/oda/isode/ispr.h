
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ispr.h,v 1.1 89/09/21 05:23:28 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ispr.h,v $

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

$Log:	ispr.h,v $
 * Revision 1.1  89/09/21  05:23:28  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:26:36  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/11  14:55:56  mss
 * removed leading #
 * 
 * Revision 1.2  89/08/28  16:55:03  jl3p
 * added copyright notice
 * 
 * Revision 1.1  89/08/15  17:20:19  mss
 * Initial revision
 * 
  

*/

#ifndef	TK_INCLUDED_ISPR
#define	TK_INCLUDED_ISPR

#ifdef TK_TKI_ISPR
#define TK_ISPR
#else
#ifdef TK_IMPL_ISPR
#define	TK_ISPR
#else
#define AP_ISPR
#endif
#endif


#ifdef TK_IMPL_ISPR



#endif

#include <ispr19.h>

#endif
