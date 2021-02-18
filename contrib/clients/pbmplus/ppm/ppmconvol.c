/* ppmconvol.c - general MxN convolution on a portable pixmap
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
#include "ppm.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *cifd, *ifd;
    pixel **cpixels, **pixelbuf, *outputrow, p;
    int argn, crows, ccols, ccolso2, crowso2, rows, cols, format, crow, row;
    register int ccol, col;
    pixval cmaxval, maxval, r, g, b;
    float **rweights, **gweights, **bweights, rsum, gsum, bsum;
    char *usage = "<convolutionfile> [ppmfile]";

    pm_progname = argv[0];

    argn = 1;

    if ( argn == argc )
	pm_usage( usage );
    cifd = pm_openr( argv[argn] );
    argn++;

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

    /* Read in the convolution matrix. */
    cpixels = ppm_readppm( cifd, &ccols, &crows, &cmaxval );
    pm_close( cifd );
    if ( ccols % 2 != 1 || crows % 2 != 1 )
	pm_error(
	    "the convolution matrix must have an odd number of rows and columns",
	    0,0,0,0,0 );
    ccolso2 = ccols / 2;
    crowso2 = crows / 2;

    ppm_readppminit( ifd, &cols, &rows, &maxval, &format );
    if ( cols < ccols || rows < crows )
	pm_error(
	    "the image is smaller than the convolution matrix", 0,0,0,0,0 );

    /* Set up the normalized weights. */
    rweights = (float **) pm_allocarray( ccols, crows, sizeof(float) );
    gweights = (float **) pm_allocarray( ccols, crows, sizeof(float) );
    bweights = (float **) pm_allocarray( ccols, crows, sizeof(float) );
    rsum = gsum = bsum = 0;
    for ( crow = 0; crow < crows; crow++ )
	for ( ccol = 0; ccol < ccols; ccol++ )
	    {
	    rsum += rweights[crow][ccol] =
		( PPM_GETR(cpixels[crow][ccol]) * 2.0 / cmaxval - 1.0 );
	    gsum += gweights[crow][ccol] =
		( PPM_GETG(cpixels[crow][ccol]) * 2.0 / cmaxval - 1.0 );
	    bsum += bweights[crow][ccol] =
		( PPM_GETB(cpixels[crow][ccol]) * 2.0 / cmaxval - 1.0 );
	    }
    if ( rsum < 0.9 || rsum > 1.1 || gsum < 0.9 || gsum > 1.1 ||
	 bsum < 0.9 || bsum > 1.1 )
	fprintf( stderr, "WARNING: this convolution matrix is biased.\n" );

    /* Allocate space for one convolution-matrix's worth of rows, plus
    ** a row output buffer. */
    pixelbuf = ppm_allocarray( cols, crows );
    outputrow = ppm_allocrow( cols );

    ppm_writeppminit( stdout, cols, rows, maxval );

    /* Read in one convolution-matrix's worth of image, less one row. */
    for ( row = 0; row < crows - 1; row++ )
	{
	ppm_readppmrow( ifd, pixelbuf[row], cols, maxval, format );
	/* Write out just the part we're not going to convolve. */
	if ( row < crowso2 )
	    ppm_writeppmrow( stdout, pixelbuf[row], cols, maxval );
	}

    /* Now the rest of the image -- read in the row at the end of
    ** pixelbuf, and convolve and write out the row in the middle.
    */
    for ( ; row < rows; row++ )
	{
	ppm_readppmrow( ifd, pixelbuf[row % crows], cols, maxval, format );

	for ( col = 0; col < cols; col++ )
	    if ( col < ccolso2 || col >= cols - ccolso2 )
		outputrow[col] = pixelbuf[(row - crowso2) % crows][col];
	    else
		{
		rsum = gsum = bsum = 0.0;
		for ( crow = 0; crow < crows; crow++ )
		    for ( ccol = 0; ccol < ccols; ccol++ )
			{
			p = pixelbuf[(row+1+crow) % crows][col-ccolso2+ccol];
			rsum += PPM_GETR( p ) * rweights[crow][ccol];
			gsum += PPM_GETG( p ) * gweights[crow][ccol];
			bsum += PPM_GETB( p ) * bweights[crow][ccol];
			}
		if ( rsum < 0.0 ) r = 0;
		else if ( rsum > maxval ) r = maxval;
		else r = rsum + 0.5;
		if ( gsum < 0.0 ) g = 0;
		else if ( gsum > maxval ) g = maxval;
		else g = gsum + 0.5;
		if ( bsum < 0.0 ) b = 0;
		else if ( bsum > maxval ) b = maxval;
		else b = bsum + 0.5;
		PPM_ASSIGN( outputrow[col], r, g, b );
		}

	ppm_writeppmrow( stdout, outputrow, cols, maxval );
	}
    pm_close( ifd );

    /* Now write out the remaining unconvolved rows in pixelbuf. */
    for ( ; row < rows + crowso2; row++ )
	ppm_writeppmrow(
	    stdout, pixelbuf[(row - crowso2) % crows], cols, maxval );

    exit( 0 );
    }
