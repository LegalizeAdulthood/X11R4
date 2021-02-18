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
 *  File:   iog33.c
 *  Author: Andrew Werth
 *
 *  Input routines for Group 3 Two-Dimensional rasters.
 *
 */

#define TK_TKI_mem
#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_list
#define TK_TKI_g3
#define TK_IMPL_g3
#define TK_GVARS_iog33

#include <rgsite.h>
#include <rgerr.h>
#include <rgmem.h>
#include <raster.h>
#include <rasio.h>
#include <iog3.h>
#include <iog32.h>
#include <iog33.h>
#include <rglist.h>

#undef TK_GVARS_iog33
#undef TK_IMPL_g3
#undef TK_TKI_g3
#undef TK_TKI_list
#undef TK_TKI_rio
#undef TK_TKI_rast
#undef TK_TKI_mem

#ifndef USE_LONG_NAMES

#define DecodeTwo	    dcdtwo
#define HandleCompression   hndcom
#define RowDetectChange	    rwdtch
#define RowGetBit	    rwgtbi
#define RowSetBit	    rwstbi
#define UndoHorizMode	    unhomd
#define UndoPassMode	    unpsmd
#define UndoVertMode	    unvtmd

#define g3_CurAlloc	    g3cral
#define g3_CurPos	    g3crps
#define g3_FirstRun	    g3frrn
#define g3_MaxSize	    g3mxsz
#define g3_PrevRowSize	    g3prsz

#endif

static ROW_t *DecodeTwo();
static BYTE_type RowDetectChange();

extern BYTE_type    PEL_BitOnL[];   /* Bit values, 2^n	    */
extern BYTE_type    PEL_BitOnR[];


extern CodeWord_t   wt_make[];
extern CodeWord_t   bl_make[];
extern CodeWord_t   wt_term[];
extern CodeWord_t   bl_term[];
extern DecodeWord_t WhiteTreeTop[];
extern DecodeWord_t BlackTreeTop[];

static INT_type	    g3_CurAlloc = 0;
static INT_type	    g3_MaxSize	= DEFAULT_BUFFER;
static INT_type	    g3_PrevRowSize = 0;
static INT_type	    g3_CurPos   = -1;
static BOOL_type    g3_FirstRun = BOOL_true;

/*\
 *  G3_READBITMAP2:  Read a Group 3 raster stored in the
 *		     two-dimensional format.
\*/

RASTER_type *G3_ReadBitmap2(GetByte, parameter, format)
    IN	INT_type	    (*GetByte)();
    IN	POINTER_type	    parameter;
    IN	FORMAT_GROUP3_type  *format;
{
    RASTER_type	*rval;
    INT_type	rows;
    INT_type	i;
    LIST_t	*list;
    INT_type	NextRowCoding;
    INT_type	Width;
    ROW_t	*row;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_ReadBitmap");
    }
#endif

    rows = 0;
    g3_CurAlloc = g3_CurPos = g3_MaxSize = 0;

    for (i = 0; i < 11; i++) {		    /* Check EOL */
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
    if (G3_GetBit() != (BYTE_type) 1) {	    /* Check Tag = 1 */
	rval = RG_ERROR_RASTER;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }

    /* Allocate the raster */
    rval = RAST_AllocRaster();

    /* Build list of rows */
    list = LST_MakeList();
    NextRowCoding = ONE_DIM;
    do {
	/* Decode the row, either 1d or 2d */
	
	if (NextRowCoding == ONE_DIM) {
	    row = G3_DecodeOne(&Width);
	}
	else {
	    row = DecodeTwo(&Width, row);
	}
	if (row == RG_ERROR_ROW) {
	    rval = RG_ERROR_RASTER;
	    goto LEAVE;
	}

	LST_AddElement(list,(POINTER_type) row);
	rows++;

	if (g3_PrevRowSize == 0) {
	    g3_PrevRowSize = Width;
	    rval->width = Width;
	}

	/* Check the dimensions of next row */

	if (G3_GetBit() == (BYTE_type) 1) {
	    NextRowCoding = ONE_DIM;
	}
	else {
	    NextRowCoding = TWO_DIM;
	}

    } while (row != RG_NULL_ROW);
    rows -= 5;

    rval->height = rows;
    rval->bytes = (rval->width + 7) / 8;
    rval->rows = (ROW_t *) MEM_Malloc
			    ((INT_type) (rows * sizeof(ROW_t)));

    rows = 0;
    LST_MoveHead(list);
    while ((row = (ROW_t *) LST_NextElement(list)) != RG_NULL_ROW) {
	rval->rows[rows].swap_tag = row->swap_tag;
	rval->rows[rows].swap_body.pels = row->swap_body.pels;
	rows++;
    }

    LST_FreeList(list);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G3_ReadBitmap");
    }
