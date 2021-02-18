/* ppmshear.c - read a portable pixmap and shear it by some angle
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
    pixel **pixels;
    register pixel *newpixelrow, *pP, *npP;
    pixel bgpixel, prevpixel;
    int argn, rows, cols, newcols, row, col;
    pixval maxval;
    float fangle, shearfac, new0;
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
    if ( fangle <= -90.0 || fangle >= 90.0 )
	pm_error( "angle must be between -90 and 90", 0,0,0,0,0 );
    fangle = fangle * M_PI / 180.0;	/* convert to radians */
    shearfac = tan( fangle );
    if ( shearfac < 0.0 )
	shearfac = -shearfac;

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

    newcols = rows * shearfac + cols + 0.999999;

    ppm_writeppminit( stdout, newcols, rows, maxval );
    newpixelrow = ppm_allocrow( newcols );

    bgpixel = ppm_backgroundpixel( pixels, cols, rows );

    for ( row = 0; row < rows; row++ )
	{
	if ( fangle > 0.0 )
	    new0 = row * shearfac;
	else
	    new0 = ( rows - row ) * shearfac;
	intnew0 = (int) new0;
	fracnew0 = ( new0 - intnew0 ) * SCALE;
	omfracnew0 = SCALE - fracnew0;

	for ( col = 0, npP = newpixelrow; col < newcols; col++, npP++ )
	    *npP = bgpixel;

	prevpixel = bgpixel;

	for ( col = 0, npP = &(newpixelrow[intnew0]), pP = pixels[row]; col < cols; col++, npP++, pP++ )
	    {
	    PPM_ASSIGN( *npP,
		( fracnew0 * PPM_GETR(prevpixel) + omfracnew0 * PPM_GETR(*pP) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETG(prevpixel) + omfracnew0 * PPM_GETG(*pP) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETB(prevpixel) + omfracnew0 * PPM_GETB(*pP) + HALFSCALE ) / SCALE );
	    prevpixel = *pP;
	    }
	if ( fracnew0 > 0 )
	    {
	    npP = &(newpixelrow[intnew0 + cols]);
	    PPM_ASSIGN( *npP,
		( fracnew0 * PPM_GETR(prevpixel) + omfracnew0 * PPM_GETR(bgpixel) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETG(prevpixel) + omfracnew0 * PPM_GETG(bgpixel) + HALFSCALE ) / SCALE,
		( fracnew0 * PPM_GETB(prevpixel) + omfracnew0 * PPM_GETB(bgpixel) + HALFSCALE ) / SCALE );
	    }

	ppm_writeppmrow( stdout, newpixelrow, newcols, maxval );
	}

    exit( 0 );
    }
