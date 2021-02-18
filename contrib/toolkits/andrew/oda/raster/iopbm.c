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
 *  File:   iopbm.c
 *  Author: Andrew Werth
 *
 *  I/O for PBM formats (Poskanzer Portable Bitmap)
 *
 */

#define TK_TKI_mem
#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_pel
#define TK_TKI_etc
#define TK_TKI_pbm
#define TK_IMPL_pbm
#define TK_GVARS_iopbm

#include <rgsite.h>
#include <rgmem.h>
#include <raster.h>
#include <rasio.h>
#include <pel.h>
#include <rgerr.h>
#include <ioetc.h>
#include <iopbm.h>

#undef TK_TKI_mem
#undef TK_TKI_rast
#undef TK_TKI_rio
#undef TK_TKI_pel
#undef TK_TKI_etc
#undef TK_TKI_pbm
#undef TK_IMPL_pbm
#undef TK_GVARS_iopbm

#ifndef USE_LONG_NAMES

#define ReadBit		redbit
#define ReadBitmap	rdbtmp
#define ReadInt		redint
#define ReadSize	redsiz
#define WriteBitmap	wrbtmp
#define pbm_GetByte	pbmgby
#define pbm_Parameter	pbmpar
#define pbm_PutByte	pbmpby
#define ReadMagic	redmag

#endif

static RASTER_type *ReadBitmap();


static INT_type		(*pbm_GetByte)();
static INT_type		(*pbm_PutByte)();
static POINTER_type	pbm_Parameter;

extern BYTE_type	PEL_BitOnL[];

#define WHITESPACE(c)	((c) == ' ' || (c) == '\t' || (c) == '\n')
#define NUMBER(c)	((c) >= '0' && (c) <= '9')


/*\
 *  PBM_READRASTER:  Read a PBM raster into memory.
\*/

RASTER_type *PBM_ReadRaster(GetByte, parameter, format)
    IN	INT_type	(*GetByte)();
    IN	POINTER_type    parameter;
    IN	FORMAT_type	*format;
{
    RASTER_type	    *rval;
    INT_type	    Width;
    INT_type	    Height;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PBM_ReadRaster");
    }
#endif

    pbm_GetByte = GetByte;
    pbm_Parameter = parameter;

    if (ReadMagic() == RG_ERROR_INT) {
	rval = RG_ERROR_RASTER;
	RGError = RG_NOFORM_err;
	goto LEAVE;
    }
    if (ReadSize(&Width,&Height) == RG_ERROR_INT) {
	rval = RG_ERROR_RASTER;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }

    rval = MakeRaster(Width, Height, PEL_BITS);
    if (rval == RG_ERROR_RASTER) {
	goto LEAVE;		/* RGError already set */
    }
    
    rval = ReadBitmap(rval);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PBM_ReadRaster");
    }
#endif

    return(rval);
}


/*\
 *  READMAGIC:  Read PBM Magic Number.
\*/

static INT_type ReadMagic()
{
    INT_type	rval;
    INT_type	ch1;
    INT_type	ch2;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ReadMagic");
    }
#endif

    ch1 = (*pbm_GetByte)(pbm_Parameter);
    ch2 = (*pbm_GetByte)(pbm_Parameter);

    if (ch1 != (BYTE_type) PBM_MAGIC1 && 
	ch2 != (BYTE_type) PBM_MAGIC2) {

	rval = RG_ERROR_INT;
	RGError = RG_NOFORM_err;
	goto LEAVE;
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ReadMagic");
    }
#endif

    return(rval);
}


/*\
 *  READSIZE:  Get the size of a PBM bitmap
\*/

static INT_type ReadSize(Width, Height)
    OUT	INT_type	*Width;
    OUT	INT_type	*Height;
{
    INT_type		rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ReadSize");
    }
#endif

    if ((*Width = ReadInt()) == RG_ERROR_INT ||
	(*Height = ReadInt()) == RG_ERROR_INT) {

	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ReadSize");
    }
#endif

    return(rval);
}


/*\
 *  READINT:  Get an integer bounded by whitespace.
\*/

static INT_type ReadInt()
{
    INT_type	rval;
    INT_type	ch;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ReadInt");
    }