#endif

    return(rval);
}


/*\
 *  DECODETWO:  Decode a two-dimensionally coded line.
\*/

static ROW_t *DecodeTwo(Width, RefRow)
    IN	INT_type    *Width;
    IN	ROW_t	    *RefRow;
{
    ROW_t	    *rval;
    ROW_t	    *CurRow;
    DecodeWord_t    *Decode;
    INT_type	    BufferPos;
    INT_type	    LastMode;
    INT_type	    PrevMode;
    BYTE_type	    Color;

    static INT_type Nulls = 0;
    static DecodeWord_t decode[] = {
	{    0,   14,    1,   -1 },	/* 0  */
        {    0,   12,    2,   -1 },	/* 1  */
	{    0,    5,    3,   -1 },	/* 2  */
        {    0,    4,    6,   -1 },	/* 3  */
        {    1,   -1,   -1,    0 },	/* 4  */
        {    1,   -1,   -1,    1 },	/* 5  */
        {    0,   10,    7,   -1 },	/* 6  */
        {    0,    8,   18,   -1 },	/* 7  */
        {    0,   17,    9,   -1 },	/* 8  */
        {    1,   -1,   -1,    2 },	/* 9  */
        {    0,   16,   11,   -1 },	/* 10  */
        {    1,   -1,   -1,    3 },	/* 11  */
        {    0,   15,   13,   -1 },	/* 12  */
        {    1,   -1,   -1,    4 },	/* 13  */
        {    1,   -1,   -1,    5 },	/* 14  */
        {    1,   -1,   -1,    6 },	/* 15  */
        {    1,   -1,   -1,    7 },	/* 16  */
        {    1,   -1,   -1,    8 },	/* 17  */
        {    0,   25,   19,   -1 },	/* 18  */
        {    0,   -1,   20,   -1 },	/* 19  */
        {    0,   -1,   21,   -1 },	/* 20  */
        {    0,   -1,   22,   -1 },	/* 21  */
        {    0,   -1,   23,   -1 },	/* 22  */
        {    0,   24,   23,   -1 },	/* 23  */
        {    1,   -1,   -1, EOL2 },	/* 24  */
        {    1,   -1,   -1, COMP },	/* 25  */
    };	    

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("DecodeTwo");
    }
#endif

    if ((CurRow = (ROW_t *) MEM_Malloc(sizeof(ROW_t))) 
						== RG_ERROR_ROW) {
	rval = RG_ERROR_ROW;
	goto LEAVE;
    }
    CurRow->swap_tag = SWAP_BODY_PELS;

    g3_CurAlloc = 0;
    g3_CurPos = -1;
    Color = WHITE;
    g3_FirstRun = BOOL_true;
    
    do {
        if ((Decode = G3_Traverse(decode)) == RG_ERROR_DecodeWord) {
	    rval = RG_ERROR_ROW;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}

	if (Decode->value != EOL2) {
	    PrevMode = Decode->value;
	}
	switch (Decode->value) {

	    case P:	    
		if (UndoPassMode(CurRow, RefRow, Color) 
						== RG_ERROR_INT) {
		    rval = RG_ERROR_ROW;
		    goto LEAVE;
		}
		Nulls = (INT_type) 0;
		break;

	    case H:
		if (UndoHorizMode(CurRow, Color) == RG_ERROR_INT) {
		    rval = RG_ERROR_ROW;
		    goto LEAVE;	
		}
		Nulls = (INT_type) 0;
		break;

	    case VL3:
	    case VL2:
	    case VL1:
	    case V0:
	    case VR1:
	    case VR2:
	    case VR3:
		if (UndoVertMode(CurRow, RefRow, Color, 
				    Decode->value) == RG_ERROR_INT) {
		    rval = RG_ERROR_ROW;
		    goto LEAVE;
		}
		Color = (BYTE_type) 1 - Color;
		Nulls = (INT_type) 0;
		break;

	    case EOL2:
		LastMode = PrevMode;
		Nulls++;
		break;

	    case COMP:
		Color = HandleCompression(&g3_CurPos);
		if (Color == (BYTE_type) RG_ERROR_INT) {
		    rval = RG_ERROR_ROW;
		    goto LEAVE;
		}
		break;
	    default:
		rval = RG_ERROR_ROW;
		RGError = RG_INTERNAL_err;
		goto LEAVE;
	
	}
	g3_FirstRun = BOOL_false;

    } while (Decode->value != EOL2);

    if (g3_CurPos < g3_PrevRowSize) {
	if (LastMode != V0) {
	    if (Color == WHITE) {
		PutRun(CurRow, g3_PrevRowSize - g3_CurPos, WHITE);
	    }
	}
	else {
	    if (Color == BLACK) {
		PutRun(CurRow,g3_PrevRowSize-g3_CurPos, 1 - Color);
	    }
	}
    }

    *Width = g3_CurPos;
    BufferPos = g3_CurPos / 8 + 1;
    if (g3_CurAlloc > BufferPos) {
	CurRow->swap_body.pels = (BYTE_type *)
		MEM_Realloc(CurRow->swap_body.pels, BufferPos);
	g3_MaxSize = BufferPos;
    }
    rval = CurRow;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("DecodeTwo");
    }
