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
 *  File:   iox.c
 *  Author: Andrew Werth
 *
 *  Input/output routines for X bitmap.
 *
 */

#define TK_TKI_mem
#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_list
#define TK_TKI_x
#define TK_IMPL_x
#define TK_GVARS_iox

#include <rgsite.h>
#include <rgerr.h>
#include <rgmem.h>
#include <raster.h>
#include <rasio.h>
#include <iox.h>
#include <rglist.h>

#undef TK_GVARS_iox
#undef TK_IMPL_x
#undef TK_TKI_x
#undef TK_TKI_list
#undef TK_TKI_rio
#undef TK_TKI_rast
#undef TK_TKI_mem

#ifndef USE_LONG_NAMES

#define Compare		compar
#define GetLine		getlin
#define PutNumber	putnum
#define PutString	putstr
#define x_GetByte	xgetby
#define x_Parameter	xparam
#define x_PutByte	xputby

#define Reverse		rever

#endif

static BOOL_type Compare();

static INT_type	    (*x_PutByte)();	/* Points to i/o routines */
static INT_type	    (*x_GetByte)();
static POINTER_type x_Parameter;


/* MACRO: determine hex value of char, returns 256 if bad char */

#define HEXVAL(c)   (((c) <= '9' && (c) >= '0') ? ((c) - '0')	  :\
		     (((c) <= 'F' && (c) >= 'A')? ((c) - 'A' + 10):\
		     (((c) <= 'f' && (c) >= 'a')? ((c) - 'a' + 10):\
			256)))

/*\
 *  X_READRASTER:  Read in an X Bitmap.
\*/

RASTER_type *X_ReadRaster(GetByte, Parameter, Format)
    IN	INT_type	(*GetByte)();
    IN	POINTER_type	Parameter;
    IN	FORMAT_XBM_type	*Format;
{
    RASTER_type	    *rval;
    RASTER_type	    *raster;
    BYTE_type	    *pos;
    BOOL_type	    ReadIn;
    CHAR_type	    line[MAX_LINE];
    INT_type	    width;
    INT_type	    height;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("X_ReadRaster");
    }
#endif

    x_GetByte = GetByte;
    x_Parameter = Parameter;

    ReadIn = BOOL_false;

    while (ReadIn == BOOL_false) {
        if (GetLine(line) == RG_ERROR_INT) {
	    rval = RG_ERROR_RASTER;
	    goto LEAVE;
	}
        if (Compare(line,"#define ")	 == BOOL_true) {
	    pos = MoveTo(&line[7],'_') + 1;

	    if (Compare(pos,"width") == BOOL_true) {
	        width = GetNum(pos+6);
	        if (width == RG_ERROR_INT) {
		    rval = RG_ERROR_RASTER;
		    goto LEAVE;
		}
	    }
	    else if (Compare(pos,"height") == BOOL_true) {
	        height = GetNum(pos+7);
	        if (height == RG_ERROR_INT) {
		    rval = RG_ERROR_RASTER;
		    goto LEAVE;
		}
	    }
	    else {
	        rval = RG_ERROR_RASTER;
	        goto LEAVE;
	    }
	}
	else if (Compare(line,"static ") == BOOL_true) {
	    raster = MakeRaster(width, height);
	    raster = ReadRaster(raster);
	    ReadIn = BOOL_true;
	}
    }

    rval = raster;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("X_ReadRaster");
    }
#endif

    return(rval);
}


/*\
 *  READRASTER:  Read in the raster data portion of an X bitmap.
\*/

static RASTER_type *ReadRaster(raster)
    IN	RASTER_type *raster;
{
    RASTER_type	    *rval;
    INT_type	    ch;
    INT_type	    ch1;
    INT_type	    x;
    INT_type	    y;
    INT_type	    Hexv;
    INT_type	    hex;
    INT_type	    hex1;
    INT_type	    BytesWide;

    static INT_type Reverse[] = {   /* Reverse hex values */
	0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15
    };

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ReadRaster");
    }