#endif

    rval = 0;
    ch = (INT_type) ' ';

    while (WHITESPACE(ch)) {
	ch = (*pbm_GetByte)(pbm_Parameter);
    }

    /*  MUST ADD OVERFLOW CHECKING HERE! @@@ */

    while (NUMBER(ch)) {
	rval = (rval * 10) + (INT_type) (ch - (INT_type) '0');
	ch = (*pbm_GetByte)(pbm_Parameter);
    }

    if (!WHITESPACE(ch)) {
	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ReadInt");
    }
#endif

    return(rval);
}


/*\
 *  READBITMAP:  Read a PBM bitmap.
\*/

static RASTER_type *ReadBitmap(raster)
    IN	RASTER_type	*raster;
{
    RASTER_type		*rval;
    INT_type		x;
    INT_type		y;
    INT_type		bit;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ReadBitmap");
    }
#endif

    /*  Add error checking and swapping!  @@@ */

    raster->rows = (ROW_t *) MEM_Malloc
			    (raster->height * sizeof(ROW_t));

    for (y = 0; y < raster->height; y++) {
	raster->rows[y].swap_tag = SWAP_BODY_PELS;
	raster->rows[y].swap_body.pels = 
		(BYTE_type *) MEM_Malloc (raster->width / 8 + 1);


	for (x = 0; x < raster->width; x++) {
	    if ((bit = ReadBit()) == RG_ERROR_INT) {
		rval = RG_ERROR_RASTER;
		RGError = RG_FORMBAD_err;
		goto LEAVE;
	    }
	    if (bit == (INT_type) '1') {	    /* x MOD 8 */
		raster->rows[y].swap_body.pels[x >> 3] |= 
						PEL_BitOnL[x & 7];
	    }
	}
    }

    rval = raster;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ReadBitmap");
    }
#endif

    return(rval);
}


/*\
 *  READBIT:  Read a bit from a PBM source.
\*/

static INT_type ReadBit()
{
    INT_type	rval;
    INT_type	ch;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ReadBit");
    }
#endif

    do {
	ch = (*pbm_GetByte)(pbm_Parameter);
	if (ch == (INT_type) '#') {
	    while (ch != (INT_type) '\n') {
        	ch = (*pbm_GetByte)(pbm_Parameter);
	    }
	}
    } while (WHITESPACE((INT_type) ch));

    if (ch != (INT_type) '0' && ch != (INT_type) '1') {
	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }

    rval = ch;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ReadBit");
    }
#endif

    return(rval);
}


/*\
 *  PBM_WRITERASTER:  Write a specified PBM raster to output.
\*/

INT_type PBM_WriteRaster(PutByte, parameter, format, raster)
    IN	INT_type	(*PutByte)();
    IN	POINTER_type	parameter;
    IN  FORMAT_type	*format;
    IN	RASTER_type	*raster;
{
    INT_type		rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PBM_WriteRaster");
    }
#endif

    pbm_PutByte = PutByte;
    pbm_Parameter = parameter;

    if ((*PutByte)(parameter,(BYTE_type)PBM_MAGIC1) == RG_ERROR_INT ||
	(*PutByte)(parameter,(BYTE_type)PBM_MAGIC2) == RG_ERROR_INT ||
	(*PutByte)(parameter,(BYTE_type) '\n')      == RG_ERROR_INT ||
	ETC_PutNumber(PutByte,parameter,raster->width,1)
						    == RG_ERROR_INT ||
	ETC_PutNumber(PutByte,parameter,raster->height,1)
						    == RG_ERROR_INT ||
	WriteBitmap(raster)			    == RG_ERROR_INT) {
	    
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PBM_WriteRaster");
    }
#endif

    return(rval);
}


static INT_type WriteBitmap(raster)
    RASTER_type	*raster;
{
    INT_type	rval;
    INT_type	x;
    INT_type	y;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("WriteBitmap");
    }
#endif

    for (y = 0; y < raster->height; y++) {
	if (RAST_SwapIn(raster,y) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	for (x = 0; x < raster->width; x++) {
	    (*pbm_PutByte)(pbm_Parameter,
		(GetBit(raster,x,y) == BIT_BLACK) ? '1' : '0');
	    if ((x & 63) == 0) {
		/* Guarantee no line longer than 64 bytes */
		(*pbm_PutByte)(pbm_Parameter, '\n');
	    }
	}
	/* Put blanks in between rows */
	(*pbm_PutByte)(pbm_Parameter, '\n');
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("WriteBitmap");
    }
#endif

    return(rval);
}




/*  End of file .c  */