#endif

    return(rval);
}


/*\
 *  UNDOPASSMODE:  Decodes a Pass Mode section.
\*/

static INT_type UndoPassMode(CurLine, RefLine, Color)
    IN	ROW_t	    *CurLine;
    IN	ROW_t	    *RefLine;
    IN	BYTE_type    Color;
{
    INT_type	rval;
    INT_type	B1;
    BYTE_type	NewColor;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("UndoPassMode");
    }
#endif

    B1 = g3_CurPos;

    NewColor = RowDetectChange(RefLine, &B1);
    if (NewColor == Color && B1 < g3_PrevRowSize) {
	RowDetectChange(RefLine, &B1);
    }
    if (B1 < g3_PrevRowSize) {
        RowDetectChange(RefLine, &B1);
    }
    
    PutRun(CurLine, B1 - g3_CurPos + 1, Color);
    g3_CurPos--;

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("UndoPassMode");
    }
#endif

    return(rval);
}


/*\
 *  UNDOHORIZMODE:  Decode a Horizontal Mode section.
\*/

static INT_type UndoHorizMode(CurLine, Color)
    IN	ROW_t	    *CurLine;
    IN	BYTE_type   Color;
{
    INT_type	rval;
    RUN_t	*Run;
    BYTE_type	NewColor;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("UndoHorizMode");
    }
#endif

    if ((Run = G3_NextRun(Color)) == RG_ERROR_RUN) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    if (g3_FirstRun == BOOL_true) {
	Run->length++;
    }
    PutRun(CurLine, Run->length, Color);

    NewColor = (BYTE_type) 1 - Color;
    if ((Run = G3_NextRun(NewColor)) == RG_ERROR_RUN) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    PutRun(CurLine, Run->length, NewColor);

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("UndoHorizMode");
    }
#endif

    return(rval);
}


/*\
 *  UNDOVERTMODE:  Decode a Vertical Mode section.
\*/

static INT_type UndoVertMode(CurLine, RefLine, Color, V)
    IN ROW_t	    *CurLine;
    IN ROW_t	    *RefLine;
    IN BYTE_type    Color;
    IN INT_type	    V;
{
    INT_type	rval;
    INT_type	Pos;
    BYTE_type	NewColor;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("UndoVertMode");
    }
#endif

    /* Detect B1 */
    Pos = g3_CurPos;

    NewColor = RowDetectChange(RefLine, &Pos);
    if (NewColor == Color && Pos < g3_PrevRowSize) {
	RowDetectChange(RefLine, &Pos);
    }

    rval = PutRun(CurLine, Pos - g3_CurPos + V - 5, Color);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("UndoVertMode");
    }
#endif

    return(rval);
}


/*\
 *  ROWDETECTCHANGE:  Find a changing element in a row of pels.
\*/

static BYTE_type RowDetectChange(Line, StartPos)
    IN	    ROW_t       *Line;
    IN OUT  INT_type	*StartPos;
{
    BYTE_type	rval;
    BYTE_type	Color;
    INT_type	Pos;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("RowDetectChange");
    }
#endif

    if (*StartPos < 0) {
	Color = WHITE;
    }
    else {
        Color = (BYTE_type) RowGetBit(Line, *StartPos);
    }
    Pos = (*StartPos);

    do {
	Pos++;
    } while (Pos < g3_PrevRowSize &&
	     Color == (BYTE_type) RowGetBit(Line, Pos));

    *StartPos = Pos;

    rval = (BYTE_type) 1 - Color;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("RowDetectChange");
    }
#endif

    return(rval);
}


