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
 *  File:   ioandy.c
 *  Author: Andrew Werth
 *
 *  Input/output routines for rasters in the Andrew .raster format.
 *
 */


#define TK_TKI_mem
#define TK_TKI_pel
#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_etc
#define TK_TKI_andy
#define TK_IMPL_andy
#define TK_GVARS_ioandy

#include <rgsite.h>
#include <rgerr.h>
#include <rgmem.h>
#include <pel.h>
#include <raster.h>
#include <rasio.h>
#include <ioandy.h>
#include <ioetc.h>

#undef TK_GVARS_ioandy
#undef TK_IMPL_andy
#undef TK_TKI_andy
#undef TK_TKI_etc
#undef TK_TKI_rio
#undef TK_TKI_rast
#undef TK_TKI_pel
#undef TK_TKI_mem

#ifndef USE_LONG_NAMES

#define GetBits	    getbit
#define GetPels	    getpel
#define GetString   getstr
#define PutBits	    putbit
#define PutClose    putcls
#define PutPels	    putpel
#define PutString   putstr

#define JustWrapped	jstwrp
#define andy_GetByte	angtby
#define andy_Parameter	anpara
#define andy_PutByte	anptby
#define andy_Width	anwdth

#endif

static VALUE_type GetPels();

#define HEXDIGIT(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

#define HEXVAL(c)   (((c) <= '9') ? ((c) - '0') : (((c) <= 'F') ? (c) - 'A' + 10 : (c) - 'a' + 10))

#define PUNCTUATION(c) ((c) >= 0x21 && (c) <= 0x2f)

#define PUNCVAL(c)  ((c) - 0x1f)

#define GTOZ(c)	    ((c) >= 'G' && (c) <= 'Z')
#define GTOZVAL(c)  ((c) - 0x46)

#define SGTOZ(c)    ((c) >= 'g' && (c) <= 'z')
#define SGTOZVAL(c) ((c) - 0x66)

#define WHITERUN(r) (((r) >= 20) ? 0x7a : (r) + 0x66)

#define BLACKRUN(r) (((r) >= 20) ? 0x5a : (r) + 0x46)

#define BYTERUN(r)  (((r) >= 16) ? 0x2f : (r) + 0x1f)



#define HEADER_TEXT "begindata{raster,"
#define TRAILER_TEXT "\n\\enddata{raster,"

extern BYTE_type    PEL_BitOffL[];

static INT_type	    (*andy_GetByte)();
static INT_type	    (*andy_PutByte)();
static POINTER_type andy_Parameter;
static INT_type	    andy_Width;

RASTER_type *ANDY_ReadRaster(GetByte, parameter, format)
    IN	INT_type	    (*GetByte)();
    IN	POINTER_type	    parameter;
    IN	FORMAT_ANDREW_type  *format;
{
    RASTER_type	    *rval;
    INT_type	    width;	/* Width of raster	*/
    INT_type	    height;	/* Height of raster	*/
    VALUE_type	    NextStatus; /* Whether last row or not */

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ANDY_ReadRaster");
    }
#endif

    andy_GetByte = GetByte;
    andy_Parameter = parameter;

    /* Get preliminary information */

    if ((format->object_id = GetBits(&width,&height)) == RG_ERROR_INT){
	rval = RG_ERROR_RASTER;
	goto LEAVE;
    }

    andy_Width = width;

    if ((rval = MakeRaster(width, height, PEL_BITS))
					 == RG_ERROR_RASTER){
	goto LEAVE;
    }

    GetPels(rval, BOOL_true);
    while ((NextStatus = GetPels(rval,BOOL_false)) != RASTER_DONE) {
	if (NextStatus == RG_ERROR_VALUE) {
	    rval = RG_ERROR_RASTER;
	    goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ANDY_ReadRaster");
    }
#endif

    return(rval);
}


/*\
 *  GETPELS:  Get a row of pels (just return the pels portion).
\*/

