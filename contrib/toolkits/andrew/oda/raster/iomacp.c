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
 *  File:   iomacp.c
 *  Author: Andrew Werth
 *
 *  Reads/Writes in a raster stored in MacPaint format.
 *
 */

#define TK_TKI_mem
#define TK_TKI_rast
#define TK_TKI_rio
#define TK_TKI_list
#define TK_TKI_pel
#define TK_TKI_macp
#define TK_IMPL_macp
#define TK_GVARS_macp

#include <rgsite.h>
#include <rgerr.h>
#include <rgmem.h>
#include <raster.h>
#include <rasio.h>
#include <iomacp.h>
#include <pel.h>
#include <rglist.h>

#undef TK_GVARS_macp
#undef TK_IMPL_macp
#undef TK_TKI_macp
#undef TK_TKI_pel
#undef TK_TKI_list
#undef TK_TKI_rio
#undef TK_TKI_rast
#undef TK_TKI_mem

#ifndef USE_LONG_NAMES

#define PutHeader	    pthead
#define ReadMacPaintFile    rdmpfl
#define checkborders	    cekbor
#define fillbits	    fillbit
#define filltemp	    flltmp
#define writemacp	    wrmacp
#define sendbytes	    senbyt
#define macp_Parameter	    mparam
#define macp_PutByte	    mptbyt


#endif

static INT_type	    (*macp_PutByte)();
static POINTER_type macp_Parameter;

#define TRUE  1
#define FALSE 0
#define EQUAL (1 == 1)
#define UNEQUAL (1 == 2)

#define PELVAL(c)   (((c) < Bytes) ? (pels[(c)]) : 0x00)


#define TWOS(c)	(((c) > 127) ? (256 - 127) : (256 - (c)))
#define NORM(c) (((c) > 127) ? 127 : (c))

/*\
 *  MACP_READRASTER:  Read in a MacPaint raster.
\*/

RASTER_type *MACP_ReadRaster(GetByte, Parameter, Format)
    IN	INT_type	      (*GetByte)();
    IN	POINTER_type	      Parameter;
    IN	FORMAT_MACPAINT_type  *Format;
{
    RASTER_type	    *rval;
    INT_type	    Lines;
    INT_type	    y;
    BYTE_type	    header[128];
    INT_type	    byte;
    INT_type	    i;

    static BYTE_type Pic[MAX_LINES][BYTES_WIDE];

#ifdef DEBUG
    if (TraceRoutine(1)) {
	Entering("MACP_ReadRaster");
    }
#endif

    if (Format->macbin != 0) {
	for (i = 0; i < 128; i++) {
	    byte = (*GetByte)(Parameter);
	    if (byte == RG_ERROR_INT) {
		rval = RG_ERROR_RASTER;
		goto LEAVE;
	    }
	    header[i] = (BYTE_type) (byte & 0xff);
	}
	if (header[0] != 0 || header[74] != 0) {
	    /* File is NOT a MacBinary file */
	    rval = RG_ERROR_RASTER;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	}
    }

    if (ReadMacPaintFile(GetByte, Parameter, &Lines, Pic) < 0) {
	rval = RG_ERROR_RASTER;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }
    rval = MakeRaster(576, Lines, PEL_BITS);


/****
    rval->height = Lines;
    rval->width = BYTES_WIDE * 8;
    rval->rows = (ROW_t *) MEM_Malloc
			    ((INT_type) (Lines * sizeof(ROW_t)));
****/

    for (y = 0; y < Lines; y ++) {
	rval->rows[y].swap_body.pels = &(Pic[y][0]);
    }


LEAVE:
#ifdef DEBUG
    if (TraceRoutine(1)) {
	Leaving("MACP_ReadRaster");
    }
#endif

    return(rval);
}


/* macptopbm.c - read a MacPaint file and produce a portable bitmap
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

/*
** Some of the following routine is:
**
**              Copyright 1987 by Patrick J. Naughton
**                       All Rights Reserved
** Permission to use, copy, modify, and distribute this software
** and its documentation for any purpose and without fee is hereby
** granted, provided that the above copyright notice appear in all
** copies and that both that copyright notice and this permission
** notice appear in supporting documentation.
*/

