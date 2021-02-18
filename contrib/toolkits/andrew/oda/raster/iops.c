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
 *  File:   iops.c
 *  Author: Andrew Werth
 *
 *  Write a raster out in PostScript form.
 *
 */

/*\
 *  Some of this code has been taken from the Poskanzer Portable 
 *  Bitmap library (PBM), as noted below.
 *    It has been modified in several ways:  Input/Output
 *  has been changed to match Raster Graphics Tool Kit
 *  specifications; debugging output has been removed; error
 *  handling has been modified for RGTK specs; procedures
 *  have been made static where appropriate; and minor changes
 *  to more closely resemble RGTK coding conventions were made.
 *
 *  Modifications by Andrew Werth.
 *
\*/
/*  Some of the following code falls under this copyright notice */


/* pbmtops.c - read a portable bitmap and produce a PostScript bitmap file
**
** Copyright (C) 1988 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/


#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_ps
#define TK_IMPL_ps
#define TK_GVARS_iops

#include <rgsite.h>
#include <rgerr.h>
#include <raster.h>
#include <rasio.h>
#include <iops.h>

#undef TK_TKI_rast
#undef TK_TKI_rio
#undef TK_TKI_ps
#undef TK_IMPL_ps
#undef TK_GVARS_iops

#ifndef USE_LONG_NAMES

#define putitem		putitm
#define putrlbuffer	ptrlbu
#define putrlitem	tprlit
#define ps_PutByte	psptby

#define HexDigit	hexdig
#define bitshift	bitshi
#define bitsperitem	btprit
#define itembuf		itmbuf
#define ps_Parameter	pspara
#define repeatcount	repcnt
#define repeatitem	repitm
#define rlitemsperline	rltmln

#endif


static INT_type	    (*ps_PutByte)();
static POINTER_type ps_Parameter;

/*\
 *  PS_WRITERASTER:  Writes the raster in PostScript.
\*/

