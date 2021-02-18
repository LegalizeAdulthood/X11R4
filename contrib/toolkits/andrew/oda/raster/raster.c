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
 *  File:   raster.c
 *  Author: Andrew Werth
 *
 *  Raster creation, deletion, and management.
 *
 */

#define RG_ERROR_SWAP_INFO ((SWAP_INFO_type *) 0)
#define TK_TKI_mem
#define TK_TKI_pel
#define TK_TKI_list
#define TK_TKI_rast
#define TK_IMPL_rast
#define TK_GVARS_raster

#include <rgsite.h>
#include <rgmem.h>
#include <pel.h>
#include <rglist.h>
#include <raster.h>

#undef TK_GVARS_raster
#undef TK_IMPL_rast
#undef TK_TKI_rast
#undef TK_TKI_list
#undef TK_TKI_pel
#undef TK_TKI_mem

#ifndef USE_LONG_NAMES

#define AddRaster	addras
#define FreeRaster	freras
#define RAST_SwapOut	raswot
#define RasterList	raslst

#endif

extern INT_type MEM_Alloced;

static LIST_t	    *RasterList = NULL_LIST;

/*\
 *  RAST_INITRASTERLIST:  Create a new NULL rasterlist.
\*/

POINTER_type RAST_InitRasterList()
{
    POINTER_type    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering();
    }
#endif

    RasterList = LST_MakeList();
    if (RasterList != ERROR_LIST) {
	rval = (POINTER_type) RasterList;
    }
    else {
	rval = RG_ERROR_POINTER;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving();
    }
#endif

    return(rval);
}


/*\
 *  RASTERHEIGHT:  Return the height field of a raster structure.
\*/

INT_type RasterHeight(raster)
    IN	RASTER_type *raster;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    rval = raster->height;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  RASTERWIDTH:  Return the width field of a raster structure.
\*/

INT_type RasterWidth(raster)
    IN	RASTER_type	*raster;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    rval = raster->width;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  RASTERSTATUS:  Return the held/released status.
\*/

VALUE_type RasterStatus(raster)
    IN	RASTER_type	*raster;
{
    VALUE_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (raster->held > 0) {
	rval = RG_STATUS_HELD;
    }
    else if (raster->held == 0) {
	rval = RG_STATUS_RELEASED;
    }
    else {
	rval = RG_ERROR_INT;
	RGError = RG_BADPARM_err;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  RASTERPELFORMAT:  Return the pel format type of a raster
 *		      structure.  Since only bitmaps are supported,
 *		      PEL_BITS is always returned.
\*/

VALUE_type RasterPelFormat(raster)
    IN	RASTER_type *raster;
{
    VALUE_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    rval = PEL_BITS;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  MAKERASTER:  Create a blank raster of specified size.
\*/

RASTER_type *MakeRaster(Width, Height)
    IN	INT_type    Width;
    IN	INT_type    Height;
{
    RASTER_type		*rval;
    REGISTER INT_type	y;
    REGISTER INT_type	x;
    POINTER_type	pels;
    INT_type		BytesWide;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering();
    }
#endif

    if (Width < 1 || Height < 1) {
	rval = RG_ERROR_RASTER;
	RGError = RG_BADPARM_err;
	goto LEAVE;
    }

    rval = RAST_AllocRaster();
    if (rval == (RASTER_type *) RG_ERROR_POINTER) {
	rval = RG_ERROR_RASTER;
	RGError = RG_NOMEM_err;
	goto LEAVE;
    }

    rval->width		   = Width;
    rval->bytes		   = (Width + 7) / 8;
    rval->height	   = Height;
    rval->held		   = RG_STATUS_RELEASED;
    rval->format	   = (POINTER_type) 0;
    rval->peltype	   = PEL_BITS;
    rval->rows		   = (ROW_t *) MEM_Malloc
					(Height * sizeof(ROW_t));
    BytesWide = rval->bytes;
    for (y = 0; y < Height; y++) {
	rval->rows[y].swap_tag = SWAP_BODY_PELS;
	if ((pels = MEM_Malloc(BytesWide)) == RG_ERROR_POINTER) {
	    rval = RG_ERROR_RASTER;
	    goto LEAVE;
	}
	rval->rows[y].swap_body.pels = (BYTE_type *) pels;
	for (x = 0; x < rval->bytes; x++) {
	    rval->rows[y].swap_body.pels[x] = 0x00;
	}
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving();
    }
#endif

    return(rval);
}


/*\
 *  DELETERASTER:  Delete a raster from memory.
\*/

INT_type DeleteRaster(raster)
    IN	RASTER_type *raster;
{
    INT_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering();
    }
#endif

    LST_DeleteElement(RasterList, (POINTER_type) raster);
    rval = FreeRaster(raster);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving();
    }
#endif

    return(rval);
}


/*\
 *  ADDRASTER:  Add a raster to the raster list.
\*/

static INT_type AddRaster(raster)
    IN	RASTER_type *raster;
{
    INT_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering();
    }
#endif

    if (LST_AddElement(RasterList,(POINTER_type) raster) 
						== RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving();
    }
