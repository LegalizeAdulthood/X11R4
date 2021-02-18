/* imgtoppm.c - read an Img-whatnot file and produce a portable pixmap
**
** Based on a simple conversion program posted to comp.graphics by Ed Falk.
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
#ifdef SYSV
#include <string.h>
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    pixel *pixelrow, colormap[256];
    register pixel *pP;
    int argn, rows, cols, row, i;
    register int col;
    pixval maxval;
    int len, cmaplen, gotAT, gotCM;
    unsigned char buf[4096];
    register unsigned char *bP;

    pm_progname = argv[0];

    argn = 1;

    if ( argn < argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( "[imgfile]" );

    /* Get signature. */
    fread( buf, 8, 1, ifd );
    buf[8] = '\0';

    /* Get entries. */
    gotAT = 0;
    gotCM = 0;
    while ( fread( buf, 2, 1, ifd ) == 1 )
	{
	if ( strncmp( buf, "AT", 2 ) == 0 )
	    {
	    if ( fread( buf, 8, 1, ifd ) != 1 )
		pm_error( "bad attributes header", 0,0,0,0,0 );
	    buf[8] = '\0';
	    len = atoi( buf );
	    if ( fread( buf, len, 1, ifd ) != 1 )
		pm_error( "bad attributes buf", 0,0,0,0,0 );
	    buf[len] = '\0';
	    sscanf( buf, "%4u%4u%4u", &cols, &rows, &cmaplen );
	    maxval = 255;
	    gotAT = 1;
	    }

	else if ( strncmp( buf, "CM", 2 ) == 0 )
	    {
	    if ( ! gotAT )
		pm_error( "missing attributes header", 0,0,0,0,0 );
	    if ( fread( buf, 8, 1, ifd ) != 1 )
		pm_error( "bad colormap header", 0,0,0,0,0 );
	    buf[8] = '\0';
	    len = atoi( buf );
	    if ( fread( buf, len, 1, ifd ) != 1 )
		pm_error( "bad colormap buf", 0,0,0,0,0 );
	    if ( cmaplen * 3 != len )
		{
		pm_message(
		    "cmaplen (%d) and colormap buf length (%d) do not match",
		    cmaplen, len, 0, 0, 0 );
		if ( cmaplen * 3 < len )
		    len = cmaplen * 3;
		else if ( cmaplen * 3 > len )
		    cmaplen = len / 3;
		}
	    for ( i = 0; i < len; i += 3 )
		PPM_ASSIGN( colormap[i / 3], buf[i], buf[i + 1], buf[i + 2] );
	    gotCM = 1;
	    }

	else if ( strncmp( buf, "PD", 2 ) == 0 )
	    {
	    if ( fread( buf, 8, 1, ifd ) != 1 )
		pm_error( "bad pixel data header", 0,0,0,0,0 );
	    buf[8] = '\0';
	    len = atoi( buf );
	    if ( len != cols * rows )
		pm_message(
		    "pixel data length (%d) does not match image size (%d)",
		    len, cols * rows, 0, 0, 0 );

	    ppm_writeppminit( stdout, cols, rows, maxval );
	    pixelrow = ppm_allocrow( cols );

	    for ( row = 0; row < rows; row++ )
		{
		if ( fread( buf, 1, cols, ifd ) != cols )
		    pm_error( "premature EOF", 0,0,0,0,0 );
		for ( col = 0, pP = pixelrow, bP = buf;
		      col < cols; col++, pP++, bP++ )
		    {
		    if ( gotCM )
			*pP = colormap[*bP];
		    else
			PPM_ASSIGN( *pP, *bP, *bP, *bP );
		    }
		ppm_writeppmrow( stdout, pixelrow, cols, maxval );
		}
	    pm_close( ifd );
	    exit( 0 );
	    }
	}
    }
