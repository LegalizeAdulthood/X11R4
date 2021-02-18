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
 *  File:   iog32.c
 *  Author: Andrew Werth
 *
 *  Two dimensional coding/decoding of Group 3 Rasters.
 *
 */

#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_list
#define TK_TKI_g3
#define TK_IMPL_g3
#define TK_GVARS_iog32

#include <rgsite.h>
#include <raster.h>
#include <rasio.h>
#include <rgerr.h>
#include <iog3.h>
#include <iog32.h>
#include <iog4.h>
#include <rglist.h>

#undef TK_GVARS_iog32
#undef TK_IMPL_g3
#undef TK_TKI_g3
#undef TK_TKI_list
#undef TK_TKI_rio
#undef TK_TKI_rast

#ifndef USE_LONG_NAMES

#define DetectA1    dtcta1
#define DetectA2    dtcta2
#define DetectB1    dtctb1
#define DetectB2    dtctb2
#define DetectChange	dttcha
#define EncodeTwo	enctwo
#define HorizontalMode	hormod
#define PassMode	pssmod
#define VerticalMode	vermod
#define g3_Group4	g3grp4
#define g3_LineBits	g3lnbt
#define g3_PadSize	g3pdsz
#define g3_Raster	g3rast

#endif


static RASTER_type  *g3_Raster;	    /* Raster being used	*/
static INT_type	    g3_PadSize;	    /* Optional padding width	*/
static INT_type	    g3_LineBits;    /* Number bits written	*/
static BOOL_type    g3_Group4;	    /* Actually a Gr 4?		*/


/*\
 *  G3_WRITEBITMAP2:  Write a Group 3 raster to output using
 *		      two-dimensional coding.
\*/

INT_type G3_WriteBitmap2(raster, format)
    RASTER_type		*raster;
    FORMAT_GROUP3_type	*format;
{
    INT_type	    rval;
    INT_type	    K;
    INT_type	    Line;
    INT_type	    i;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G3_WriteBitmap2");
    }
#endif

