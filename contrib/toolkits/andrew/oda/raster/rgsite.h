/*\
 *
 *  (C) Copyright 1989 by Carnegie Mellon University
 *
 *  Permission to use, copy, modify, and distribute these programs
 *  and their documentation for any purpose and without fee is
 *  hereby granted, provided that this copyright and permission
 *  notice appear on all copies and supporting documentation, and
 *  that the name of Carnegie Mellon University not be used in
 *  advertising or publicity pertaining to distribution of the
 *  programs without specific prior permission and notice be given
 *  in supporting documentation that copying and distribution is
 *  by permission of Carnegie Mellon University.
 *
 *  Carnegie Mellon University makes no representations about the
 *  suitability of this software for any purpose.  It is provided
 *  as is, without express or implied warranty.
 *
 *  Software by Andrew Werth, Information Technology Center,
 *  Carnegie Mellon University, except where noted.
\*/

#ifndef TK_INCLUDED_rgsite
#define TK_INCLUDED_rgsite


/* If you are NOT using the CMU ODA Toolkit, you must
 * #define RGTK_STANDALONE
 */

#define RGTK_STANDALONE

/* Type definition for SWAP_INFO_type should either be here or
 * in local.h 
 */

typedef int	    SWAP_INFO_type;

/* Type definition for FLOAT_type follows: */

typedef float	    FLOAT_type;


/* The following four lines should probably not be changed */

#ifdef RG_NULL
#undef  RG_NULL
#endif
#ifdef RG_EOF
#undef  RG_EOF
#endif
#define RG_NULL	    ((INT_type) 0)
#define RG_EOF	    ((INT_type) -1)

/* These files MUST be included from this file */

#include <rgreal.h>
#include <lcsite.h>
#include <local.h>
#include <rgdbg.h>
#include <rgerr.h>

#define RG_ERROR_POINTER (LCL_ERROR_POINTER)

#endif
