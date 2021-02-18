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
 *  File:   iox.h
 *  Author: Andrew Werth
 *
 *  Header info for input/output routines for X Bitmap format.
 *
 */


#ifndef TK_INCLUDED_iox
#define	TK_INCLUDED_iox

#ifdef TK_TKI_x
#define TK_x
#else
#ifdef TK_IMPL_x
#define	TK_x
#else
#define AP_x
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_x.

*/



/* 
    EVERYONE SEES THIS
*/

typedef struct {
    CHAR_type	*name;	/* Name to use for structure */
} FORMAT_XBM_type;

/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_x

RASTER_type *X_ReadRaster();
INT_type    X_WriteRaster();

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_x

#define MAX_LINE    256
#define MAX_READ_INT 99999
#define MAX_INT_LENGTH 16

static RASTER_type  *ReadRaster();
static INT_type	    GetLine();
static BOOL_type    Compare();
static BYTE_type    *MoveTo();
static INT_type	    GetNum();
static INT_type	    PutString();
static INT_type	    PutNumber();

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif

/*  End of file   */