static VALUE_type GetPels(raster,reset)
    IN OUT  RASTER_type	*raster;
    IN	    BOOL_type	reset;
{
    VALUE_type		rval;
    REGISTER INT_type	ch2;
    REGISTER INT_type	count;
    BYTE_type		*byte;
    INT_type		LastX;
    INT_type		Hexval;
    static INT_type	x = 0;
    static INT_type	ch = ' ';
    static INT_type	line = 0;
    static BOOL_type	JustWrapped = BOOL_false;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("GetPels");
    }
#endif

    if (reset == BOOL_true) {	    /* Reset the static pointers */
	x = 0;
	ch = ' ';
	line = 0;
	JustWrapped = BOOL_false;
	rval = RASTER_MORE;
	goto LEAVE;
    }

    if (ch == (INT_type) '\\') {    /* First character is '\' */
	rval = RASTER_DONE;
	goto LEAVE;
    }

    if (line < raster->height && 
	RAST_SwapIn(raster,line) == RG_ERROR_VALUE) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    byte = raster->rows[line].swap_body.pels;

    /* Get a character, and parse it along with any necessary
     * following characers.
     */

    do {
	LastX = x;
	ch = (*andy_GetByte)(andy_Parameter);

	/* After a hex character, get the second and use that
	 * hex value as the byte
	 */
	if (HEXDIGIT(ch)) {
	    ch2 = (*andy_GetByte)(andy_Parameter);
	    if (HEXDIGIT(ch2)) {
		byte[x++] = HEXVAL(ch) * 16 + HEXVAL(ch2);
	    }
	    else {
		rval = RG_ERROR_INT;
		RGError = RG_FORMBAD_err;
		goto LEAVE;
	    }
	}
	/* After "punctuation" comes a hex number which says
	 * which byte gets repeated PUNCVAL times.
	 */
	else if (PUNCTUATION(ch)) {
	    count = PUNCVAL(ch);
	    ch = (*andy_GetByte)(andy_Parameter);
	    if (HEXDIGIT(ch)) {
		ch2 = (*andy_GetByte)(andy_Parameter);
		if (HEXDIGIT(ch2)) {
		    Hexval = 16 * HEXVAL(ch) + HEXVAL(ch2);
		    while (count > 0) {
			byte[x++] = Hexval;
			count--;
		    }
		}
		else {
		    rval = RG_ERROR_INT;
		    RGError = RG_FORMBAD_err;
		    goto LEAVE;
		}
	    }
	    else {
		rval = RG_ERROR_INT;
		RGError = RG_FORMBAD_err;
		goto LEAVE;
	    }
	}
	/* If ch is from G-Z, then a black run */
	else if (GTOZ(ch)) {
	    for (count = GTOZVAL(ch); count > 0; count--) {
		byte[x++] = (BYTE_type) 0xff;
	    }
	}
	/* If ch is from g-z, then a white run */
	else if (SGTOZ(ch)) {
	    for (count = SGTOZVAL(ch); count > 0; count--) {
		byte[x++] = (BYTE_type) 0x00;
	    }
	}
	/* Another backslash meands the end of the raster */
	else if (ch == (INT_type) '\\') {
	    rval = RASTER_MORE;
	    goto LEAVE;
	}

	/* Vertical bar means the end of a scan line */
	if (x >= raster->width && ch != (INT_type) '|') {
	    x = 0;
	    line++;
	    if (line < raster->height && 
		RAST_SwapIn(raster,line) == RG_ERROR_VALUE) {
		rval = RG_ERROR_VALUE;
		goto LEAVE;
	    }
	    byte = raster->rows[line].swap_body.pels;
	    JustWrapped = BOOL_true;
	}
	else if (x > LastX) {
	    JustWrapped = BOOL_false;
	}
    } while (ch != (INT_type) '|');
    if (JustWrapped == BOOL_false) {
	x = 0;
	line++;
    }
    
    rval = RASTER_MORE;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("GetPels");
    }
#endif

    return(rval);
}


/*\
 *  GETBITS:  Get the .raster envelop, the header information,
 *	      and the "bits" line.
\*/

static INT_type GetBits(width, height)
    OUT	INT_type    *width;
    OUT	INT_type    *height;
{
    INT_type	rval;
    REGISTER INT_type	i;
    INT_type	ObjectID;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("GetBits");
    }
