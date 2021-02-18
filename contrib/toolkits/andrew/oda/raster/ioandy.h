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
 *  File:   ioandy.h
 *  Author: Andrew Werth
 *
 *  Header info for I/O routines for Andrew .rasters rasters.
 *
 */


#ifndef TK_INCLUDED_ioandy
#define	TK_INCLUDED_ioandy

#ifdef TK_TKI_andy
#define TK_andy
#else
#ifdef TK_IMPL_andy
#define	TK_andy
#else
#define AP_andy
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_andy.

*/



/* 
    EVERYONE SEES THIS
*/

typedef struct {
    INT_type	object_id;
} FORMAT_ANDREW_type;

/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_andy

RASTER_type *ANDY_ReadRaster();
INT_type    ANDY_WriteRaster();

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_andy

static VALUE_type   GetPels();
static INT_type	    GetBits();
static INT_type	    GetString();

static INT_type PutPels();
static INT_type PutHex();
static INT_type PutBits();
static INT_type PutClose();
static INT_type PutString();

#define RASTER_DONE ((VALUE_type) 1)
#define RASTER_MORE ((VALUE_type) 2)

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif

/*  End of file   */
