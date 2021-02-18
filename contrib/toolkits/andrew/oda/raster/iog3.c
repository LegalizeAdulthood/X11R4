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
 *  File:   iog3.c
 *  Author: Andrew Werth
 *
 *  Input/output routines for Group 3 (T.4) rasters.
 *
 */

#define TK_TKI_mem
#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_list
#define TK_TKI_g3
#define TK_IMPL_g3
#define TK_GVARS_iog3

#include <rgsite.h>
#include <rgerr.h>
#include <rgmem.h>
#include <raster.h>
#include <rasio.h>
#include <iog3.h>
#include <iog32.h>
#include <iog33.h>
#include <rglist.h>

#undef TK_GVARS_iog3
#undef TK_IMPL_g3
#undef TK_TKI_g3
#undef TK_TKI_list
#undef TK_TKI_rio
#undef TK_TKI_rast
#undef TK_TKI_mem

#ifndef USE_LONG_NAMES

#define PutLength	putlen
#define WriteBitmap	wrbtmp
#define g3_GetByte	g3gtby
#define g3_LineBits	g3lnbt
#define g3_Parameter	g3para
#define g3_PutByte	g3ptby

#endif

static BYTE_type    *BitOn;	    /* Current BitOn array  */

static INT_type	    (*g3_GetByte)();
static INT_type	    (*g3_PutByte)();
static INT_type	    g3_LineBits;    /* # Bits since last EOL */
static POINTER_type g3_Parameter;

extern BYTE_type    PEL_BitOnL[];   /* Bit values, 2^n	    */
extern BYTE_type    PEL_BitOnR[];


extern CodeWord_t   wt_make[];
extern CodeWord_t   bl_make[];
extern CodeWord_t   wt_term[];
extern CodeWord_t   bl_term[];
extern DecodeWord_t WhiteTreeTop[];
extern DecodeWord_t BlackTreeTop[];

/*\
 *  G3_READRASTER:  Read in a Group 3 (T.4) raster.
\*/

RASTER_type *G3_ReadRaster(GetByte, parameter, format)
    IN	INT_type	    (*GetByte)();
    IN	POINTER_type	    parameter;
    IN	FORMAT_GROUP3_type  *format;
{
    RASTER_type	    *rval;

    INT_type	    i;
    INT_type	    rownum;
    INT_type	    rows;
    INT_type	    Width;
    ROW_t	    *row;
    LIST_t	    *list;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_ReadRaster");
    }
#endif

    /* Check dimensions */

    switch (format->msb) {
	case LEFTBIT:
	    BitOn = PEL_BitOnL;
	    break;
	case RIGHTBIT:
	    BitOn = PEL_BitOnR;
	    break;
	case UNKNOWN:
	    rval = RG_ERROR_RASTER;
	    RGError = RG_UNIMPL_err;
	    goto LEAVE;
	default:
	    rval = RG_ERROR_RASTER;
	    RGError = RG_BADPARM_err;
	    break;
    }

    g3_GetByte    = GetByte;
    g3_Parameter  = parameter;

    /* Check coding dimension */
    if (format->k != 1) {
	rval = G3_ReadBitmap2(GetByte, parameter, format);
	goto LEAVE;
    }

    rows = 0;

    /* Check for EOLN to start */
    for (i = 0; i < 11; i++) {
	if (G3_GetBit() != (BYTE_type) 0) {
	    rval = RG_ERROR_RASTER;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
    }
    if (G3_GetBit() != (BYTE_type) 1) {
	rval = RG_ERROR_RASTER;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }

    rval = RAST_AllocRaster();

    /* Build a list of rows */

    list = LST_MakeList();
    do {
	row = G3_DecodeOne(&Width);
	LST_AddElement(list,(POINTER_type) row);
	rows++;
	if (Width > rval->width) {
	    rval->width = Width;
	}
    } while (row != RG_NULL_ROW);
    rows -= 5;


    rval->height = rows;
    rval->bytes = (rval->width + 7)/8;
    rval->rows = (ROW_t *) MEM_Malloc
			    ((INT_type) (rows * sizeof(ROW_t)));
    rownum = 0;

    /* Copy rows from the list into the raster's array */

    LST_MoveHead(list);
    while ((row = (ROW_t *) LST_NextElement(list)) != RG_NULL_ROW) {
	rval->rows[rownum].swap_tag = 
					row->swap_tag;
	rval->rows[rownum].swap_body.pels = row->swap_body.pels;
	rownum++;
    }

    LST_FreeList(list);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G3_ReadRaster");
    }
#endif

    return(rval);
}


