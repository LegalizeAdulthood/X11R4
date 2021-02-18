
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


#ifndef TK_INCLUDED_ISWR2
#define TK_INCLUDED_ISWR2

#undef EXTERN

#ifdef TK_GVARS_ISWR2
#define EXTERN
#else
#define EXTERN extern
#endif

/* declarations that everyone sees */

#define ERROR_CHARS		(CHAR_type *) ERROR_POINTER

#ifdef TK_IMPL_ISWR

/* real names for procedures */

#ifndef USE_LONG_NAMES

#define String2Chars		strchr
#define Bytes2Chars		bytchr
#define GetLastIDDigits		gtlidd
#define Ints2OID		intoid

#endif

/* declarations seen by the implementer */

EXTERN CHAR_type *String2Chars();
EXTERN INT_type Bytes2Chars();
EXTERN STRING_type GetLastIDDigits();
EXTERN struct OIDentifier *Ints2OID();

#endif

#endif
