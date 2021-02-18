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
 *  File:   ioobm.c
 *  Author: Andrew Werth
 *
 *  Input and Output routines for the ODA Bitmap Encoding Scheme.
 *
 */

#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_list
#define TK_TKI_obm
#define TK_IMPL_obm
#define TK_GVARS_ioobm

#include <rgsite.h>
#include <raster.h>
#include <rasio.h>
#include <rgerr.h>
#include <rglist.h>
#include <ioobm.h>

#undef TK_TKI_rast
#undef TK_TKI_rio
#undef TK_TKI_list
#undef TK_TKI_obm
#undef TK_IMPL_obm
#undef TK_GVARS_ioobm

#ifndef USE_LONG_NAMES

#define obm_GetByte	obmgby
#define obm_Parameter	obmpar
#define obm_Width	obmwdt

#endif


static INT_type	    (*obm_GetByte)();
static POINTER_type obm_Parameter;
static INT_type	    obm_Width;

extern BYTE_type    PEL_BitOnL[];
extern BYTE_type    PEL_BitOffL[];


/*\
 *  OBM_WRITERASTER:  Write a raster in ODA Bitmap Encoding Scheme.
\*/

INT_type OBM_WriteRaster(PutByte, Parameter, format, raster)
    IN	INT_type	    (*PutByte)();
    IN	POINTER_type	    Parameter;
    IN	FORMAT_ODABM_type   *format;
    IN	RASTER_type	    *raster;
{
    INT_type		    rval;
    INT_type		    Modulo;
    REGISTER BYTE_type	    LastByte;
    REGISTER INT_type	    y;
    REGISTER INT_type	    x;
    
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("OBM_WriteRaster");
    }
#endif

    obm_Width = ((raster->width + 8) / 8);

    Modulo = format->width % 8;

    for (y = 0; y < raster->height; y++) {
	if (RAST_SwapIn(raster,y) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
        LastByte = raster->rows[y].swap_body.pels[obm_Width - 1];
	if (Modulo != 0) {
	    for (x = Modulo; x < 8; x++) {
		LastByte &= PEL_BitOffL[x];
	    }
	}
	for (x = 0; x < obm_Width - 1; x++) {
	    if ((*PutByte)(Parameter,
		 raster->rows[y].swap_body.pels[x]) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}

	if ((*PutByte)(Parameter, LastByte) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("OBM_WriteRaster");
    }
#endif

    return(rval);
}


/*\
 *  OBM_READREASTER:  Read a raster stored in the ODA Bitmap
 *		      Encoding Scheme.
\*/

RASTER_type *OBM_ReadRaster(GetByte, parameter, format)
    IN	INT_type	    (*GetByte)();
    IN	POINTER_type	    parameter;
    IN	FORMAT_ODABM_type   *format;
{
    RASTER_type	    *rval;
    ROW_t	    *Row;
    LIST_t	    *List;
    INT_type	    Rows;
    INT_type	    Rownum;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("OBM_ReadRaster");
    }
#endif

    obm_GetByte = GetByte;
    obm_Parameter = parameter;
    obm_Width = ((format->width + 8) / 8);

    Rows = 0;
    List = LST_MakeList();
    do {
	if ((Row = Decode()) == RG_ERROR_ROW) {
	    rval = RG_ERROR_RASTER;
	    goto LEAVE;
	}
	LST_AddElement(List,(POINTER_type) Row);
	Rows++;
    } while (Row != RG_NULL_ROW);
    Rows -= 2;

    rval = RAST_AllocRaster();
    rval->height = Rows;
    rval->width  = format->width;
    rval->bytes  = (format->width + 7) / 8;
    rval->rows = (ROW_t *) MEM_Malloc
			    ((INT_type) (Rows * sizeof(ROW_t)));

    Rownum = 0;
    LST_MoveHead(List);
    while ((Row = (ROW_t *) LST_NextElement(List)) != RG_NULL_ROW) {
	rval->rows[Rownum].swap_tag = Row->swap_tag;
	rval->rows[Rownum].swap_body.pels = Row->swap_body.pels;
	Rownum++;
    }

    LST_FreeList(List);


LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("OBM_ReadRaster");
    }
#endif

    return(rval);
}


/*\
 *  DECODE:  Decode a line of ODA Bitmap Encoding Scheme.
\*/

static ROW_t *Decode()
{
    ROW_t	    *rval;
    INT_type	    x;
    static INT_type Byte;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("Decode");
    }
#endif

    if (Byte == RG_EOF) {
	rval = RG_NULL_ROW;
	goto LEAVE;
    }
    rval = (ROW_t *) MEM_Malloc((INT_type) sizeof(ROW_t));
    if (rval == (ROW_t *) RG_ERROR_INT) {
	rval = RG_ERROR_ROW;
	goto LEAVE;
    }

    rval->swap_tag = SWAP_BODY_PELS;
    rval->swap_body.pels = (BYTE_type *)
		MEM_Malloc((INT_type) (obm_Width));

    if (rval->swap_body.pels == (BYTE_type *) RG_ERROR_INT) {
	MEM_Free(rval);
	rval = RG_ERROR_ROW;
	goto LEAVE;
    }

    for (x = 0; x < obm_Width; x++) {
	Byte = (*obm_GetByte)(obm_Parameter);
	if (Byte == (INT_type) RG_EOF) {
	    goto LEAVE;
	}
	rval->swap_body.pels[x] = Byte;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("Decode");
    }
#endif

    return(rval);
}



/*  End of file .c  */