/*\
 *  G3_DECODEONE:  Decode one line from a T.4 raster and return
 *		   the row of bits in internal raster format.
\*/

ROW_t *G3_DecodeOne(LineSize)
    OUT	INT_type    *LineSize;
{
    ROW_t	    *rval;
    RUN_t	    *Run;
    RUN_t	    *Current;
    LIST_t	    *List;	    /* List of run lengths	*/
    BYTE_type	    Color;	    /* Current run color	*/
    INT_type	    Bitnum;	    /* Current bit in row	*/
    INT_type	    RunLength;   
    INT_type	    byte;	    /* Current byte in row	*/
    INT_type	    bit;	    /* Current bit in byte	*/

    static INT_type Nulls = 0;	    /* Number of null lines	*/

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_DecodeOne");
    }
#endif

    Color =	WHITE;

    List = LST_MakeList();

    /* Build up a linked list of run lengths */

    *LineSize = 0;
    Nulls++;
    while ((Run = G3_NextRun(Color)), Run->type != EOLN) {
	Nulls = 0;
	if (Run == RG_ERROR_RUN) {
	    rval = RG_ERROR_ROW;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
	LST_AddElement(List, (POINTER_type) Run);
	*LineSize += Run->length;
	Color = 1 - Color;
    }

    if (Nulls == 5) {	    /* End of File detected */
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
		MEM_Malloc((INT_type) ((*LineSize / 7) + 2));

    if (rval->swap_body.pels == (BYTE_type *) RG_ERROR_INT) {
	MEM_Free(rval);
	rval = RG_ERROR_ROW;
	goto LEAVE;
    }

    Bitnum = (INT_type) 0;
    LST_MoveHead(List);
    while ((Current = (RUN_t *) LST_NextElement(List))
			    != RG_NULL_RUN) {

	/* Turn ON the BLACK bits in the current run */
	
	for (RunLength = Current->length; 
	    RunLength > 0; RunLength--) {
	    if ((bit = (Bitnum & MOD8)) == 0) {
		byte = Bitnum >> DIV8;
		if (RunLength >= 8) {
		    if (Current->type == BLTERM) {
			rval->swap_body.pels[byte] = (BYTE_type) 0xff;
		    }
		    else {
			rval->swap_body.pels[byte] = (BYTE_type) 0x00;
		    }
		    RunLength -= 7;
		    Bitnum += 7;
		}
		else {
		    if (Current->type == BLTERM) {
			rval->swap_body.pels[byte] = PEL_BitOnL[0];
		    }
		    else {
			rval->swap_body.pels[byte] = (BYTE_type) 0;
		    }
		}
	    }
	    else {
		if (Current->type == BLTERM) {
		    rval->swap_body.pels[byte] |= PEL_BitOnL[bit];
		}
	    }
	    Bitnum++;
	}
    }

    LST_FreeList(List);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G3_DecodeOne");
    }
#endif

    return(rval);
}


/*\
 *  NEXTRUN:  Return the next run length.
\*/

RUN_t *G3_NextRun(Color)
    IN	BYTE_type   Color;
{
    RUN_t	    *rval;
    DecodeWord_t    *tree;
    DecodeWord_t    *Decode1;
    DecodeWord_t    *Decode2;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_NextRun");
    }
#endif

    rval = (RUN_t *) MEM_Malloc((INT_type) sizeof(RUN_t));
    rval->length = 0;

    if (Color == BLACK) {	/* Select which tree to traverse */
	tree = BlackTreeTop;
    }
    else {
	tree = WhiteTreeTop;
    }

    if (tree == RG_NULL) {
	rval = RG_ERROR_RUN;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }

    /* Traverse the tree until a leaf is found -> the runlength */
    if ((Decode1 = G3_Traverse(tree)) == RG_ERROR_DecodeWord) {
	rval = RG_ERROR_RUN;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }
    
    /* If above value was make-up code, now read terminal code. */
    if (Decode1->type == MAKE) {
	if ((Decode2 = G3_Traverse(tree)) == RG_ERROR_DecodeWord) {
	    rval = RG_ERROR_RUN;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
	rval->length = Decode1->value + Decode2->value;
	rval->type = Decode2->type;
    }
    else {
	rval->length = Decode1->value;
	rval->type = Decode1->type;
    }


LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G3_NextRun");
    }
#endif

    return(rval);
}


/*\
 *  G3_TRAVERSE:  Traverse the decode tree.
\*/