INT_type PS_WriteRaster(PutByte, Parameter, format, raster)
    IN	INT_type	     (*PutByte)();
    IN	POINTER_type	      Parameter;
    IN  FORMAT_POSTSCRIPT_type *format;
    IN	RASTER_type	     *raster;
{
    INT_type	rval;
    INT_type	row;
    INT_type	col;
    INT_type	PadRight;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    ps_PutByte = PutByte;
    ps_Parameter = Parameter;
    PadRight = ((raster->width + 7) / 8) * 8 - raster->width;

    if (PutInit(raster->width, raster->height, 
		format->scale) == RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    for (row = 0; row < raster->height; row++) {
	for (col = 0; col < raster->width; col++) {
	    if (PutBit(GetBit(raster, col, row)) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
	for (col = 0; col < PadRight; col++) {
	    if (PutBit(0) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	}
    }

    rval = PutRest();

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}

static int item, bitsperitem, bitshift, rlitemsperline;
static int repeat, itembuf[128], count, repeatitem, repeatcount;

static INT_type PutInit(cols, rows, scale)
INT_type cols, rows;
FLOAT_type scale;
    {
    INT_type	rval;
    int scols, srows;
    int movec, mover;

    scols = (int) ((FLOAT_type) cols * scale * 0.96 + 0.5);
				/*   0.96 is the multiple of   */
    srows = (int) ((FLOAT_type) rows * scale * 0.96 + 0.5);
				/* 72/300 that is closest to 1 */

    movec = 300 - (scols / 2);
    mover = 400 - (srows / 2);
    if (movec < 0 || mover < 0) {
	rval = RG_ERROR_INT;
	RGError = RG_BADPARM_err;
	goto LEAVE;	
    }
    Writef( "%! RGTK.ps\n");
    Writef( "\n" );
    Writef( "/rlestr1 1 string def\n" );
    Writef( "/rlestr 128 string def\n" );
    Writef( "/readrlestring {\n" );
    Writef( "  currentfile rlestr1 readhexstring pop  0 get\n" );
    Writef( "  dup 127 le {\n" );
    Writef( "    currentfile rlestr 0  4 3 roll  1 add  getinterval\n" );
    Writef( "    readhexstring  pop\n" );
    Writef( "  } {\n" );
    Writef( "    256 exch sub  dup\n" );
    Writef( "    currentfile rlestr1 readhexstring pop  0 get\n" );
    Writef( "    exch 0 exch 1 exch 1 sub { rlestr exch 2 index put } for\n" );
    Writef( "    pop  rlestr exch 0 exch getinterval\n" );
    Writef( "  } ifelse\n" );
    Writef( "} bind def\n" );
    Writef( "\n" );
    Writen(300 - (scols / 2));
    Writen(400 - (srows / 2));
    Writef("translate\t% move to lower left corner of box\n");
    Writen(scols);
    Writen(srows);
    Writef( "scale\t\t% scale box\n");
    Writef( "\n" );
    Writen(cols);
    Writen(rows);
    Writef( "1\t\t\t% width height bits/sample\n");
    Writef("[ ");
    Writen(cols);
    Writef("0 0 -");
    Writen(rows);
    Writef("0 ");
    Writen(rows);
    Writef("]\t% transformation matrix\n");
    Writef( "{ readrlestring }\t% proc\n" );
    Writef( "image\n" );

    rlitemsperline = 0;
    item = 0;
    bitsperitem = 0;
    bitshift = 7;

    repeat = 1;
    count = 0;
    rval = RG_SUCCESS_INT;
LEAVE:

    return(rval);
    }

static int putrlitem( rlitem )
int rlitem;
    {
    if ( rlitemsperline == 30 )
	{
	(*ps_PutByte)(ps_Parameter, '\n' );
	rlitemsperline = 0;
	}
    rlitemsperline++;
    Writex( rlitem );
    }

static int putrlbuffer( )
    {
    int i;

    if ( repeat )
	{
	putrlitem( 256 - count );
	putrlitem( repeatitem );
	}
    else
	{
	putrlitem( count - 1 );
	for ( i = 0; i < count; i++ )
	    putrlitem( itembuf[i] );
	}
    repeat = 1;
    count = 0;
    }

static putitem( )
    {
    int i;

    if ( count == 128 )
	putrlbuffer( );

    if ( repeat && count == 0 )
	{ /* Still initializing a repeat buf. */
	itembuf[count] = repeatitem = item;
	count++;
	}
    else if ( repeat )
	{ /* Repeating - watch for end of run. */
	if ( item == repeatitem )
	    { /* Run continues. */
	    itembuf[count] = item;
	    count++;
	    }
	else
	    { /* Run ended - is it long enough to dump? */
	    if ( count > 2 )
		{ /* Yes, dump a repeat-mode buffer and start a new one. */
		putrlbuffer( );
		itembuf[count] = repeatitem = item;
		count++;
		}
	    else
		{ /* Not long enough - convert to non-repeat mode. */
		repeat = 0;
		itembuf[count] = repeatitem = item;
		count++;
		repeatcount = 1;
		}
	    }
	}
    else
	{ /* Not repeating - watch for a run worth repeating. */
	if ( item == repeatitem )
	    { /* Possible run continues. */
	    repeatcount++;
	    if ( repeatcount > 3 )
		{ /* Long enough - dump non-repeat part and start repeat. */
		count = count - ( repeatcount - 1 );
		putrlbuffer( );
		count = repeatcount;
		for ( i = 0; i < count; i++ )
		    itembuf[i] = item;
		}
	    else
		{ /* Not long enough yet - continue as non-repeat buf. */
		itembuf[count] = item;
		count++;
		}
	    }
	else
	    { /* Broken run. */
	    itembuf[count] = repeatitem = item;
	    count++;
	    repeatcount = 1;
	    }
	}

    item = 0;
    bitsperitem = 0;
    bitshift = 7;
    }

static PutBit( b )
BYTE_type b;
    {
    if ( bitsperitem == 8 )
	{
	putitem( );
	}
    if ( ! b )
	item += 1 << bitshift;
    bitsperitem++;
    bitshift--;
    }

static PutRest( )
    {
    if ( bitsperitem > 0 )
	putitem( );
    if ( count > 0 )
	putrlbuffer( );
    Writef( "\nshowpage\n" );
    }


static INT_type Writef(string)
    CHAR_type	*string;
{
    INT_type	rval;
    CHAR_type	*ch;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    ch = string;
    while (*ch != '\0') {
	if ((*ps_PutByte)(ps_Parameter, *ch) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
	ch++;
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
 *  WRITEN:  Write a number to the output device specified in 
 *	     ps_PutByte/ps_Parameter.  This routine assumes
 *	     a character set where numbers start at '0'.
\*/

static INT_type Writen(number)
    INT_type	number;
{
    INT_type	rval;
    INT_type	num;
    CHAR_type	buf[MAX_DIGITS];
    INT_type	bufptr;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if (number > MAX_NUMBER) {
	rval = RG_ERROR_INT;
	RGError = RG_INTERNAL_err;
	goto LEAVE;
    }

    if (number == 0) {
	rval = Writef("0 ");
	goto LEAVE;
    }

    bufptr = 0;
    num = number;
    while (num > 0) {
	buf[bufptr] = (num % 10) + '0';
	num = num / 10;
	bufptr++;
    }

    while (bufptr > 0) {
	bufptr--;
	if ((*ps_PutByte)(ps_Parameter, buf[bufptr]) == RG_ERROR_INT) {
	    rval = RG_ERROR_INT;
	    goto LEAVE;
	}
    }
    rval = (*ps_PutByte)(ps_Parameter, ' ');

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}


static INT_type Writex(number)
    INT_type	number;
{
    INT_type	rval;
    INT_type	lo4;
    INT_type	hi4;
    static BYTE_type HexDigit[] = {
	'0','1','2','3','4','5','6','7','8','9','a',
	'b','c','d','e','f',
    };

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    lo4 = number & 0x0f;
    hi4 = number >> 4;

    (*ps_PutByte)(ps_Parameter,HexDigit[hi4]);
    (*ps_PutByte)(ps_Parameter,HexDigit[lo4]);

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
