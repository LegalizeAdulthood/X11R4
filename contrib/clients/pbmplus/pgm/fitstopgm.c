/* fitstopgm.c - read a FITS file and produce a portable graymap
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
#include "pgm.h"
#ifdef SYSV
#include <string.h>
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define min(a,b) ((a) < (b) ? (a) : (b))

struct FITS_Header {
    int simple;		/* basic format or not */
    int bitpix;		/* number of bits per pixel */
    int naxis;		/* number of axes */
    int naxis1;		/* number of points on axis 1 */
    int naxis2;		/* number of points on axis 2 */
    double datamin;	/* min # */
    double datamax;	/* max # */
    double bzero;	/* ??? */
    double bscale;	/* ??? */
    };

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register gray *grayrow, *gP;
    int argn, row;
    register int col;
    gray maxval;
    double fmaxval, scale;
    int rows, cols;
    struct FITS_Header h;

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
	pm_usage( "[fitsfile]" );

    read_fits_header( ifd, &h );

    if ( ! h.simple )
	pm_error( "FITS file is not in simple format, can't read", 0,0,0,0,0 );
    switch ( h.bitpix )
	{
	case 8:
	fmaxval = 255.0;
	break;

	case 16:
	fmaxval = 65535.0;
	break;

	case 32:
	fmaxval = 4294967295.0;
	break;

	default:
	pm_error( "unusual bits per pixel (%d), can't read", h.bitpix, 0,0,0,0 );
	}
    if ( h.naxis != 2 )
	pm_error( "FITS file has %d axes, can't read", h.naxis, 0,0,0,0 );
    cols = h.naxis1;
    rows = h.naxis2;
    maxval = min( fmaxval, PGM_MAXMAXVAL );
    scale = maxval / ( h.datamax - h.datamin );

    pgm_writepgminit( stdout, cols, rows, maxval );
    grayrow = pgm_allocrow( cols );
    for ( row = 0; row < rows; row++)
	{
	for ( col = 0, gP = grayrow; col < cols; col++, gP++ )
	    {
	    int ich;
	    double val;

	    switch ( h.bitpix )
		{
		case 8:
		ich = getc( ifd );
		if ( ich == EOF )
		    pm_error( "premature EOF", 0,0,0,0,0 );
		val = ich;
		break;

		case 16:
		ich = getc( ifd );
		if ( ich == EOF )
		    pm_error( "premature EOF", 0,0,0,0,0 );
		val = ich << 8;
		ich = getc( ifd );
		if ( ich == EOF )
		    pm_error( "premature EOF", 0,0,0,0,0 );
		val += ich;
		break;

		case 32:
		ich = getc( ifd );
		if ( ich == EOF )
		    pm_error( "premature EOF", 0,0,0,0,0 );
		val = ich << 24;
		ich = getc( ifd );
		if ( ich == EOF )
		    pm_error( "premature EOF", 0,0,0,0,0 );
		val += ich << 16;
		ich = getc( ifd );
		if ( ich == EOF )
		    pm_error( "premature EOF", 0,0,0,0,0 );
		val += ich << 8;
		ich = getc( ifd );
		if ( ich == EOF )
		    pm_error( "premature EOF", 0,0,0,0,0 );
		val += ich;
		break;

		default:
		pm_error( "can't happen", 0,0,0,0,0 );
		}
	    *gP = (gray) ( scale * ( val * h.bscale + h.bzero - h.datamin) );
	    }
	pgm_writepgmrow( stdout, grayrow, cols, maxval );
	}
    pm_close( ifd );

    exit( 0 );
    }

read_fits_header( fd, hP )
FILE *fd;
struct FITS_Header *hP;
    {
    char buf[80];
    int seen_end;
    int i;
    char c;

    seen_end = 0;
    hP->simple = 0;
    hP->bzero = 0.0;
    hP->bscale = 1.0;
    hP->datamin = 0.0;
    hP->datamax = 1.0;

    while ( ! seen_end )
	for ( i = 0; i < 36; i++ )
	    {
	    read_card( fd, buf );

	    if ( sscanf( buf, "SIMPLE = %c", &c ) == 1 )
		{
		if ( c == 'T' || c == 't' )
		    hP->simple = 1;
		}
	    else if ( sscanf( buf, "BITPIX = %d", &(hP->bitpix) ) == 1 );
	    else if ( sscanf( buf, "NAXIS = %d", &(hP->naxis) ) == 1 );
	    else if ( sscanf( buf, "NAXIS1 = %d", &(hP->naxis1) ) == 1 );
	    else if ( sscanf( buf, "NAXIS2 = %d", &(hP->naxis2) ) == 1 );
	    else if ( sscanf( buf, "DATAMIN = %lg", &(hP->datamin) ) == 1 );
	    else if ( sscanf( buf, "DATAMAX = %lg", &(hP->datamax) ) == 1 );
	    else if ( sscanf( buf, "BZERO = %lg", &(hP->bzero) ) == 1 );
	    else if ( sscanf( buf, "BSCALE = %lg", &(hP->bscale) ) == 1 );
	    else if ( strncmp( buf, "END ", 4 ) == 0 ) seen_end = 1;
	    }
    }

read_card( fd, buf )
FILE *fd;
char *buf;
    {
    if ( fread( buf, 1, 80, fd ) == 0 )
	pm_error( "error reading header", 0,0,0,0,0 );
    }
