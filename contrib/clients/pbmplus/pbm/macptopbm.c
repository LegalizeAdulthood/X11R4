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

#include <stdio.h>
#include <sys/types.h>
#include "pbm.h"
#include "macp.h"
#ifdef SYSV
#include <string.h>
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define max(a,b) ((a) > (b) ? (a) : (b))

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    unsigned char Pic[MAX_LINES][BYTES_WIDE];
    register bit *bitrow;
    int argn, extraskip, scanLine, rows, cols, row, bcol, i;
    char *usage = "[-extraskip <n>] [macpfile]";

    pm_progname = argv[0];

    argn = 1;
    extraskip = 0;

    /* Check for flags. */
    if ( argn < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-extraskip",max(strlen(argv[argn]),2)) == 0 )
	    {
	    argn++;
	    if ( argn == argc || sscanf( argv[argn], "%d", &extraskip ) != 1 )
		pm_usage( usage );
	    }
	else
	    pm_usage( usage );
	argn++;
	}

    if ( argn < argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    ReadMacPaintFile( ifd, extraskip, &scanLine, Pic );

    pm_close( ifd );

    cols = BYTES_WIDE * 8;
    rows = scanLine;
    pbm_writepbminit( stdout, cols, rows );
    bitrow = pbm_allocrow( cols );

    for ( row = 0; row < rows; row++ )
	{
	for ( bcol = 0; bcol < BYTES_WIDE; bcol++ )
	    for ( i = 0; i < 8; i++ )
		bitrow[bcol * 8 + i] =
		    ( (Pic[row][bcol] >> (7 - i)) & 1 ) ? PBM_BLACK : PBM_WHITE;
	pbm_writepbmrow( stdout, bitrow, cols );
	}

    exit( 0 );
    }

/*
** Some of the following routine is:
**
**                Copyright 1987 by Patrick J. Naughton
**                         All Rights Reserved
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted,
** provided that the above copyright notice appear in all copies and that
** both that copyright notice and this permission notice appear in
** supporting documentation.
*/

ReadMacPaintFile( file, extraskip, scanLineP, Pic )
FILE *file;
int extraskip, *scanLineP;
unsigned char Pic[MAX_LINES][BYTES_WIDE];
    {
    unsigned int i, j, k;
    unsigned char ch;

    /* Skip over the header. */
    for ( i = 0; i < extraskip; i++ )
	getc( file );
    for ( i = 0; i < HEADER_LENGTH; i++ )
	getc( file );

    *scanLineP = 0;
    k = 0;

    while ( *scanLineP < MAX_LINES )
	{
	ch = (unsigned char) getc( file );	/* Count byte */
	i = (unsigned int) ch;
	if ( ch < 0x80 )
	    {	/* Unpack next (I+1) chars as is */
	    for ( j = 0; j <= i; j++ )
		if ( *scanLineP < MAX_LINES )
		    {
		    Pic[*scanLineP][k++] = (unsigned char) getc( file );
		    if ( ! (k %= BYTES_WIDE) )
			*scanLineP += 1;
		    }
	    }
	else
	    {	/* Repeat next char (2's comp I) times */
	    ch = getc( file );
	    for ( j = 0; j <= 256 - i; j++ )
		if ( *scanLineP < MAX_LINES )
		    {
		    Pic[*scanLineP][k++] = (unsigned char) ch;
		    if ( ! (k %= BYTES_WIDE) )
			*scanLineP += 1;
		    }
	    }
	}
    }