#endif

    BytesWide = raster->bytes;
    x = 0;
    ch = 0;

    y = 0;

    while (ch != (INT_type) '}') {
	if ((ch = (*x_GetByte)(x_Parameter)) == RG_ERROR_INT) {
	    rval = RG_ERROR_RASTER;
	    goto LEAVE;
	}
	if (ch == (INT_type) '0') {
	    if ((ch  = (*x_GetByte)(x_Parameter)) == RG_ERROR_INT	||
		 ch != (INT_type) 'x'				||
		(ch  = (*x_GetByte)(x_Parameter)) == RG_ERROR_INT	||
		(ch1 = (*x_GetByte)(x_Parameter)) == RG_ERROR_INT	||
		(hex  = HEXVAL(ch))		  >  255        ||
		(hex1 = HEXVAL(ch1))		  >  255 ) {
		rval = RG_ERROR_RASTER;
		goto LEAVE;
	    }
	    if (x == BytesWide) {
		x = 0;
		y++;
		if (y == raster->height) {
		    rval = RG_ERROR_RASTER;
		    RGError = RG_FORMBAD_err;
		    goto LEAVE;
		}
	    }
	    Hexv = Reverse[hex1] * 16 + Reverse[hex];
	    raster->rows[y].swap_body.pels[x++] = Hexv;
	}
    }
    
    rval = raster;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ReadRaster");
    }
#endif

    return(rval);
}

/*\
 *  GETLINE:  Read in a line from the input source into the buffer
 *	      provided.  The line must be less than MAX_LINE - 1
 *	      characters (before the null).
\*/

static INT_type GetLine(line)
    IN	CHAR_type   *line;
{
    INT_type	rval;
    INT_type	x;
    INT_type	ch;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("GetLine");
    }
#endif

    x = 0;
    while (ch != '\n') {
	if ((ch = (*x_GetByte)(x_Parameter)) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	line[x++] = ch;
	if (x == MAX_LINE) {
	    rval = RG_ERROR_INT;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
    }
    line[x] = '\0';

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("GetLine");
    }
#endif

    return(rval);
}




/*\
 *  COMPARE:  See if the second string starts off the first string.
 *	      Does NOT check for matching '\0' character.
\*/

static BOOL_type Compare(str1, str2)
    IN	BYTE_type *str1;
    IN	BYTE_type *str2;
{
    BOOL_type	rval;
    INT_type	i;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("Compare");
    }
#endif

    i = 0;

    while (str1[i] == str2[i]) {
	if (str2[++i] == '\0') {
	    rval = BOOL_true;
	    goto LEAVE;
	}
    }
    rval = BOOL_false;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("Compare");
    }
#endif

    return(rval);
}

/*\
 *  MOVETO:  Move to a certain character within a line.
\*/

static BYTE_type *MoveTo(line,ch)
    IN	BYTE_type	*line;
    IN	CHAR_type	ch;
{
    BYTE_type	*rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("MoveTo");
    }
#endif

    rval = line;
    while (*rval != (BYTE_type) ch) {
	rval++;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("MoveTo");
    }
#endif

    return(rval);
}


/*\
 *  GETNUM:  Get a number out of a string.
\*/

static INT_type GetNum(line)
    IN	BYTE_type	*line;
{
    INT_type	rval;
    BYTE_type	*byte;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("GetNum");
    }
#endif

    byte = line;
    rval = 0;

    while (*byte >= '0' && *byte <= '9') {
	rval = rval * 10 + *byte - '0';
	if (rval > MAX_READ_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	byte++;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("GetNum");
    }
#endif

    return(rval);
}



/*\
 *  X_WRITERASTER:  Write an X bitmap to output.
\*/