#endif

    return(rval);
}


/*\
 *  FREERASTER:  Free up the raster pieces (rows) and then
 *		 deallocate the raster structure itself.
\*/

static INT_type FreeRaster(raster)
    IN	RASTER_type *raster;
{
    INT_type		rval;
    REGISTER INT_type	rownum;
    ROW_t		*row;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering();
    }
#endif

    for (rownum = 0; rownum < raster->height; rownum++) {
	if (raster->rows != RG_NULL) {
	    row =  &raster->rows[rownum];
	    if (RAST_SwapIn(raster,rownum) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	    MEM_Free(row->swap_body.pels);
	}
    }

    MEM_Free(raster->rows);
    MEM_Free(raster);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving();
    }
#endif

    return(rval);
}


/*\
 *  RAST_FINALRASTERLIST:  Remove and deallocate the raster list.
\*/

INT_type RAST_FinalRasterList()
{
    INT_type	    rval;
    RASTER_type	    *raster;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering();
    }
#endif

    LST_MoveHead(RasterList);
    while ((raster = (RASTER_type *) LST_NextElement(RasterList)) 
		   != (RASTER_type *) RG_ERROR_POINTER) {
	if (FreeRaster(raster) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    RGError = RG_INTERNAL_err;
	    goto LEAVE;
	}
    }
    LST_FreeList(RasterList);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving();
    }
#endif

    return(rval);
}


/*\
 *  RAST_ALLOCRASTER:  Allocate a raster structure.  If not enough
 *		       memory is available, attempt to swap out
 *		       other rasters until memory is available.
\*/

