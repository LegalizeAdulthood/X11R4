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
 *  File:   rasio.h
 *  Author: Andrew Werth
 *
 *  Header info for raster input/output routines.
 *
 */

#ifndef TK_INCLUDED_rasio
#define TK_INCLUDED_rasio

#ifdef TK_TKI_rio
#define TK_rio
#else
#ifdef TK_IMPL_rio
#define	TK_rio
#else
#define AP_rio
#endif
#endif

#ifdef TK_IMPL_rio
#define TK_TKI_g3
#define TK_TKI_g4
#define TK_TKI_obm
#define TK_TKI_pbm
#define TK_TKI_macp
#define TK_TKI_ps
#define TK_TKI_andy
#define TK_TKI_x
#define TK_TKI_fbm
#define TK_TKI_etc
#endif

#include <iog3.h>
#include <iog4.h>
#include <ioobm.h>
#include <iopbm.h>
#include <iomacp.h>
#include <iops.h>
#include <ioandy.h>
#include <iox.h>
#include <iofbm.h>
#include <ioetc.h>

#ifdef TK_IMPL_rio
#undef TK_TKI_etc
#undef TK_TKI_fbm
#undef TK_TKI_x
#undef TK_TKI_andy
#undef TK_TKI_ps
#undef TK_TKI_macp
#undef TK_TKI_pbm
#undef TK_TKI_obm
#undef TK_TKI_g4
#undef TK_TKI_g3
#endif

/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_rio.

*/



/* 
    EVERYONE SEES THIS
*/

typedef INT_type FORMAT_GIF_type;
typedef INT_type FORMAT_SUN_type;
typedef INT_type FORMAT_IFF_type;

#define FORMAT_PBM	    ((TAG_type) 0)
#define FORMAT_ODABM	    ((TAG_type) 1)
#define FORMAT_GROUP3	    ((TAG_type) 3)
#define FORMAT_GROUP4	    ((TAG_type) 4)
#define FORMAT_MACPAINT	    ((TAG_type) 5)
#define FORMAT_POSTSCRIPT   ((TAG_type) 6)
#define FORMAT_ANDREW	    ((TAG_type) 7)
#define FORMAT_XBM	    ((TAG_type) 8)
#define FORMAT_GIF	    ((TAG_type) 9)
#define FORMAT_PCX	    ((TAG_type) 10)
#define FORMAT_SUN	    ((TAG_type) 11)
#define FORMAT_IFF	    ((TAG_type) 12)
#define FORMAT_DETERMINE    ((TAG_type) 19)

typedef INT_type		FORMAT_PBM_type;

typedef struct {
    TAG_type	format_tag;

    union {
	FORMAT_ANDREW_type	andrew;
	FORMAT_GIF_type		gif;
	FORMAT_GROUP3_type	g3;
	FORMAT_GROUP4_type	g4;
	FORMAT_IFF_type		iff;
	FORMAT_MACPAINT_type	macpaint;
	FORMAT_ODABM_type	oda_bitmap;
	FORMAT_PBM_type		pbm;
	FORMAT_POSTSCRIPT_type	ps;
	FORMAT_SUN_type		sun_ras;
	FORMAT_XBM_type		xbm;
    } format_body;
} FORMAT_type;

RASTER_type	*ReadRaster();
INT_type	WriteRaster();
RASTER_type	*ReadMemRaster();
BYTE_type 	*WriteMemRaster();

/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_rio

static RASTER_type  *DoReadRaster();
static INT_type	    DoWriteRaster();

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_rio

static INT_type    GetByteFromId();
static INT_type    GetByteFromMem();

static INT_type    PutByteToOd();
static INT_type    PutByteToMem();

#define MEM_BUF_SIZE	1024	    /* Block size for WriteMem() */

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#endif

/*  End of file   */