/*
 * Strategy:  Use Figure 7/T.4 of the CCITT Recommendation T.4 as
 *	      a guideline.
 */

    K		= format->k;
    g3_PadSize	= format->pad_width;
    g3_Raster	= raster;
    g3_Group4   = BOOL_false;

    Line	= (INT_type) 0;

    G3_PutEOL();
    while (Line < g3_Raster->height) {

        if ((Line % K) == (INT_type) 0) {
	    G3_PutBit(1);
	    if (G3_EncodeOne(g3_Raster, Line, -g3_PadSize)
						 == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
	else {
	    G3_PutBit(0);
	    if (EncodeTwo(Line) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}

	Line++;

    }
    G3_PutBit(1);
    for (i = 0; i < 6; i++) {	    /* Send Return To Control RTC */
	G3_PutEOL();
	G3_PutBit(1);
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G3_WriteBitmap2");
    }
#endif

    return(rval);
}


/*\
 *  ENCODETWO:  Encode a line of a Group 3 raster in a
 *		two-dimensional format.
\*/

static INT_type EncodeTwo(line)
    INT_type	    line;
{
    INT_type	    rval;
    INT_type	    CodeLine;	/* Current line being coded */
    INT_type	    RefLine;	/* Previous line, reference line */

    INT_type	    a0;		/* Coding Line Changing Elements */
    INT_type	    a1;
    INT_type	    b1;		/* Reference Line Changing Elmnts */
    INT_type	    b2;
    INT_type	    a0color;
    INT_type	    abs;
    INT_type	    i;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("EncodeTwo");
    }
#endif

    CodeLine	= line;
    RefLine	= line - (INT_type) 1;

    if (g3_Group4 == BOOL_false) {
	g3_LineBits = (INT_type) 0;
    }

    a0 = (INT_type) -1;	/* Put a0 before first picture element */
    a1 = (INT_type) 0;

    do {
        if ((a1 = DetectA1(CodeLine, a0))	   == RG_ERROR_INT ||
	    (b1 = DetectB1(CodeLine, RefLine, a0)) == RG_ERROR_INT ||
	    (b2 = DetectB2(RefLine, b1))	   == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	if (a0 < (INT_type) 0) {
	    a0color = WHITE;
	}
	else {
	    a0color = GetBit(g3_Raster, a0, CodeLine);
	}
	abs = a1 - b1;
	if (abs < (INT_type) 0) {
	    abs = -abs;
	}

	if (b2 < a1) {
	    if ((a0 = PassMode(b2)) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
	else if (abs <= 3) {
	    if ((a0 = VerticalMode(a1,b1)) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
	else {
	    if ((a0 = HorizontalMode(a0, a1, CodeLine, a0color)) ==
						RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
    } while (a0 < g3_Raster->width);

    /* Add FILL, if specified. (A string of 0s acts as pause) */

    if (g3_Group4 == BOOL_false) {
        if (g3_LineBits < g3_PadSize - 13) {
	    for (i = g3_LineBits; i < g3_PadSize - 13; i++) {
		if (G3_PutBit(0) == RG_ERROR_INT) {
		    rval = RG_ERROR_INT;
		    goto LEAVE;
		}
	    } 	
	}
	G3_PutEOL();
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("EncodeTwo");
    }
#endif

    return(rval);
}


/*\
 *  DETECTCHANGE:  Determine the next changing picture element
 *		   after X.
\*/

static INT_type DetectChange(Line, X)
    INT_type	Line;
    INT_type	X;
{
    INT_type	rval;
    INT_type	color;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("DetectChange");
    }
#endif

    if (Line == -1) {
	rval = g3_Raster->width;
	goto LEAVE;
    }

    rval = X;
    if (X < 0) {
	color = WHITE;
    }
    else {
	color = GetBit(g3_Raster, X, Line);
    }

    do {
	rval++;
    } while (rval < g3_Raster->width &&
	     GetBit(g3_Raster, rval, Line) == color);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("DetectChange");
    }
#endif

    return(rval);
}


/*\
 *  DETECTA1:  Determine the location of changing element a1.
 *	       The changing element to the right of a0 on the
 *	       coding line.
\*/

static INT_type DetectA1(CodeLine, a0)
    INT_type	CodeLine;
    INT_type	a0;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("DetectA1");
    }
#endif

    rval = DetectChange(CodeLine,a0);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("DetectA1");
    }
#endif

    return(rval);
}


/*\
 *  DETECTA2:  Determine location of changing element a2.
 *	       The changing element to the right of a1 on the
 *	       coding line.
\*/

static INT_type DetectA2(CodeLine, a1)
    INT_type	CodeLine;
    INT_type	a1;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("DetectA2");
    }
#endif

    rval = DetectChange(CodeLine,a1);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("DetectA2");
    }
#endif

    return(rval);
}


/*\
 *  DETECTB1:  Determine location of changing element b1.
 *	       The first changing element on the reference line to
 *	       the right of a0 and of opposite color to a0.
\*/

static INT_type DetectB1(CodeLine, RefLine, a0)
    INT_type	CodeLine;
    INT_type	RefLine;
    INT_type	a0;
{
    INT_type	rval;
    VALUE_type	color;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("DetectB1");
    }
#endif

    if (g3_Group4 == BOOL_false && RefLine == -1) {
	rval = RG_ERROR_INT;
	RGError = RG_INTERNAL_err;
	goto LEAVE;
    }

    if (a0 < 0) {
	color = WHITE;
    }
    else {
        color = GetBit(g3_Raster, a0, CodeLine);
    }

    rval = DetectChange(RefLine, a0);

    if (rval < g3_Raster->width) {
        if (GetBit(g3_Raster, rval, RefLine) == color) {
	    rval = DetectChange(RefLine, rval);
	}
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("DetectB1");
    }
#endif

    return(rval);
}


/*\
 *  DETECTB2:  Determine location of changing element b2.
 *	       The changing element to the right of b1 on the
 *	       reference line.
\*/

static INT_type DetectB2(RefLine, b1)
    INT_type	RefLine;
    INT_type	b1;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("DetectB2");
    }