static INT_type
ReadMacPaintFile( GetByte, Parameter, scanLineP, Pic )
INT_type	(*GetByte)();
POINTER_type	Parameter;
INT_type	*scanLineP;
BYTE_type	Pic[MAX_LINES][BYTES_WIDE];
    {
    unsigned int i, j, k;
    unsigned char ch;

    /* Skip over the header. */
    for ( i = 0; i < HEADER_LENGTH; i++ )
	(*GetByte)(Parameter);

    *scanLineP = 0;
    k = 0;

    while ( *scanLineP < MAX_LINES )
	{
	ch = (unsigned char) (*GetByte)(Parameter);	/* Count byte */
	i = (unsigned int) ch;
	if ( ch < 0x80 )
	    {	/* Unpack next (I+1) chars as is */
	    for ( j = 0; j <= i; j++ )
		if ( *scanLineP < MAX_LINES )
		    {
		    Pic[*scanLineP][k++] = (unsigned char) (*GetByte)(Parameter);
		    if ( ! (k %= BYTES_WIDE) )
			*scanLineP += 1;
		    }
	    }
	else
	    {	/* Repeat next char (2's comp I) times */
	    ch = (*GetByte)(Parameter);
	    for ( j = 0; j <= 256 - i; j++ )
		if ( *scanLineP < MAX_LINES )
		    {
		    Pic[*scanLineP][k++] = (unsigned char) ch;
		    if ( ! (k %= BYTES_WIDE) )
			*scanLineP += 1;
		    }
	    }
	}

    return(0);
    }


/*\
 *  MACP_WriteRaster:  Write a MacPaint raster.
\*/

INT_type MACP_WriteRaster(PutByte, Parameter, format, raster)
    IN	INT_type	     (*PutByte)();
    IN	POINTER_type	      Parameter;
    IN  FORMAT_MACPAINT_type *format;
    IN	RASTER_type	     *raster;
{
    INT_type	    rval;
    INT_type	    y;
    INT_type	    x;
    INT_type	    x1;
    INT_type	    run;
    INT_type	    LastX;
    INT_type	    DataBytes;
    INT_type	    i;
    INT_type	    Bytes;
    BYTE_type	    *pels;
    BYTE_type	    byte;
    BYTE_type	    *blank;

#ifdef DEBUG
    if (TraceRoutine(1)) {
	Entering("MACP_WriteRaster");
    }
#endif

    macp_PutByte = PutByte;
    macp_Parameter = Parameter;
    Bytes = raster->bytes;

    if (PutHeader() == RG_ERROR_INT) {
	rval = RG_ERROR_INT;
	goto LEAVE;
    }

    blank = (BYTE_type *) MEM_Malloc(73);
    for (i = 0; i < 72; i++) {
	blank[i] = 0x00;
    }

    for (y = 0; y < 720; y++) {
        x = 0;
	if (y < raster->height) {
	    if (RAST_SwapIn(raster,y) == RG_ERROR_INT) {
		rval = RG_ERROR_INT;
		goto LEAVE;
	    }
	    pels = raster->rows[y].swap_body.pels;
	}
	else {
	    pels = blank;
	}

        while (x < 72) {
	    byte = PELVAL(x);
	    x1 = x;
	    while (x < 72 && byte == PELVAL(x)) {
		x++;
	    }
	    run = x - x1;
	    if (run >= 3) {
		while (run > 0) {
		    if ((*macp_PutByte)(macp_Parameter,257 - run) ==
						    RG_ERROR_INT ||
			(*macp_PutByte)(macp_Parameter,byte) ==
						    RG_ERROR_INT ) {
			rval = RG_ERROR_INT;
			goto LEAVE;
		    }
		    run -= 127;
		}
	    }
	    else {
		x = x1;
		LastX = x++;
		DataBytes = 1;
		while (x < 72 && DataBytes == 1) {
		    byte = PELVAL(x);
		    x++;
		    while (x < 72 && byte == PELVAL(x)) {
			DataBytes++;
			x++;
			if (DataBytes == 3) {
			    x -= DataBytes;
			    break;
			}
		    }
		    if (DataBytes < 3) {
			DataBytes = 1;
		    }
		}
		if ((*macp_PutByte)
		    (macp_Parameter,x - LastX - 1)==RG_ERROR_INT) {
		    rval = RG_ERROR_INT;
		    goto LEAVE;
		}
		for (i = LastX ; i < x; i++) {
		    if ((*macp_PutByte)
			(macp_Parameter,PELVAL(i)) ==	RG_ERROR_INT) {
			rval = RG_ERROR_INT;
			goto LEAVE;
		    }
		}
	    }	
	}
    }
    MEM_Free(blank);

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(1)) {
	Leaving("MACP_WriteRaster");
    }
#endif

    return(rval);
}

/*\
 *  PUTHEADER:  Output a 512 byte header.  Since we want the header
 *		to be ignored, use version number 0 --> all zeroes.
\*/