RASTER_type *RAST_AllocRaster()
{
    RASTER_type	    *rval;
    POINTER_type    Raster;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if ((Raster=MEM_Malloc(sizeof(RASTER_type))) == RG_ERROR_POINTER) {
	rval = RG_ERROR_RASTER;
	goto LEAVE;
    }
    if (AddRaster((RASTER_type *) Raster) == RG_ERROR_INT) {
	rval = RG_ERROR_RASTER;
	goto LEAVE;
    }

    rval = (RASTER_type *) Raster;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  RAST_SWAPALLOC:  Keep swapping out memory until a memory 
 *		     block of size Size can be allocated.
\*/

POINTER_type RAST_SwapAlloc(Size)
    IN	INT_type    Size;
{
    POINTER_type	rval;

#ifdef SWAPPING
    REGISTER INT_type	ToFree;
    RASTER_type		*raster;
    REGISTER INT_type	y;
    ROW_t		*row;
    SWAP_INFO_type	*swinfo;
#endif

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

#ifdef SWAPPING

    ToFree = Size;    
    LST_MoveHead(RasterList);
    y = 0;

    raster = (RASTER_type *) LST_NextElement(RasterList);

    while (raster != (RASTER_type *) NULL_LISTEL) {
	if (raster->held == 0) {
	    for (y = 0; y < raster->height; y++) {
		row = &raster->rows[y];
		if (row->swap_tag == SWAP_BODY_PELS) {
		    swinfo = RAST_SwapOut(row->swap_body.pels,
						raster->bytes);
		    row->swap_tag = SWAP_BODY_SWAP;
		    row->swap_body.swap = swinfo;
		    ToFree -= raster->bytes;
		    if (ToFree <= 0) {
			rval = RG_SUCCESS_INT;
			goto LEAVE;
		    }
		}
	    }
	}
	raster = (RASTER_type *) LST_NextElement(RasterList);
    }

#endif

    rval = RG_ERROR_POINTER;
    RGError = RG_NOMEM_err;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


INT_type RAST_SwapIn(raster,line)
    IN	RASTER_type *raster;
    IN	INT_type    line;
{
    INT_type	    rval;
    ROW_t	    *row;

#ifdef SWAPPING
    SWAP_INFO_type  *swinfo;
    BYTE_type	    *pels;
#endif

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    row = (ROW_t *) &raster->rows[line];

    if (row->swap_tag == SWAP_BODY_PELS) {
	rval = RG_SUCCESS_INT;
	goto LEAVE;
    }

#ifdef SWAPPING

    swinfo = row->swap_body.swap;

    pels = (BYTE_type *) MEM_Malloc(raster->bytes);
    if (LOCAL_SwapIn(swinfo,row->swap_body.pels,raster->bytes)
						 == LCL_ERROR_INT){
	RGError = LCLError;
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    row->swap_body.pels = pels;
    row->swap_tag = SWAP_BODY_SWAP;

#else

    rval = RG_ERROR_INT;
    RGError = RG_INTERNAL_err;
    goto LEAVE;

#endif

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  RAST_SWAPOUT:  Swap out specified memory and adjust counters.
\*/

static SWAP_INFO_type *RAST_SwapOut(address, length)
    IN	POINTER_type	address;
    IN	INT_type	length;
{
    SWAP_INFO_type  *rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

#ifdef SWAPPING

    rval = LOCAL_SwapOut(address, length);
    if (rval == LCL_ERROR_SWAP_INFO) {
	RGError = LCLError;
	rval = RG_ERROR_SWAP_INFO;
	goto LEAVE;
    }
    MEM_Free(address);

    MEM_Alloced -= length;

#else

    rval = RG_ERROR_SWAP_INFO;
    RGError = RG_INTERNAL_err;

#endif

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}





/*\
 *  HOLDRASTER:  Tool Kit routine which requests that the specified
 *		 raster be held entirely in main memory.  Routine
 *		 swaps in those rows which aren't in memory, if
 *		 the raster is already being.
\*/

INT_type HoldRaster(raster)
    IN 	RASTER_type *raster;
{
    INT_type	rval;
    INT_type	y;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (raster->held == (INT_type) 0) {
	for (y = 0; y < raster->height; y++) {
	    if (raster->rows[y].swap_tag == SWAP_BODY_SWAP) {
		if (RAST_SwapIn(raster,y) == RG_ERROR_INT) {
		    rval = RG_ERROR_INT;
		    goto LEAVE;
		}
	    }
	}
    }

    raster->held++;
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  RELEASERASTER:  Decreases the held count on the specified
 *		    raster.  If the count reaches zero, then the
 *		    raster is not being held any more.  Has no
 *		    effect if count is already zero.
\*/

INT_type ReleaseRaster(raster)
    IN	RASTER_type *raster;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (raster->held > 0) {
	raster->held--;
    }
    else {
	raster->held = 0;
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  EXTRACTRASTER:  Create a new raster copied from a portion of
 *		    an existing raster.
\*/

RASTER_type *ExtractRaster(raster, left, top, right, bottom)
    IN	RASTER_type	*raster;
    IN	INT_type	left;
    IN	INT_type	top;
    IN	INT_type	right;
    IN	INT_type	bottom;
{
    RASTER_type		*rval;
    INT_type		xs;	/* Source coordinates	    */
    INT_type		ys;
    REGISTER INT_type	xd;	/* Destination coordinates  */
    REGISTER INT_type	yd;
    REGISTER INT_type	bit;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (left < 0 || top < 0 ||
	right >= raster->width || bottom >= raster->height ||
	left >= right || top >= bottom) {
	rval = RG_ERROR_RASTER;
	RGError = RG_PELRANGE_err;
	goto LEAVE;
    }

    if ((rval = MakeRaster(right - left + 1, bottom - top + 1))
				    == RG_ERROR_RASTER) {
	goto LEAVE;				
    }
    
    rval->format = raster->format;  /* Inherit default format */
    rval->peltype = raster->peltype;/* Inherit the pel type   */

    yd = 0;
    for (ys = top; ys <= bottom; ys++) {
	xd = 0;
	for (xs = left; xs <= right; xs++) {
	    if ((bit = GetBit(raster, xs, ys)) == RG_ERROR_INT ||
	        AssignBit(rval, xd, yd, bit) == RG_ERROR_INT) {
		DeleteRaster(rval);
		rval = RG_ERROR_RASTER;
		goto LEAVE;
	    }
	    xd++;
	}
	yd++;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  INSERTRASTER:  Place one raster into another raster at the
 *		   specified coordinates.
\*/

INT_type InsertRaster(SourceRaster, DestRaster, x, y)
    IN	RASTER_type	*SourceRaster;
    IN	RASTER_type	*DestRaster;
    IN	INT_type	x;
    IN	INT_type	y;
{
    INT_type		rval;
    INT_type		xs;	/* Source coordinates	    */
    INT_type		ys;
    REGISTER INT_type	xd;	/* Destination coordinates  */
    REGISTER INT_type	yd;
    REGISTER INT_type	bit;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (x < 0 || x >= DestRaster->width || 
	y < 0 || y >= DestRaster->height) {
	rval = RG_ERROR_INT;
	RGError = RG_PELRANGE_err;
	goto LEAVE;
    }

    yd = y;
    for (ys = 0; ys < SourceRaster->height && 
		 yd < DestRaster->height; ys++) {
	xd = x;
	for (xs = 0; xs < SourceRaster->width && 
		     xd < DestRaster->width; xs++) {
	    if ((bit = GetBit(SourceRaster, xs, ys)) == RG_ERROR_INT ||
		AssignBit(DestRaster, xd, yd, bit) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	    xd++;
	}
	yd++;
    }
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*  These are temporary  */

#ifndef SWAPPING

SWAP_INFO_type *LOCAL_SwapOut(address, length)
    IN POINTER_type address;
    IN INT_type	    length;
{
    SWAP_INFO_type  *rval;    /* The returned value */

    rval = RG_ERROR_SWAP_INFO;
    RGError = RG_UNIMPL_err;

LEAVE:
    return rval;
}

INT_type LOCAL_SwapIn(swapinfo, address, available)
    IN SWAP_INFO_type	*swapinfo;
    IN POINTER_type	address;
    IN INT_type		available;
{
    INT_type	rval;	    /* The returned value */

    rval = RG_ERROR_INT;
    RGError = RG_UNIMPL_err;

LEAVE:
    return rval;
}

#endif


/*  End of file raster.c  */
