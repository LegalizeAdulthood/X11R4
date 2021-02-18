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
 *  File:   iomacp.h
 *  Author: Andrew Werth
 *
 *  Header info for reading/writing MacPaint rasters.
 *
 */

#ifndef TK_INCLUDED_iomacp
#define TK_INCLUDED_iomacp

#ifdef TK_TKI_macp
#define TK_macp
#else
#ifdef TK_IMPL_macp
#define	TK_macp
#else
#define AP_macp
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_macp.

*/



/* 
    EVERYONE SEES THIS
*/

RASTER_type	*MACP_ReadRaster();
INT_type	MACP_WriteRaster();

typedef struct {
    INT_type	    left;
    INT_type	    right;
    INT_type	    top;
    INT_type	    bottom;
    INT_type	    macbin;	/* 1 = MB I, 2 = MB II */
}  FORMAT_MACPAINT_type;

/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_macp

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_macp

#define	HEADER_LENGTH	512
#define	MAX_LINES	720
#define	BYTES_WIDE	72
#define MAX_COLS	576	/* = BYTES_WIDE * 8 */

static INT_type PutHeader();
static checkborders();
static fillbits();
static INT_type writemacp();
static filltemp();
static sendbytes();
static header();
static int packit();
static INT_type ReadMacPaintFile();

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif

/*  End of file   */