INT_type X_WriteRaster(PutByte, parameter, format, raster)
    IN	INT_type	(*PutByte)();
    IN	POINTER_type	parameter;
    IN  FORMAT_XBM_type	*format;
    IN	RASTER_type	*raster;
{
    INT_type		rval;
    INT_type		y;
    INT_type		x;
    INT_type		BytesWide;
    BYTE_type		byte;
    BOOL_type		FirstByte;
    INT_type		Position;

    static BYTE_type Reverse[] = {
	'0','8','4','c','2','a','6','e','1','9','5','d',
	'3','b','7','f'
    };


#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    x_PutByte = PutByte;
    x_Parameter = parameter;

    FirstByte = BOOL_true;
    Position = 0;
    BytesWide = (raster->width + 7) / 8;

    PutString("#define ");
    PutString(format->name);
    PutString("_width ");
    PutNumber(raster->width);
    PutString("\n#define ");
    PutString(format->name);
    PutString("_height ");
    PutNumber(raster->height);
    PutString("\nstatic char ");
    PutString(format->name);
    PutString("_bits[] = {\n");

    for (y = 0; y < raster->height; y++) {
	if (RAST_SwapIn(raster,y) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	for (x = 0; x < BytesWide; x++) {
	    if (FirstByte == BOOL_true) {
		FirstByte = BOOL_false;
	    }
	    else {
		if ((*x_PutByte)(x_Parameter,',') == RG_ERROR_INT) {
		    rval = RG_ERROR_INT;
		    goto LEAVE;
		}
	    }
	    if (Position++ == 15) {
		if (PutString("\n ") == RG_ERROR_INT) {
		    rval = RG_ERROR_INT;
		    goto LEAVE;
		}
		Position = 0;
	    }
	    byte = raster->rows[y].swap_body.pels[x];
	    if (PutString("0x") == RG_ERROR_INT		    ||
		(*x_PutByte)(x_Parameter,Reverse[byte & 15])
			        == RG_ERROR_INT		    ||
		(*x_PutByte)(x_Parameter,Reverse[byte >> 4])
			        == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
    }

    PutString("};");

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
 *  PUTSTRING: Put a string to output.
\*/

static INT_type PutString(string)
    CHAR_type	*string;
{
    INT_type	rval;
    CHAR_type	*ch;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    for (ch = string; *ch != '\0'; ch++) {
	if ((*x_PutByte)(x_Parameter, *ch) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
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
 *  PUTHEX:  Output a hexadecimal number (two digits).
\*/

static INT_type PutHex(number)
    INT_type number;
{
    INT_type	rval;
    static BYTE_type Hex[] = {
	'0','1','2','3','4','5','6','7','8','9',
	'a','b','c','d','e','f'
    };

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PutHex");
    }
#endif

    if ((*x_PutByte)(x_Parameter,Hex[(number & 0xf0) >> 4]) ==
						RG_ERROR_INT ||
	(*x_PutByte)(x_Parameter,Hex[(number & 0x0f)]) ==
						RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	goto LEAVE;   
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PutHex");
    }
#endif

    return(rval);
}


/*\
 *  PUTNUMBER:  Puts a number to output.
\*/

static INT_type PutNumber(number, spaces)
    INT_type	number;
    INT_type	spaces;
{
    INT_type	rval;
    INT_type	length;
    INT_type	i;
    CHAR_type	buffer[MAX_INT_LENGTH];

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PutNumber");
    }
#endif

    length = 0;
    i = number;

    if (i == 0) {
	(*x_PutByte)(x_Parameter,'0');
	goto END;
    }

    while (i > 0) {
	buffer[length++] = (i % 10) + '0';
	i = i / 10;
    }

    while (--length >= 0) {
	if ((*x_PutByte)
	    (x_Parameter,(BYTE_type)buffer[length]) ==RG_ERROR_INT) {

	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }

END:
    if (spaces != 0) {
	if ((*x_PutByte)(x_Parameter, ' ') == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PutNumber");
    }
#endif

    return(rval);
}


/*  End of file .c  */
