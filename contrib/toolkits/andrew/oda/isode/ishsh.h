
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ishsh.h,v 1.1 89/09/21 05:22:41 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ishsh.h,v $

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

$Log:	ishsh.h,v $
 * Revision 1.1  89/09/21  05:22:41  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:25:50  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/11  14:55:18  mss
 * removed leading #
 * 
 * Revision 1.3  89/08/28  16:52:19  jl3p
 * added copyright notice
 * 
 * Revision 1.2  89/08/16  09:55:17  mss
 * Arranged everything to use global odaiso.h
 * 
 * Revision 1.1  89/08/03  15:44:42  mss
 * Initial revision
 * 
 * Revision 1.1  89/06/21  17:37:50  annm
 * Initial revision
 * 
 * Revision 1.1  89/06/21  16:55:29  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ISHSH1
#define TK_INCLUDED_ISHSH1

#undef EXTERN

#ifdef TK_GVARS_ISRD19
#define EXTERN
#else
#define EXTERN extern
#endif

/* declarations that everyone sees */

/* stuff seen only by the implementer */

#ifdef TK_IMPL_ISRD

#ifndef USE_LONG_NAMES

#define buckets gisbuc

#define ERROR_BUCKET	gisebu

#endif

struct hash_t {
    struct hash_t *next;
    STRING_type id;
    CONSTITUENT constituent;
};

struct hash_t *ERROR_BUCKET;

/* the hash buckets */

#define NHASH	20

#define EMPTY_BUCKET	( (struct hash_t *) 0)

struct hash_t buckets[NHASH];

#endif

#endif