#endif

    if ((*andy_GetByte)(andy_Parameter) == (INT_type) '\\') {

	/* Get Envelop data */
	if (GetString(HEADER_TEXT) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}

	ObjectID = ETC_GetNumber(andy_GetByte, andy_Parameter);

	/* Check for closing brace and newline */
	if (GetString("\n") == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}

	/* Check for the eight numbers in the header line */
	for (i = 0; i < 8; i++) {
	    if (ETC_GetNumber(andy_GetByte, andy_Parameter) 
						== RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		RGError = RG_FORMBAD_err;
		goto LEAVE;	
	    }
	}
    }

    if (GetString("bits ") == RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }

    /* Get the object ID, the width, and the height of the raster */

    if ((ETC_GetNumber(andy_GetByte, andy_Parameter) != ObjectID) ||
	((*width  = ETC_GetNumber(andy_GetByte, andy_Parameter))
					     == RG_ERROR_INT)	  ||
	((*height = ETC_GetNumber(andy_GetByte, andy_Parameter))
					     == RG_ERROR_INT)) {
	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }

    rval = ObjectID;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("GetBits");
    }
#endif

    return(rval);
}

/*\
 *  GETSTRING:  Get a string from input.  Returns RG_SUCCESS_INT if
 *		the string is found, otherwise RG_ERROR_INT.
\*/

static INT_type GetString(string)
    IN CHAR_type    *string;
{
    INT_type	rval;
    REGISTER INT_type	byte;
    CHAR_type	*ch;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("GetString");
    }
#endif

    ch = string;

    while (*ch != (CHAR_type) '\0') {
	byte = (*andy_GetByte)(andy_Parameter);
	if (byte == RG_ERROR_INT || byte != *ch) {
	    rval = RG_ERROR_INT;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
	ch++;
    }
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("GetString");
    }
#endif

    return(rval);
}


/*\
 * ANDY_WRITERASTER:  Write a raster to output in Andrew .raster
 *		      format.
\*/

INT_type ANDY_WriteRaster(PutByte, Parameter, format, raster)
    IN	INT_type	    (*PutByte)();
    IN	POINTER_type	    Parameter;
    IN  FORMAT_ANDREW_type  *format;
    IN	RASTER_type	    *raster;
{
    INT_type	    rval;
    INT_type	    y;
    INT_type	    Modulo;
    BYTE_type	    Crop;   /* For turning off last bits in last byte */

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("ANDY_WriteRaster");
    }
