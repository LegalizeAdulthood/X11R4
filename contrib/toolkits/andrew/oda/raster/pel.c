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
 *  File:   pel.c
 *  Author: Andrew Werth
 *
 *  Routines for setting, clearing, and getting the status of pels
 *  (both multi-planar and bit pels).
 */

#define TK_TKI_rast
#define TK_TKI_pel
#define TK_IMPL_pel
#define TK_GVARS_pel

#include <rgsite.h>
#include <raster.h>
#include <pel.h>

#undef TK_GVARS_pel
#undef TK_IMPL_pel
#undef TK_TKI_pel
#undef TK_TKI_rast

BYTE_type PEL_BitOnL[] = {
    128, 64, 32, 16, 8, 4, 2, 1
};

BYTE_type PEL_BitOffL[] = {
    ~128, ~64, ~32, ~16, ~8, ~4, ~2, ~1
};

BYTE_type PEL_BitOnR[] = {
    1, 2, 4, 8, 16, 32, 64, 128
};

BYTE_type PEL_BitOffR[] = {
    ~1, ~2, ~4, ~8, ~16, ~32, ~64, ~128
};



/*\
 *  ASSIGNBIT:  Assign a value to one bit in a raster.
\*/

INT_type AssignBit(raster, x, y, value)
    RASTER_type	    *raster;
    INT_type	    x;
    INT_type	    y;
    VALUE_type	    value;
{
    INT_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (x < 0 || y < 0 || x >= raster->width || y>= raster->height){
	rval = RG_ERROR_INT;
	RGError = RG_PELRANGE_err;
	goto LEAVE;
    }

    if (raster->rows[y].swap_tag == SWAP_BODY_SWAP) {
	if (RAST_SwapIn(raster,y) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }

    if (value == BIT_BLACK) {
	raster->rows[y].swap_body.pels[x >> 3] |= PEL_BitOnL[x & 7];
	rval = RG_SUCCESS_INT;
    }
    else if (value == BIT_WHITE) {
	raster->rows[y].swap_body.pels[x >> 3] &=PEL_BitOffL[x & 7];
        rval = RG_SUCCESS_INT;
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
 *  ASSIGNPEL:  Assign a value (color) to a pel.
 *		Currently this function is just a link to the
 *		GetPel routine with the appropriate parameters.
\*/

INT_type AssignPel(raster, x, y, value)
    RASTER_type	    *raster;
    INT_type	    x;
    INT_type	    y;
    PEL_type	    *value;
{
    INT_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (value->pel_tag != PEL_BITS) {
	rval = RG_ERROR_INT;
	RGError = RG_UNIMPL_err;
	goto LEAVE;
    }

    rval = AssignBit(raster, x, y, value->pel.bit);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  GETBIT:  Get the status of a bit, either BIT_BLACK or
 *		 BIT_WHITE.
\*/

VALUE_type GetBit(raster, x, y)
    RASTER_type	    *raster;
    INT_type	    x;
    INT_type	    y;
{
    VALUE_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (x < 0 || y < 0 || x>= raster->width || y>= raster->height){
	rval = RG_ERROR_VALUE;
	RGError = RG_PELRANGE_err;
	goto LEAVE;
    }

    if (raster->rows[y].swap_tag == SWAP_BODY_SWAP) {
	if (RAST_SwapIn(raster,y) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }

    if ((raster->rows[y].swap_body.pels[x >>3] & PEL_BitOnL[x & 7])
				     == 0) {
	rval = BIT_WHITE;
    }
    else {
	rval = BIT_BLACK;
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
 *  GETPEL:  Determining the value of a pel.  Currently this
 *	     routine is just a link to the GetBit function with
 *	     the appropriate parameters.
\*/

INT_type GetPel(raster, x, y, pel)
    RASTER_type	    *raster;
    INT_type	    x;
    INT_type	    y;
    PEL_type	    *pel;
{
    INT_type	    rval;
    VALUE_type	    bit;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if ((bit = GetBit(raster, x, y)) == RG_ERROR_VALUE) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    pel->pel_tag = PEL_BITS;
    pel->pel.bit = bit;
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}




/*  End of file .c  */