DecodeWord_t *G3_Traverse(tree)
    IN	DecodeWord_t	*tree;
{
    DecodeWord_t	*rval;
    INT_type		node;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    node = 0;
    do {
        if (G3_GetBit() == 0) {
	    node = tree[node].zero;
	}
	else {
	    node = tree[node].one;
	}
	if (node == -1) {
	    rval = RG_ERROR_DecodeWord;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
    } while (tree[node].type == INTERNAL);

    rval = &tree[node];

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


/*\
 *  G3_GETBIT:  Get one bit from the input source.
\*/

BYTE_type G3_GetBit()
{
    static INT_type	bitnum = 0;
    static INT_type	byte = 0;
    static BYTE_type	gotten = 0;
    BYTE_type		rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_GetBit");
    }
#endif

    if (gotten == 0) {				/* Get first byte */
	byte = (*g3_GetByte)(g3_Parameter);
	if (byte == RG_EOF) {
	    rval = (BYTE_type) 3;      /* Anything other than 0/1 */
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
	gotten = 1;
    }

    if ((byte & BitOn[bitnum]) == 0) {
	rval = (BYTE_type) 0;
    }
    else {
	rval = (BYTE_type) 1;
    }

    bitnum = (bitnum + 1) & MOD8;
    if (bitnum == 0) {				/* Get next byte  */
	byte = (*g3_GetByte)(g3_Parameter);
	if (byte == RG_EOF) {
	    rval = (BYTE_type) 3;      /* Anything other than 0/1 */
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
/*    putchar(rval+'0'); */
    if (TraceRoutine(0)) {
	Leaving("G3_GetBit");
    }
#endif

    return(rval);
}


/********************  OUTPUT  *********************/


/*\
 *  G3_WRITERASTER:  Write a group 3 raster.
\*/

INT_type G3_WriteRaster(PutByte, Parameter, format, raster)
    IN	INT_type	    (*PutByte)();
    IN	POINTER_type	    Parameter;
    IN  FORMAT_GROUP3_type  *format;
    IN	RASTER_type	    *raster;
{
    INT_type		    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_WriteRaster");
    }
#endif

    g3_PutByte = PutByte;
    g3_Parameter = Parameter;

    /* Determine how the bits shall be stored */

    switch (format->msb) {
	case LEFTBIT:
	    BitOn = PEL_BitOnL;
	    break;
	case RIGHTBIT:
	    BitOn = PEL_BitOnR;
	    break;
	case UNKNOWN:
	    rval = RG_ERROR_INT;
	    RGError = RG_UNIMPL_err;
	    goto LEAVE;
	default:
	    rval = RG_ERROR_INT;
	    RGError = RG_BADPARM_err;
	    break;
    }

    /* Determine if one- or two-dimensional coding is used */

    if (format->k == 1) {
        if (WriteBitmap(raster,format)  == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }
    else {
	if (format->k != 2 && format->k != 4) {
	    rval = RG_ERROR_INT;
	    RGError = RG_BADPARM_err;
	    goto LEAVE;
	}
	if (G3_WriteBitmap2(raster,format) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G3_WriteRaster");
    }
#endif

    return(rval);
}

  

  
/*\
 *  WRITEBITMAP:  Write a Group 3 raster to output using
 *		  one-dimensional coding.
\*/

static INT_type WriteBitmap(raster, format)
    RASTER_type		*raster;
    FORMAT_GROUP3_type	*format;
{
    INT_type		rval;
    INT_type		y;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("WriteBitmap");
    }
#endif

    if (G3_PutEOL() == RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    for (y = 0; y < raster->height; y++) {
	G3_EncodeOne(raster,y,format->pad_width);
    }

    /* End with 6 EOLs */

    for (y = 0; y < 5; y++) {
	if (G3_PutEOL() == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }
    G3_PutBit(2);		    /* Flush last byte */

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("WriteBitmap");
    }
#endif

    return(rval);
}


/*\
 *  ENCODEONE:  Encode a line in one-dimensional format.
\*/

INT_type G3_EncodeOne(raster, y, PadWidth)
    RASTER_type	    *raster;
    INT_type	    y;
    INT_type	    PadWidth;
{
    INT_type	    rval;
    INT_type	    x;
    INT_type	    Color;
    INT_type	    Run;
    INT_type	    RealPad;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    Color = WHITE;
    Run = 0;
    g3_LineBits = 0;

    if (PadWidth < 0) {		    /* Means 2d line if negative */
	RealPad = (-PadWidth) - 1;
    }
    else {
	RealPad = PadWidth;
    }

    for (x = 0; x < raster->width; x++) {
        if (GetBit(raster,x,y) == Color) {
	    Run++;
	}
	else {
	    if (G3_PutRunLength(Color,Run) == RG_ERROR_INT) {
		goto LEAVE;
	    }
	    Color = 1 - Color;
	    Run = 1;
	}			
    }
    if (G3_PutRunLength(Color,Run) == RG_ERROR_INT) {
	goto LEAVE;
    }

    if (g3_LineBits < RealPad - 12) {
	for (x = g3_LineBits; x < RealPad - 12; x++) {
	    if (G3_PutBit(0) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
    }


    if (G3_PutEOL() == RG_ERROR_INT) {
        rval = RG_ERROR_INT;
        goto LEAVE;
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
 *  G3_PUTRUNLENGTH:  Write the coded bits representing the 
 *		      specified run length.  Returns the number of
 *		      bits written.
\*/

INT_type G3_PutRunLength(color,run)
    INT_type	color;
    INT_type	run;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_PutRunLength");
    }
#endif

    rval = g3_LineBits;

    if (color == WHITE) {
	if (run > (INT_type) 63) {
	    if (PutLength(wt_make,(run >> DIV64) -1) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
	if (PutLength(wt_term,run & MOD64) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }
    else if (color == BLACK) {
	if (run > (INT_type) 63) {
	    if (PutLength(bl_make,(run >> DIV64) -1) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
	if (PutLength(bl_term,run & MOD64) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }
    else {
	rval = RG_ERROR_INT;
	RGError = RG_INTERNAL_err;
	goto LEAVE;
    }
    
    rval = g3_LineBits - rval;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G3_PutRunLength");
    }
#endif

    return(rval);
}


/*\
 *  PUTLENGTH:  Write the code word in the specified tree of the
 *		specified length.  Return number of bits written.
\*/

static INT_type PutLength(array, index)
    CodeWord_t	*array;
    INT_type	index;
{
    INT_type	rval;
    INT_type	pattern;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PutLength");
    }
#endif

    pattern = array[index].pattern;
    for (rval = 0; rval < array[index].length; rval++) {
	if (G3_PutBit((BYTE_type) (pattern & 01)) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	pattern = (pattern >> 1);
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PutLength");
    }
#endif

    return(rval);
}


/*\
 *  G3_PUTEOL:  Put an End Of Line marker to the output.
\*/

INT_type G3_PutEOL()
{
    INT_type	rval;
    INT_type	count;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_PutEOL");
    }
#endif

    for (count = 0; count < 11; count++) {
	if (G3_PutBit((BYTE_type) 0) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }
    rval = G3_PutBit((BYTE_type) 1);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G3_PutEOL");
    }
#endif

    return(rval);
}

/*\
 *  G3_PUTBIT:  Put one bit to the output.
\*/

INT_type G3_PutBit(bit)
    BYTE_type	bit;
{
    INT_type	        rval;
    static BYTE_type	byte = (BYTE_type) 0;
    static BYTE_type	bitnum = (BYTE_type) 0;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_PutBit");
    }
#endif

    if (bit == (BYTE_type) 2) {
	rval = (*g3_PutByte)(g3_Parameter, byte);
	goto LEAVE;
    }

    if (bit == (BYTE_type) 1) {
	byte |= BitOn[bitnum];
    }
    bitnum++;
    if (bitnum == (BYTE_type) 8) {
	rval = (*g3_PutByte)(g3_Parameter, byte);
	byte = (BYTE_type) 0;
	bitnum = (BYTE_type) 0;
    }
    else {
	rval = RG_SUCCESS_INT;
    }
    g3_LineBits++;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G3_PutBit");
    }
#endif

    return(rval);
}




/*\
 *  G4_WRITERASTER:  Write a group 4 raster.
\*/

INT_type G4_WriteRaster(PutByte, Parameter, format, raster)
    IN	INT_type	    (*PutByte)();
    IN	POINTER_type	    Parameter;
    IN  FORMAT_GROUP4_type  *format;
    IN	RASTER_type	    *raster;
{
    INT_type		    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G4_WriteRaster");
    }
#endif

    g3_PutByte = PutByte;
    g3_Parameter = Parameter;

    /* Determine how the bits shall be stored */

    switch (format->msb) {
	case LEFTBIT:
	    BitOn = PEL_BitOnL;
	    break;
	case RIGHTBIT:
	    BitOn = PEL_BitOnR;
	    break;
	case UNKNOWN:
	    rval = RG_ERROR_INT;
	    RGError = RG_UNIMPL_err;
	    goto LEAVE;
	default:
	    rval = RG_ERROR_INT;
	    RGError = RG_BADPARM_err;
	    break;
    }

    if (G4_WriteBitmap(raster,format) == RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G4_WriteRaster");
    }
#endif

    return(rval);
}



/*  End of file .c  */