#endif

    andy_PutByte = PutByte;
    andy_Parameter = Parameter;
    andy_Width = raster->width / 8 + 1;
    Crop = 0xff;

    /* Turn off the last bits in the last byte if needed */

    if ((Modulo = raster->width % 8) == 0) {
	andy_Width--;
    } else {
	for (y = Modulo; y < 8; y++) {
	    Crop &= PEL_BitOffL[y];
	}
    }

    if (PutBits(raster,format) == RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    /* Put each row of pels individually */

    for (y = 0; y < raster->height; y++) {
	if (RAST_SwapIn(raster,y) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	raster->rows[y].swap_body.pels[andy_Width] &= Crop;
	if (PutPels(raster->rows[y].swap_body.pels) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }
    if (PutClose(format) == RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("ANDY_WriteRaster");
    }
#endif

    return(rval);
}


/*\
 *  PUTPELS:  Put a row of pels to the output.
\*/

static INT_type PutPels(pels)
    IN	BYTE_type   *pels;
{
    INT_type		rval;
    REGISTER INT_type	x;
    REGISTER INT_type	x1;
    INT_type		run;
    BYTE_type		byte;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PutPels");
    }
#endif

    x = 0;

    while (x < andy_Width) {
	byte = pels[x];
	x1 = x;
	while (pels[++x1] == byte && x1 < andy_Width) {
	    /* Keep counting */
	}
	run = x1 - x;
	x += run;
	if (byte == 0x00) {	/* Handle white run */
	    while (run > 0) {
		if ((*andy_PutByte)(andy_Parameter,WHITERUN(run))
						== RG_ERROR_INT) {
		    rval = RG_ERROR_INT;
		    goto LEAVE;
		}
		run -= 20;
	    }
	}
	else if (byte == 0xff) { /* Handle black run */
	    while (run > 0) {
		if ((*andy_PutByte)(andy_Parameter,BLACKRUN(run))
						== RG_ERROR_INT) {
		    rval = RG_ERROR_INT;
		    goto LEAVE;
		}
		run -= 20;
	    }
	}
	else {			/* Handle mixed bytes */
	    if (run == 1) {
		if (PutHex(byte) == RG_ERROR_INT) {
		    rval = RG_ERROR_INT;
		    goto LEAVE;
		}
	    }
	    else {		/* Run of mixed bytes */
		while (run > 0) {
		    if ((*andy_PutByte)(andy_Parameter,BYTERUN(run))
						== RG_ERROR_INT ||
			PutHex(byte) == RG_ERROR_INT) {
			rval = RG_ERROR_INT;
			goto LEAVE;
		    }
		    run -= 16;
		}
	    }
	}
    }

    if (PutString(" |\n") == RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PutPels");
    }
#endif

    return(rval);
}

/*\
 *  PUTHEX:  Output a hexadecimal number (two digits).
\*/

static INT_type PutHex(number)
    IN	INT_type    number;
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

    if ((*andy_PutByte)(andy_Parameter,Hex[(number & 0xf0) >> 4]) ==
						RG_ERROR_INT ||
	(*andy_PutByte)(andy_Parameter,Hex[(number & 0x0f)]) ==
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
 *  PUTBITS:  Output the envelop, the header, and the bits line.
\*/

static INT_type PutBits(raster, format)
    RASTER_type		*raster;
    FORMAT_ANDREW_type	*format;
{
    INT_type		rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PutBits");
    }
#endif

    if ((*andy_PutByte)(andy_Parameter, '\\') == RG_ERROR_INT  ||
	PutString(HEADER_TEXT) == RG_ERROR_INT		    ||
	ETC_PutNumber(andy_PutByte, andy_Parameter,
		format->object_id,0) == RG_ERROR_INT	    ||
	PutString("}\n2 0 65536 65536 0 0 ") == RG_ERROR_INT   ||
	ETC_PutNumber(andy_PutByte, andy_Parameter,
		raster->width,1) == RG_ERROR_INT		    ||
	ETC_PutNumber(andy_PutByte, andy_Parameter,
		raster->height,0) == RG_ERROR_INT		    ||
	PutString("\nbits ") == RG_ERROR_INT		    ||
	ETC_PutNumber(andy_PutByte, andy_Parameter,
		format->object_id,1) == RG_ERROR_INT	    ||
	ETC_PutNumber(andy_PutByte, andy_Parameter,
		raster->width,1) == RG_ERROR_INT		    ||
	ETC_PutNumber(andy_PutByte, andy_Parameter,
		raster->height,1) == RG_ERROR_INT		    ||
	PutString("\n") == RG_ERROR_INT) {

	rval = RG_ERROR_INT;
	goto LEAVE;	    
    }
    
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PutBits");
    }
#endif

    return(rval);
}


/*\
 *  PUTCLOSE:  Output the closing envelop data.
\*/

static INT_type PutClose(format)
    FORMAT_ANDREW_type	*format;
{
    INT_type		rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PutClose");
    }
#endif

    if (PutString(TRAILER_TEXT) == RG_ERROR_INT		    ||
	ETC_PutNumber(andy_PutByte, andy_Parameter,
		format->object_id,0) == RG_ERROR_INT	    ||
	PutString("}\n") == RG_ERROR_INT) {
	    
	rval = RG_ERROR_INT;
	goto LEAVE;	    
    }
    
    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PutClose");
    }
#endif

    return(rval);
}


/*\
 *  PUTSTRING:  Put a string to output.
\*/

static INT_type PutString(string)
    CHAR_type	*string;
{
    INT_type	rval;
    CHAR_type	*ch;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("PutString");
    }
#endif

    ch = string;

    while (*ch != '\0') {
	if ((*andy_PutByte)(andy_Parameter, *ch) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	ch++;
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("PutString");
    }
#endif

    return(rval);
}



/*  End of file .c  */