static INT_type PutHeader()
{
    INT_type		rval;
    REGISTER INT_type	i;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    for (i = 0; i < 512; i++) {
	if ((*macp_PutByte)(macp_Parameter,'\0') == RG_ERROR_INT) {
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




static checkborders( top, left, bottom, right )
int top, left, bottom, right;
{
    int rval;

#ifdef DEBUG
    if (TraceRoutine(1)) {
	Entering("checkborders");
    }
#endif


  if( right <= left || left < 0 || right - left + 1 > MAX_COLS )
  {
    rval = FALSE;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "%s: error in right (= %d) and/or left (=%d)\n" */
  }
  if( bottom <= top || top < 0 || bottom - top + 1 > MAX_LINES )
  {
    rval = FALSE;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "%s: error in bottom (= %d) and/or top (=%d)\n" */
  }
  rval = TRUE;
LEAVE:
#ifdef DEBUG
    if (TraceRoutine(1)) {
	Leaving("checkborders");
    }
#endif

    return(rval);
} /* checkborders */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* centreer het over te zenden plaatje in het MacPaint document
 *
 * Het plaatje wordt vanaf al of niet opgegeven (left, bottom)
 * in een pbm bitmap van de juist macpaint afmetingen gezet,
 * en eventueel afgekapt.
 */
static fillbits( bits, bitsr, top, left, bottom, right )
BYTE_type **bits, **bitsr;
int top, left, bottom, right;
{


  BYTE_type *bi, *bir;
  register int i, j;
  int bottomr, leftr, topr, rightr;
  int width, height;

#ifdef DEBUG
    if (TraceRoutine(1)) {
	Entering("fillbits");
    }
#endif

  width = right - left + 1;
  leftr = (MAX_COLS - width) / 2;
  rightr = leftr + width - 1;

  height = bottom - top + 1;
  topr = ( MAX_LINES - height ) / 2;
  bottomr = topr + height - 1;

  for( i = 0; i < topr; i++ )
  { bi = bits[i];
    for( j = 0; j < MAX_COLS; j++ )
      *bi++ = 0;
  }

  for( i = topr; i <= bottomr; i++ )
  { bi = bits[i];
    { for( j = 0; j < leftr; j++ )
	*bi++ = 0;
      bir = bitsr[ i - topr + top ];
      for( j = leftr; j <= rightr; j++ )
	*bi++ = bir[j - leftr + left];
      for( j = rightr + 1; j < MAX_COLS; j++ )
	*bi++ = 0;
  } }

  for( i = bottomr + 1; i < MAX_LINES; i++ )
  { bi = bits[i];
    for( j = 0; j < MAX_COLS; j++ )
      *bi++ = 0;
  }
LEAVE:
#ifdef DEBUG
    if (TraceRoutine(1)) {
	Leaving("fillbits");
    }
#endif

    return;

} /* fillbits */
      
/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

static INT_type writemacp(bits)
    BYTE_type			**bits;
{
  register int i;
  BYTE_type pb[MAX_COLS * 2];
  int npb;

#ifdef DEBUG
    if (TraceRoutine(1)) {
	Entering("writemacp");
    }
#endif

  header();
  for( i=0; i < MAX_LINES; i++ )
  { npb = packit( pb, bits[i] );
    sendbytes( pb, npb );
  }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(1)) {
	Leaving("writemacp");
    }
#endif

  return(RG_SUCCESS_INT);

} /* writemacp */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* pack regel van MacPaint doc in Apple's format
 * return value = # of bytes in pb 
 */
static int
packit( pb, bits )
BYTE_type *pb, *bits;
{



  register int charcount, npb;
  int newcount, flg;
  BYTE_type temp[72];
  BYTE_type*count, *srcb, *destb, save;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("packit");
    }
#endif

  srcb = bits; destb = temp;
  filltemp( destb, srcb );
  srcb = temp;
  destb = pb;
  npb = 0;
  charcount = BYTES_WIDE;
  flg = EQUAL;
  while( charcount )
  { save = *srcb++;
    charcount--;
    newcount = 1;
    while( (*srcb == save) && charcount )
    { srcb++;
      newcount++;
      charcount--;
    }
    if( newcount > 2 )
    { count = destb++;
      *count = 257 - newcount;
      *destb++ = save;
      npb += 2;
      flg = EQUAL;
    }
    else
    { if( flg == EQUAL )
      { count = destb++;
	*count = newcount - 1;
	npb++;
      }
      else
	*count += newcount;
      while( newcount-- )
      { *destb++ = save;
        npb++;
      }
      flg = UNEQUAL;
  } }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("packit");
    }
#endif
  return npb;

} /* packit */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

static filltemp( dest, src )
BYTE_type*src, *dest;
{
  register unsigned char ch, zero, acht;
  int i, j;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("filltemp");
    }
#endif

  zero = '\0';
  acht = 8;
  i = BYTES_WIDE;
  while( i-- )
  { ch = zero; 
    j = acht;
    while( j-- )
    { ch <<= 1;
      if( *src++ )
	ch++;
    }
    *dest++ = ch;
  }
LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("filltemp");
    }
#endif

    return;

} /* filltemp */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

static sendbytes( pb, npb )
BYTE_type*pb;
register int npb;
{ BYTE_type *b;

  b = pb;
  while( npb-- )
    (*macp_PutByte)( macp_Parameter, *b++ );
} /* sendbytes */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - */

static header()
{ register int i;
  register char ch;

  /* header contains nothing ... */
  ch = '\0';
  for(i = 0; i < HEADER_LENGTH; i++ )
    (*macp_PutByte)( macp_Parameter, ch );
} /* header */



/*  End of file .c  */
