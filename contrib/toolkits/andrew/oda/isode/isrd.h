
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/isrd.h,v 1.3 89/09/29 11:28:10 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/isrd.h,v $

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

$Log:	isrd.h,v $
 * Revision 1.3  89/09/29  11:28:10  mss
 * Renamed to lcsite for consistency with raster package
 * 
 * Revision 1.2  89/09/29  11:16:01  mss
 * Changed site.h to tksite.h for compatibility with Andrew's
 * global site.h
 * 
 * Revision 1.1  89/09/21  05:30:57  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:34:28  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/11  14:54:55  mss
 * removed leading #
 * 
 * Revision 1.3  89/08/28  16:52:11  jl3p
 * added copyright notice
 * 
 * Revision 1.2  89/08/03  18:18:35  mss
 * minor editing for conversion to shared directory
 * 
 * Revision 1.1  89/08/03  15:44:18  mss
 * Initial revision
 * 
 * Revision 1.4  89/07/27  14:51:02  jl3p
 * added #include for <isrd2.h>
 * 
 * Revision 1.3  89/07/14  16:59:40  jl3p
 * no changes
 * 
 * Revision 1.2  89/06/27  12:47:31  annm
 * added fields to isode2toolkit_t to handle at_OBJECT_TYPE
 * 
 * Revision 1.1  89/06/21  17:37:44  annm
 * Initial revision
 * 
 * Revision 1.1  89/06/21  16:55:14  annm
 * Initial revision
 * 

*/

#ifndef	TK_INCLUDED_ISRD
#define	TK_INCLUDED_ISRD

#ifdef TK_TKI_ISRD
#define TK_ISRD
#else
#ifdef TK_IMPL_ISRD
#define	TK_ISRD
#else
#define AP_ISRD
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>

#include <tkerr.h>

#ifdef TK_IMPL_ISRD

struct isode2toolkit_t {
    INT_type was_error;
    DOCUMENT_type document;
    CONSTITUENT constituent;
};


#endif

#include <isrd19.h>
#include <isrd20.h>
#include <ishsh.h>

#endif
