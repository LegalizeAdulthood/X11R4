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

/*
 *  File:   rgerr.h
 *  Author: Andrew Werth
 *
 *  Error codes for raster graphics tool kit.
 *
 */

#ifndef TK_INCLUDED_rgerror
#define TK_INCLUDED_rgerror

#define RG_ERROR_INT	(-1)
#define RG_SUCCESS_INT	(0)
#define RG_ERROR_VALUE	(-1)

#define RG_BADATTR_err	(1)	/* ODA & RG Tool Kit Errors */
#define RG_BADPARM_err	(2)
#define RG_DESCRIPTOR_err	(3)
#define RG_INTERNAL_err	(5)
#define RG_LOCAL_err	(6)
#define RG_NOFINAL_err	(9)
#define RG_NOMEM_err	(11)
#define RG_UNIMPL_err	(16)

#define RG_FORMBAD_err	(20)	/* Raster Graphics TK Errors */
#define RG_NOFORM_err	(21)
#define RG_PELRANGE_err	(22)
#define RG_PELFORMAT_err	(23)



#ifdef EXTERN
#undef EXTERN
#endif
#ifdef TK_GVARS_rgerr
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN INT_type		RGError;

#undef EXTERN

#endif

/*  End of file   */
