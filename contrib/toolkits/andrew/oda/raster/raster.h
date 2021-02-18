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
 *  File:   raster.h
 *  Author: Andrew Werth
 *
 *  Header info for rasters, creation, deletion, and management.
 *
 */

#ifndef TK_INCLUDED_raster
#define TK_INCLUDED_raster

#ifdef TK_TKI_rast
#define TK_rast
#else
#ifdef TK_IMPL_rast
#define	TK_rast
#else
#define AP_rast
#endif
#endif

/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_rast.

*/

/* 
    EVERYONE SEES THIS
*/

#define RG_STATUS_RELEASED  (0)
#define RG_STATUS_HELD	    (1)


typedef struct {
    TAG_type	swap_tag;	/* Row currently swapped?	*/
    union {
	BYTE_type	*pels;	/* Points to pel data		*/
	SWAP_INFO_type	*swap;	/* Points to swapped out data	*/
    } swap_body;
} ROW_t;

typedef struct {
    INT_type	width;	/* Number of horizontal pels (columns)	*/
    INT_type	height;	/* Number of vertical pels   (rows)	*/
    INT_type	bytes;	/* Number of bytes to store a row	*/
    ROW_t	*rows;	/* Points to the rows of pels/swapinfo	*/
    INT_type	held;	/* How many times the raster is held	*/
    VALUE_type	peltype;/* What type of pels are used in raster */
    POINTER_type format;/* What format the raster was stored in */
} RASTER_type;

#define RG_ERROR_RASTER	((RASTER_type *) 0)
#define RASTER_NULL	((RASTER_type *) RG_NULL)

RASTER_type	*MakeRaster();
RASTER_type	*ExtractRaster();
VALUE_type	RasterPelFormat();
VALUE_type	RasterStatus();
INT_type	DeleteRaster();
INT_type	RasterWidth();
INT_type	RasterHeight();
INT_type	HoldRaster();
INT_type	ReleaseRaster();
INT_type	InsertRaster();

/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_rast

#define SWAP_BODY_SWAP	    (0)
#define SWAP_BODY_PELS	    (1)

#define RG_ERROR_ROW	((ROW_t *) 1)
#define RG_NULL_ROW	((ROW_t *) RG_NULL)

struct  RasterList_struct {
    RASTER_type			*raster;
    struct RasterList_struct	*next;
};

typedef struct RasterList_struct RasterList_t;

#define RASTERL_NULL	((RasterList_t *) RG_NULL)

INT_type	RAST_SwapIn();
INT_type	RAST_FinalRasterList();
POINTER_type	RAST_SwapAlloc();
POINTER_type	RAST_InitRasterList();
RASTER_type	*RAST_AllocRaster();

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_rast

static INT_type	    AddRaster();
static INT_type	    FreeRaster();
static POINTER_type SwapAlloc();
static SWAP_INFO_type *RAST_SwapOut();

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif

/*  End of file   */
