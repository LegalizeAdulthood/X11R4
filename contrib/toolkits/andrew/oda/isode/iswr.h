
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/iswr.h,v 1.3 89/09/29 11:28:15 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/iswr.h,v $

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

$Log:	iswr.h,v $
 * Revision 1.3  89/09/29  11:28:15  mss
 * Renamed to lcsite for consistency with raster package
 * 
 * Revision 1.2  89/09/29  11:16:06  mss
 * Changed site.h to tksite.h for compatibility with Andrew's
 * global site.h
 * 
 * Revision 1.1  89/09/21  05:38:56  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:43:10  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/11  14:55:30  mss
 * removed leading #
 * 
 * Revision 1.3  89/08/28  16:48:49  jl3p
 * added copyright notice
 * 
 * Revision 1.2  89/08/03  18:19:16  mss
 * minor editing for conversion to shared directory
 * 
 * Revision 1.1  89/08/03  15:39:43  mss
 * Initial revision
 * 
 * Revision 1.2  89/07/14  17:01:12  jl3p
 * Added #include for <iswr2.h>
 * 
 * Revision 1.1  89/06/21  16:52:12  annm
 * Initial revision
 * 

*/

#ifndef	TK_INCLUDED_ISWR
#define	TK_INCLUDED_ISWR

#ifdef TK_TKI_ISWR
#define TK_ISWR
#else
#ifdef TK_IMPL_ISWR
#define	TK_ISWR
#else
#define AP_ISWR
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>

#include <tkerr.h>

#ifdef TK_IMPL_ISWR

/* values for the what_kind_constituent field */
#define DOC_PROFILE	    1
#define GENERIC_LAYOUT	    2
#define SPECIFIC_LAYOUT	    3
#define TEXT		    4
#define GENERIC_LOGICAL	    5
#define SPECIFIC_LOGICAL    6
#define PRESENTATION	    7
#define LAYOUT		    8

struct isode2toolkit_t {
    PS		    ps;
    CONSTITUENT	    constituent;
    INT_type	    what_kind_constituent;
    SEQUENCE_CONSTITUENT_type Contents;
};


#endif

#include <iswr19.h>
#include <iswr20.h>

#endif
