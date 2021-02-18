
/*
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
 */


#ifndef TK_INCLUDED_ISRD2
#define TK_INCLUDED_ISRD2

#undef EXTERN

#ifdef TK_GVARS_ISRD2
#define EXTERN
#else
#define EXTERN extern
#endif

/* declarations that everyone sees */

#ifdef TK_IMPL_ISRD

/* real names for procedures */

#ifndef USE_LONG_NAMES

#define String2Bytes		strbyt
#define SaveThisDocument	svtdoc
#define GetSavedDocument	gsvdoc
#define OID2Ints		oidint
#define MatchInts		mtints

#endif

EXTERN SEQUENCE_BYTE_type String2Bytes();
EXTERN INT_type SaveThisDocument();
EXTERN DOCUMENT_type GetSavedDocument();
EXTERN SEQUENCE_INT_type OID2Ints();
EXTERN BOOL_type MatchInts();

#endif

#endif
