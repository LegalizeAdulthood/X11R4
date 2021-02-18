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
 *  File:   iog4.c
 *  Author: Andrew Werth
 *
 *  Main routines for Group 4 Rasters.
 *
 */

#define TK_TKI_mem
#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_list
#define TK_TKI_g3
#define TK_TKI_g4
#define TK_IMPL_g4
#define TK_GVARS_iog4

#include <rgsite.h>
#include <rgerr.h>
#include <rgmem.h>
#include <raster.h>
#include <rasio.h>
#include <iog3.h>
#include <iog32.h>
#include <iog33.h>
#include <iog4.h>
#include <rglist.h>

#undef TK_GVARS_iog4
#undef TK_IMPL_g4
#undef TK_TKI_g4
#undef TK_TKI_g3
#undef TK_TKI_list
#undef TK_TKI_rio
#undef TK_TKI_rast
#undef TK_TKI_mem

#ifndef USE_LONG_NAMES

#define Decode4		    decde4
#define RowDetectChange	    rwdtch
#define RowGetBit	    rwgtbt
#define RowSetBit	    rwstbt
#define UndoHorizMode	    unhrmo
#define UndoPassMode	    unpamo
#define UndoVertMode	    unvtmo
#define EndFaxBlock	    enfxbl
#define g4_CurPos	    g4cupo
#define g4_FirstRun	    g4frrn
#define g4_GetByte	    g4gtby
#define g4_Parameter	    g4para
#define g4_Width	    g4wdth

#endif

static ROW_t *Decode4();
static BYTE_type RowDetectChange();

/* External variable declarations */

extern BYTE_type    PEL_BitOnL[];   /* Bit values, 2^n	    */
extern BYTE_type    PEL_BitOnR[];
extern CodeWord_t   wt_make[];
extern CodeWord_t   bl_make[];
extern CodeWord_t   wt_term[];
extern CodeWord_t   bl_term[];
extern DecodeWord_t WhiteTreeTop[];
extern DecodeWord_t BlackTreeTop[];


/* Static Global Variables */

static BYTE_type    *BitOn;
static INT_type	    g4_Width;
static INT_type	    g4_CurPos;
static BOOL_type    g4_FirstRun;
static INT_type	    (*g4_GetByte)();
static POINTER_type g4_Parameter;




/*\
 *  G4_READRASTER:  Read in a Group 4 (T.6) raster.
\*/

RASTER_type *G4_ReadRaster(GetByte, Parameter, Format)
    IN	INT_type	    (*GetByte)();
    IN	POINTER_type	    Parameter;
    IN	FORMAT_GROUP4_type  *Format;
{
    RASTER_type	    *rval;
    LIST_t	    *List;	    /* List of rows in raster	*/
    ROW_t	    *Row;	    /* Current raster row	*/
    INT_type	    Rows;	    /* Number of rows in raster	*/
    INT_type	    Rownum;	    /* Count of rows		*/

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G4_ReadRaster");
    }
#endif

    Rows = 0;
    g4_Width = Format->width;
    
    switch (Format->msb) {	    /* Check bit ordering */
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

    g4_GetByte	 = GetByte;
    g4_Parameter = Parameter;

    Row = RG_NULL_ROW;
    rval = RAST_AllocRaster();

    /* Build a list of the rows, since the number of rows unknown */

    List = LST_MakeList();
    do {
	if ((Row = Decode4(Row)) == RG_ERROR_ROW) {
	    rval = RG_ERROR_RASTER;
	    goto LEAVE;
	}
	LST_AddElement(List, (POINTER_type) Row);
	if (Rows == 0) {
	    rval->width = g4_Width;
	}
	Rows++;
    } while (Row != RG_NULL_ROW);
    Rows -= 2;

    rval->height = Rows;
    rval->bytes = (rval->width + 7) / 8;
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
	Leaving("G4_ReadRaster");
    }
#endif

    return(rval);
}


/*\
 *  DECODE4:  Decode one line from a Group 4 raster and return
 *	      the row of bits in the internal raster format.
\*/

static ROW_t *Decode4(RefRow)
    IN	ROW_t	    *RefRow;
{
    ROW_t	    *rval;
    ROW_t	    *CurRow;
    DecodeWord_t    *Decode;
    BYTE_type	    Color;

    static BOOL_type	EndFaxBlock = BOOL_false;
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
        {    1,   -1,   -1, EDFB },	/* 24  */
        {    1,   -1,   -1, COMP },	/* 25  */
    };	    

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("Decode4");
    }
