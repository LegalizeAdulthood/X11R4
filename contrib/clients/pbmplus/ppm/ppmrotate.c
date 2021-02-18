/* ppmrotate.c - read a portable pixmap and rotate it by some angle
**
** Copyright (C) 1989 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include <stdio.h>
#include <math.h>
#define M_PI	3.14159265358979323846
#include "ppm.h"

#define SCALE 4096
#define HALFSCALE 2048

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    pixel **pixels, **temp1pixels, **temp2pixels, *newpixelrow;
    register pixel *pP, *npP;
    pixel bgpixel, prevpixel, p;
    int argn, rows, cols, newrows, tempcols, newcols, yshearjunk, x2shearjunk, row, col, new;
    pixval maxval;
    float fangle, xshearfac, yshearfac, new0;
    int intnew0;
    register long fracnew0, omfracnew0;
    char *usage = "<angle> [ppmfile]";

    pm_progname = argv[0];

    argn = 1;

    if ( argn == argc )
	pm_usage( usage );
    if ( sscanf( argv[argn], "%g", &fangle ) != 1 )
	pm_usage( usage );
    argn++;
    if ( fangle < -90.0 || fangle > 90.0 )
	pm_error( "angle must be between -90 and 90", 0,0,0,0,0 );
    fangle = fangle * M_PI / 180.0;	/* convert to radians */

    xshearfac = tan( fangle / 2.0 );
    if ( xshearfac < 0.0 )
	xshearfac = -xshearfac;
    yshearfac = sin( fangle );
    if ( yshearfac < 0.0 )
	yshearfac = -yshearfac;

    if ( argn != argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    ppm_pbmmaxval = 255;	/* use larger value for better results */
    pixels = ppm_readppm( ifd, &cols, &rows, &maxval );

    pm_close( ifd );

    tempcols = rows * xshearfac + cols + 0.999999;
    yshearjunk = ( tempcols - cols ) * yshearfac;
    newrows = tempcols * yshearfac + rows + 0.999999;
    x2shearjunk = ( newrows - rows - yshearjunk ) * xshearfac;
    newrows -= 2 * yshearjunk;
    newcols = newrows * xshearfac + tempcols + 0.999999 - 2 * x2shearjunk;

    bgpixel = ppm_backgroundpixel( pixels, cols, rows );

    /* First shear X into temp1pixels. */
    temp1pixels = ppm_allocarray( tempcols, rows );
    for ( row = 0; row < rows; row++ )
	{
	for ( col = 0, npP = temp1pixels[row]; col < tempcols; col++, npP++ )
	    *npP = bgpixel;

	if ( fangle > 0 )
	    new0 = row * xshearfac;
	else
	    new0 = ( rows - row ) * xshearfac;
	intnew0 = (int) new0;
	fracnew0 = ( new0 - intnew0 ) * SCALE;
	omfracnew0 = SCALE - fracnew0;

	prevpixel = bgpixel;

	for ( col = 0, npP = &(temp1pixels[row][intnew0]), pP = pixels[row]; col < cols; col++, npP++, pP++ )
	    {
	    PPM_ASSIGN( *npP,
		( fracnew0 * PPM_GETR(prevpixel) + omfracnew0 * PPM_GETR(*pP) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETG(prevpixel) + omfracnew0 * PPM_GETG(*pP) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETB(prevpixel) + omfracnew0 * PPM_GETB(*pP) + HALFSCALE ) / SCALE );
	    prevpixel = *pP;
	    }
	if ( fracnew0 > 0 && intnew0 + cols < tempcols )
	    {
	    PPM_ASSIGN( *npP,
		( fracnew0 * PPM_GETR(prevpixel) + omfracnew0 * PPM_GETR(bgpixel) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETG(prevpixel) + omfracnew0 * PPM_GETG(bgpixel) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETB(prevpixel) + omfracnew0 * PPM_GETB(bgpixel) + HALFSCALE ) / SCALE );
	    }

	}
    ppm_freearray( pixels, rows );

    /* Now inverse shear Y from temp1 into temp2. */
    temp2pixels = ppm_allocarray( tempcols, newrows );
    for ( col = 0; col < tempcols; col++ )
	{
	for ( row = 0; row < newrows; row++ )
	    temp2pixels[row][col] = bgpixel;

	if ( fangle > 0 )
	    new0 = ( tempcols - col ) * yshearfac;
	else
	    new0 = col * yshearfac;
	intnew0 = (int) new0;
	fracnew0 = ( new0 - intnew0 ) * SCALE;
	omfracnew0 = SCALE - fracnew0;
	intnew0 -= yshearjunk;

	prevpixel = bgpixel;

	for ( row = 0; row < rows; row++ )
	    {
	    new = intnew0 + row;
	    if ( new >= 0 && new < newrows )
		{
		npP = &(temp2pixels[new][col]);
		p = temp1pixels[row][col];
		PPM_ASSIGN( *npP,
		    ( fracnew0 * PPM_GETR(prevpixel) + omfracnew0 * PPM_GETR(p) + HALFSCALE ) / SCALE,
		    ( fracnew0 * PPM_GETG(prevpixel) + omfracnew0 * PPM_GETG(p) + HALFSCALE ) / SCALE,
		    ( fracnew0 * PPM_GETB(prevpixel) + omfracnew0 * PPM_GETB(p) + HALFSCALE ) / SCALE );
		prevpixel = p;
		}
	    }
	if ( fracnew0 > 0 && intnew0 + rows < newrows )
	    {
	    npP = &(temp2pixels[intnew0 + rows][col]);
	    PPM_ASSIGN( *npP,
		( fracnew0 * PPM_GETR(prevpixel) + omfracnew0 * PPM_GETR(bgpixel) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETG(prevpixel) + omfracnew0 * PPM_GETG(bgpixel) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETB(prevpixel) + omfracnew0 * PPM_GETB(bgpixel) + HALFSCALE ) / SCALE );
	    }
	}
    ppm_freearray( temp1pixels, rows );

    /* Finally, shear X from temp2 into newpixelrow. */
    ppm_writeppminit( stdout, newcols, newrows, maxval );
    newpixelrow = ppm_allocrow( newcols );
    for ( row = 0; row < newrows; row++ )
	{
	for ( col = 0, npP = newpixelrow; col < newcols; col++, npP++ )
	    *npP = bgpixel;

	if ( fangle > 0 )
	    new0 = row * xshearfac;
	else
	    new0 = ( newrows - row ) * xshearfac;
	intnew0 = (int) new0;
	fracnew0 = ( new0 - intnew0 ) * SCALE;
	omfracnew0 = SCALE - fracnew0;
	intnew0 -= x2shearjunk;

	prevpixel = bgpixel;

	for ( col = 0, pP = temp2pixels[row]; col < tempcols; col++, pP++ )
	    {
	    new = intnew0 + col;
	    if ( new >= 0 && new < newcols )
		{
		npP = &(newpixelrow[new]);
		PPM_ASSIGN( *npP,
		    ( fracnew0 * PPM_GETR(prevpixel) + omfracnew0 * PPM_GETR(*pP) + HALFSCALE ) / SCALE,
		    ( fracnew0 * PPM_GETG(prevpixel) + omfracnew0 * PPM_GETG(*pP) + HALFSCALE ) / SCALE,
		    ( fracnew0 * PPM_GETB(prevpixel) + omfracnew0 * PPM_GETB(*pP) + HALFSCALE ) / SCALE );
		prevpixel = *pP;
		}
	    }
	if ( fracnew0 > 0 && intnew0 + tempcols < newcols )
	    {
	    npP = &(newpixelrow[intnew0 + tempcols]);
	    PPM_ASSIGN( *npP,
		( fracnew0 * PPM_GETR(prevpixel) + omfracnew0 * PPM_GETR(bgpixel) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETG(prevpixel) + omfracnew0 * PPM_GETG(bgpixel) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETB(prevpixel) + omfracnew0 * PPM_GETB(bgpixel) + HALFSCALE ) / SCALE );
	    }
	ppm_writeppmrow( stdout, newpixelrow, newcols, maxval );
	}

    exit( 0 );
    }