#endif

    rval = DetectChange(RefLine,b1);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("DetectB2");
    }
#endif

    return(rval);
}


/*\
 *  PASSMODE:  Use the Pass coding mode.
\*/

static INT_type PassMode(b2)
    INT_type	b2;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PassMode");
    }
#endif

    if (G3_PutBit(0) == RG_ERROR_INT ||
	G3_PutBit(0) == RG_ERROR_INT ||
	G3_PutBit(0) == RG_ERROR_INT ||
	G3_PutBit(1) == RG_ERROR_INT) {
	    
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    g3_LineBits += 4;

    rval = b2;
    
LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PassMode");
    }
#endif

    return(rval);
}


/*\
 *  VERTICALMODE:  Use the Vertical coding mode.
\*/

static INT_type VerticalMode(a1,b1)
    INT_type	a1;
    INT_type	b1;
{
    INT_type	rval;
    INT_type	i;
    INT_type	V;
    INT_type	pattern;
    static CodeWord_t code[7] = {
	{ 7, 0x20 },
	{ 6, 0x10 },
	{ 3, 0x02 },
	{ 1, 0x01 },
	{ 3, 0x06 },
	{ 6, 0x30 },
	{ 7, 0x60 },
    };

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("VerticalMode");
    }
#endif

    if ((V = a1 - b1 + 3) < 0) {
	rval = RG_ERROR_INT;
	RGError = RG_INTERNAL_err;
	goto LEAVE;
    }

    pattern = code[V].pattern;
    for (i = 0; i < code[V].length; i++) {
	if (G3_PutBit((BYTE_type) (pattern & 01)) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	pattern = (pattern >> 1);
    }
    g3_LineBits += code[V].length;

    rval = a1;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("VerticalMode");
    }
#endif

    return(rval);
}


/*\
 *  HORIZONTALMODE:  Use Horizontal mode coding.
\*/

static INT_type HorizontalMode(a0, a1, CodeLine, color)
    INT_type	a0;
    INT_type	a1;
    INT_type	CodeLine;
    INT_type	color;
{
    INT_type	rval;
    INT_type	UseA0;
    INT_type	a2;
    INT_type	Bits1;
    INT_type	Bits2;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("HorizontalMode");
    }
#endif

    if (a0 == -1) {
	UseA0 = 0;
    }
    else {
	UseA0 = a0;
    }

    a2 = DetectA2(CodeLine, a1);

    if (G3_PutBit(0) == RG_ERROR_INT ||
	G3_PutBit(0) == RG_ERROR_INT ||
	G3_PutBit(1) == RG_ERROR_INT) {
	    
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    if ((Bits1 = G3_PutRunLength(color, a1 - UseA0)) == RG_ERROR_INT ||
	(Bits2 = G3_PutRunLength(1-color, a2 - a1))  == RG_ERROR_INT) {

	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    g3_LineBits += Bits1 + Bits2 + 3;

    rval = a2;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("HorizontalMode");
    }
#endif

    return(rval);
}


/*************************  GROUP 4  **************************/

/*\
 *  G4_WRITEBITMAP:  Write a Group 4 raster to output.
\*/

INT_type G4_WriteBitmap(raster, format)
    RASTER_type		*raster;
    FORMAT_GROUP4_type	*format;
{
    INT_type	    rval;
    INT_type	    Line;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("G4_WriteBitmap");
    }
#endif

/*
 * Strategy:  Use Figure 5/T.6 of the CCITT Recommendation T.4 as
 *	      a guideline.
 */

    g3_Raster	= raster;
    g3_Group4	= BOOL_true;
    Line	= (INT_type) 0;
    g3_LineBits = (INT_type) 0;

    while (Line < g3_Raster->height) {
	if (EncodeTwo(Line) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	Line++;
    }
    G3_PutEOL();
    G3_PutEOL();

    while (g3_LineBits < format->pad_size) {
	G3_PutBit(0);
    }

    g3_Group4   = BOOL_false;
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("G4_WriteBitmap");
    }
#endif

    return(rval);
}





/*  End of file .c  */