#endif

    if (EndFaxBlock == BOOL_true) {
	rval = RG_NULL_ROW;
	goto LEAVE;
    }

    if ((CurRow = (ROW_t *) MEM_Malloc(sizeof(ROW_t)))
				    == RG_ERROR_ROW) {
	rval = RG_ERROR_ROW;
	goto LEAVE;
    }
    CurRow->swap_tag = SWAP_BODY_PELS;
    CurRow->swap_body.pels = (BYTE_type *) MEM_Malloc
						 (g4_Width / 8 + 1);
    g4_CurPos = -1;
    g4_FirstRun = BOOL_true;
    Color = WHITE;    

    do {
	if ((Decode = G4_Traverse(decode)) == RG_ERROR_DecodeWord) {
	    rval = RG_ERROR_ROW;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}

	switch (Decode->value) {
	    
	    case P:
		if (UndoPassMode(CurRow, RefRow, Color)
					    == RG_ERROR_INT) {
		    rval = RG_ERROR_ROW;
		    goto LEAVE;
		}
		break;

	    case H:
		if (UndoHorizMode(CurRow, Color) == RG_ERROR_INT) {
		    rval = RG_ERROR_ROW;
		    goto LEAVE;	
		}
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
		break;

	    case EDFB:
		EndFaxBlock = BOOL_true;
		break;
	    default:
		rval = RG_ERROR_ROW;
		RGError = RG_INTERNAL_err;
		goto LEAVE;
	}
	g4_FirstRun = BOOL_false;

    } while (g4_CurPos < g4_Width && Decode->value != EDFB);
    rval = CurRow;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("Decode4");
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

    B1 = g4_CurPos;

    NewColor = RowDetectChange(RefLine, &B1);
    if (NewColor == Color && B1 < g4_Width) {
	RowDetectChange(RefLine, &B1);
    }
    if (B1 < g4_Width) {
        RowDetectChange(RefLine, &B1);
    }
    
    PutRun(CurLine, B1 - g4_CurPos + 1, Color);
    g4_CurPos--;

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

    if ((Run = G4_NextRun(Color)) == RG_ERROR_RUN) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    if (g4_FirstRun == BOOL_true) {
	Run->length++;
    }

    PutRun(CurLine, Run->length, Color);

    NewColor = (BYTE_type) 1 - Color;
    if ((Run = G4_NextRun(NewColor)) == RG_ERROR_RUN) {
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
    Pos = g4_CurPos;

    NewColor = RowDetectChange(RefLine, &Pos);
    if (NewColor == Color && Pos < g4_Width) {
	RowDetectChange(RefLine, &Pos);
    }

    rval = PutRun(CurLine, Pos - g4_CurPos + V - 5, Color);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("UndoVertMode");
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
    INT_type	RealRunLength;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (g4_CurPos < 0) {
	g4_CurPos++;
	RealRunLength = RunLength - 1;
    }
    else {
	RealRunLength = RunLength;
    }

    for (Length = 0; Length < RealRunLength; Length++) {
	if (g4_CurPos < g4_Width) {
	    if (RowSetBit(CurLine, g4_CurPos,Color) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		RGError = RG_INTERNAL_err;
		goto LEAVE;
	    }
	    g4_CurPos++;
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

    if (Line == RG_NULL_ROW) {	/* Treat first line specially */
	rval = BLACK;
	*StartPos = g4_Width;
	goto LEAVE;
    }

    if (*StartPos < 0) {	/* Get color at starting pos */
	Color = WHITE;
    }
    else {
	Color = (BYTE_type) RowGetBit(Line, *StartPos);
    }
    Pos = (*StartPos);

    do {
	Pos++;
    } while (Pos < g4_Width &&
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
    if (Line == RG_NULL_ROW) {
	if (Position < g4_Width) {
	    rval = (INT_type) 0;
	    goto LEAVE;
	}
	else {
	    rval = (INT_type) 1;
	    goto LEAVE;
	}
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

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

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
 *  NEXTRUN:  Return the next run length.
\*/

RUN_t *G4_NextRun(Color)
    IN	BYTE_type   Color;
{
    RUN_t	    *rval;
    DecodeWord_t    *tree;
    DecodeWord_t    *Decode1;
    DecodeWord_t    *Decode2;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G4_NextRun");
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
    if ((Decode1 = G4_Traverse(tree)) == RG_ERROR_DecodeWord) {
	rval = RG_ERROR_RUN;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }
    
    /* If above value was make-up code, now read terminal code. */
    if (Decode1->type == MAKE) {
	if ((Decode2 = G4_Traverse(tree)) == RG_ERROR_DecodeWord) {
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
	Leaving("G4_NextRun");
    }
#endif

    return(rval);
}


/*\
 *  G3_TRAVERSE:  Traverse the decode tree.
\*/

DecodeWord_t *G4_Traverse(tree)
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
        if (G4_GetBit() == 0) {
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
 *  G4_GETBIT:  Get one bit from the input source.
\*/

BYTE_type G4_GetBit()
{
    static INT_type	bitnum = 0;
    static INT_type	byte = 0;
    static BYTE_type	gotten = 0;
    BYTE_type		rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G4_GetBit");
    }
#endif

    if (gotten == 0) {				/* Get first byte */
	byte = (*g4_GetByte)(g4_Parameter);
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
	byte = (*g4_GetByte)(g4_Parameter);
	if (byte == RG_EOF) {
	    rval = (BYTE_type) 3;      /* Anything other than 0/1 */
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
/*    putc(rval+'0', stderr); */
    if (TraceRoutine(0)) {
	Leaving("G4_GetBit");
    }
#endif

    return(rval);
}


/*  End of file .c  */
