/* pgmtopbm.c - read a portable graymap and write a portable bitmap
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
#include "pbm.h"
#include "dithers.h"
#ifdef SYSV
#include <string.h>
#define srandom srand
#define random rand
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define max(a,b) ((a) > (b) ? (a) : (b))

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register gray *grayrow, *gP;
    register bit *bitrow, *bP;
    int argn, rows, cols, format, row, col, limitcol;
    float fthreshval;
    gray maxval;
    char *usage = "[-floyd|-fs | -threshold | -dither8|-d8 |\n     -cluster3|-c3|-cluster4|-c4|-cluster8|-c8] [-value <val>] [pgmfile]";
    int halftone;
#define QT_FS 1
#define QT_THRESH 2
#define QT_DITHER8 3
#define QT_CLUSTER3 4
#define QT_CLUSTER4 5
#define QT_CLUSTER8 6
    long threshval, sum, *thiserr, *nexterr, *temperr;
#define FS_SCALE 1024
#define HALF_FS_SCALE 512
    int fs_direction;

    pm_progname = argv[0];

    argn = 1;
    halftone = QT_FS;	/* default quantization is Floyd-Steinberg */
    fthreshval = 0.5;

    while ( argn < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-fs",max(strlen(argv[argn]),2)) == 0 ||
	     strncmp(argv[argn],"-floyd",max(strlen(argv[argn]),2)) == 0 )
	    halftone = QT_FS;
	else if ( strncmp(argv[argn],"-threshold",max(strlen(argv[argn]),2)) == 0 )
	    halftone = QT_THRESH;
	else if ( strncmp(argv[argn],"-dither8",max(strlen(argv[argn]),2)) == 0 ||
	          strncmp(argv[argn],"-d8",max(strlen(argv[argn]),3)) == 0 )
	    halftone = QT_DITHER8;
	else if ( strncmp(argv[argn],"-cluster3",max(strlen(argv[argn]),9)) == 0 ||
	          strncmp(argv[argn],"-c3",max(strlen(argv[argn]),3)) == 0 )
	    halftone = QT_CLUSTER3;
	else if ( strncmp(argv[argn],"-cluster4",max(strlen(argv[argn]),9)) == 0 ||
	          strncmp(argv[argn],"-c4",max(strlen(argv[argn]),3)) == 0 )
	    halftone = QT_CLUSTER4;
	else if ( strncmp(argv[argn],"-cluster8",max(strlen(argv[argn]),9)) == 0 ||
	          strncmp(argv[argn],"-c8",max(strlen(argv[argn]),3)) == 0 )
	    halftone = QT_CLUSTER8;
	else if ( strncmp(argv[argn],"-value",max(strlen(argv[argn]),2)) == 0 )
	    {
	    argn++;
	    if ( argn == argc || sscanf( argv[argn], "%g", &fthreshval ) != 1 ||
		 fthreshval < 0.0 || fthreshval > 1.0 )
		pm_usage( usage );
	    }
	else
	    pm_usage( usage );
	argn++;
	}

    if ( argn != argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    pgm_readpgminit( ifd, &cols, &rows, &maxval, &format );
    grayrow = pgm_allocrow( cols );

    pbm_writepbminit( stdout, cols, rows );
    bitrow = pbm_allocrow( cols );

    /* Initialize. */
    switch ( halftone )
	{
	case QT_FS:
	/* Initialize Floyd-Steinberg error vectors. */
	thiserr = (long *) pm_allocrow( cols + 2, sizeof(long) );
	nexterr = (long *) pm_allocrow( cols + 2, sizeof(long) );
	srandom( (int) time( 0 ) );
	for ( col = 0; col < cols + 2; col++ )
	    thiserr[col] = ( random( ) % FS_SCALE - HALF_FS_SCALE ) / 4;
	    /* (random errors in [-FS_SCALE/8 .. FS_SCALE/8]) */
	fs_direction = 1;
	threshval = fthreshval * FS_SCALE;
	break;

	case QT_THRESH:
	threshval = fthreshval * maxval;
	break;

	case QT_DITHER8:
	/* Scale dither matrix. */
	for ( row = 0; row < 16; row++ )
	    for ( col = 0; col < 16; col++ )
		dither8[row][col] = dither8[row][col] * ( maxval + 1 ) / 256;
	break;

	case QT_CLUSTER3:
	/* Scale order-3 clustered dither matrix. */
	for ( row = 0; row < 6; row++ )
	    for ( col = 0; col < 6; col++ )
		cluster3[row][col] = cluster3[row][col] * ( maxval + 1 ) / 18;
	break;

	case QT_CLUSTER4:
	/* Scale order-4 clustered dither matrix. */
	for ( row = 0; row < 8; row++ )
	    for ( col = 0; col < 8; col++ )
		cluster4[row][col] = cluster4[row][col] * ( maxval + 1 ) / 32;
	break;

	case QT_CLUSTER8:
	/* Scale order-8 clustered dither matrix. */
	for ( row = 0; row < 16; row++ )
	    for ( col = 0; col < 16; col++ )
		cluster8[row][col] = cluster8[row][col] * ( maxval + 1 ) / 128;
	break;

	default:
	pm_error( "can't happen", 0,0,0,0,0 );
	exit( 1 );
	}

    for ( row = 0; row < rows; row++ )
	{
	pgm_readpgmrow( ifd, grayrow, cols, maxval, format );

	switch ( halftone )
	    {
	    case QT_FS:
	    for ( col = 0; col < cols + 2; col++ )
		nexterr[col] = 0;
	    if ( fs_direction )
		{
		col = 0;
		limitcol = cols;
		gP = grayrow;
		bP = bitrow;
		}
	    else
		{
		col = cols - 1;
		limitcol = -1;
		gP = &(grayrow[col]);
		bP = &(bitrow[col]);
		}
	    do
		{
		sum = ( (long) *gP * FS_SCALE ) / maxval + thiserr[col + 1];
		if ( sum >= threshval )
		    {
		    *bP = PBM_WHITE;
		    sum = sum - threshval - HALF_FS_SCALE;
		    }
		else
		    *bP = PBM_BLACK;

		if ( fs_direction )
		    {
		    thiserr[col + 2] += ( sum * 7 ) / 16;
		    nexterr[col    ] += ( sum * 3 ) / 16;
		    nexterr[col + 1] += ( sum * 5 ) / 16;
		    nexterr[col + 2] += ( sum     ) / 16;

		    col++;
		    gP++;
		    bP++;
		    }
		else
		    {
		    thiserr[col    ] += ( sum * 7 ) / 16;
		    nexterr[col + 2] += ( sum * 3 ) / 16;
		    nexterr[col + 1] += ( sum * 5 ) / 16;
		    nexterr[col    ] += ( sum     ) / 16;

		    col--;
		    gP--;
		    bP--;
		    }
		}
	    while ( col != limitcol );
	    temperr = thiserr;
	    thiserr = nexterr;
	    nexterr = temperr;
	    fs_direction = ! fs_direction;
	    break;

	    case QT_THRESH:
	    for ( col = 0, gP = grayrow, bP = bitrow; col < cols; col++, gP++, bP++ )
		if ( *gP >= threshval )
		    *bP = PBM_WHITE;
		else
		    *bP = PBM_BLACK;
	    break;

	    case QT_DITHER8:
	    for ( col = 0, gP = grayrow, bP = bitrow; col < cols; col++, gP++, bP++ )
		if ( *gP >= dither8[row % 16][col % 16] )
		    *bP = PBM_WHITE;
		else
		    *bP = PBM_BLACK;
	    break;

	    case QT_CLUSTER3:
	    for ( col = 0, gP = grayrow, bP = bitrow; col < cols; col++, gP++, bP++ )
		if ( *gP >= cluster3[row % 6][col % 6] )
		    *bP = PBM_WHITE;
		else
		    *bP = PBM_BLACK;
	    break;

	    case QT_CLUSTER4:
	    for ( col = 0, gP = grayrow, bP = bitrow; col < cols; col++, gP++, bP++ )
		if ( *gP >= cluster4[row % 8][col % 8] )
		    *bP = PBM_WHITE;
		else
		    *bP = PBM_BLACK;
	    break;

	    case QT_CLUSTER8:
	    for ( col = 0, gP = grayrow, bP = bitrow; col < cols; col++, gP++, bP++ )
		if ( *gP >= cluster8[row % 16][col % 16] )
		    *bP = PBM_WHITE;
		else
		    *bP = PBM_BLACK;
	    break;

	    default:
	    pm_error( "can't happen", 0,0,0,0,0 );
	    exit( 1 );
	    }

	pbm_writepbmrow( stdout, bitrow, cols );
	}

    pm_close( ifd );

    exit( 0 );
    }