/*\
 *  ROWGETBIT:  Get the status of the specified pel from within
 *		the specified row.
\*/

static INT_type RowGetBit(Line, Position)
    IN	ROW_t	    *Line;
    IN	INT_type    Position;
{
    INT_type	    rval;
    INT_type	    Byte;
    INT_type	    Bit;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (Position == -1) {
	Position = 0;
    }
    Byte = Position >> DIV8;
    Bit =  Position & MOD8;
    if ((Line->swap_body.pels[Byte] & PEL_BitOnL[Bit]) != 0) {
	rval = (INT_type) 1;
    }
    else {
	rval = (INT_type) 0;
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
 *  ROWSETBIT:  Set the specified bit in the specified row.
\*/

static INT_type RowSetBit(Line, Position, Color)
    IN	ROW_t	    *Line;
    IN  INT_type    Position;
    IN	BYTE_type   Color;
{
    INT_type	    rval;
    INT_type	    Bit;
    INT_type	    Byte;
    INT_type	    BufferPos;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    BufferPos = Position / 8 + 1;

    if (g3_CurAlloc == 0) {
	if (BufferPos <= g3_MaxSize) {
	    Line->swap_body.pels = 
			    (BYTE_type *) MEM_Malloc(g3_MaxSize);
	    g3_CurAlloc = g3_MaxSize;
	}
	else {
	    Line->swap_body.pels = 
			    (BYTE_type *) MEM_Malloc(BufferPos);
	    g3_CurAlloc = g3_MaxSize = BufferPos;
	}
    }

    while (BufferPos > g3_CurAlloc) {
	g3_CurAlloc += DEFAULT_INCREMENT;
	Line->swap_body.pels = (BYTE_type *)
		MEM_Realloc(Line->swap_body.pels, g3_CurAlloc);
    }

    Byte = Position >> DIV8;
    Bit = Position  & MOD8;
    if (Bit == 0) {
	Line->swap_body.pels[Byte] = (BYTE_type) 0;
    }

    if (Color == BLACK) {
        Line->swap_body.pels[Byte] |= PEL_BitOnL[Bit];
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
 *  HANDLECOMPRESSION:  Enter the uncompressed mode, where code
 *			words very nearly represent exact bit
 *			patterns.  Returns the tag bit following
 *			the exit from uncompressed mode -- this
 *			bit determines the color of the next run.
\*/

static INT_type HandleCompression(CurLine,Position)
    ROW_t	*CurLine;
    INT_type	*Position;
{
    INT_type	rval;
    INT_type	i;
    INT_type	zeroes;
    BYTE_type	bit;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("HandleCompression");
    }
#endif

    /* Check for three 1 bits to enter uncompressed mode */

    for (i = 0; i < 3; i++) {
	if (G3_GetBit() != (BYTE_type) 1) {
	    rval = RG_ERROR_INT;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
    }


    /* Start reading uncompressed mode */

    /* Accumulate zeroes until a one is reached.  At that point:
     *	 -  zeroes <  6 means an image pattern has been found
     *   -  zeroes >= 6 means an exit pattern has been found
     */

    zeroes = 0;

    do {
	bit = G3_GetBit();
	if (bit == (BYTE_type) 0) {
	    zeroes++;
	}
	else {
	    if (zeroes < 6) {
	        for (i = 0; i < zeroes; i++) {
		    (*Position)++;
		}
		if (zeroes < 5) {
		    RowSetBit(CurLine, *Position, BLACK);
		    (*Position)++;
		}
		zeroes = 0;
	    }
	    else {
		for (i = 0; i < zeroes - 6; i++) {
		    (*Position)++;
		}
		zeroes = -zeroes;
	    }
	}
    } while (zeroes >= 0);

    rval = (INT_type) G3_GetBit();  /* Get and return tag bit */


LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("HandleCompression");
    }
#endif

    return(rval);
}


/*\
 *  PUTRUN:  Put a run length to the specified row.
\*/

static INT_type PutRun(CurLine, RunLength, Color)
    ROW_t	*CurLine;
    INT_type	RunLength;
    BYTE_type	Color;
{
    INT_type	rval;
    INT_type	Length;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    for (Length = 0; Length < RunLength; Length++) {
	if (g3_CurPos < g3_PrevRowSize) {
	    if (RowSetBit(CurLine, g3_CurPos,Color) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		RGError = RG_INTERNAL_err;
		goto LEAVE;
	    }
	    g3_CurPos++;
	}
	else {
	    break;
	}
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


/*  End of file .c  */
