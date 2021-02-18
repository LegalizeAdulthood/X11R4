/* pgmnorm.c - read a portable graymap and normalize the contrast
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

#define max(a,b) ((a) > (b) ? (a) : (b))
#define MAXMAXVAL 1023

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    gray maxval, **grays, *grayrow;
    register gray range, *gP;
    int argn, rows, cols, format, row;
    int hist[MAXMAXVAL+1], i, size, cutoff, count;
    float bpercent, wpercent;
    int bvalue, wvalue;
    int specbpercent, specbvalue, specwpercent, specwvalue;
    register int col;
    char *usage = "[-bpercent <n> | -bvalue <n>] [-wpercent <n> | -wvalue <n>] [pgmfile]";

    pm_progname = argv[0];

    argn = 1;
    bpercent = 2.0;
    wpercent = 1.0;
    specbpercent = specbvalue = specwpercent = specwvalue = 0;

    while ( argn + 1 < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-bpercent",max(strlen(argv[argn]),3)) == 0 )
	    {
	    if ( specbvalue )
		pm_error(
		    "only one of -bpercent and -bvalue may be specified",
		    0,0,0,0,0 );
	    if ( sscanf( argv[argn+1], "%g", &bpercent ) != 1 )
		pm_usage( usage );
	    if ( bpercent < 0.0  || bpercent > 100.0 )
		pm_error(
		    "black percentage must between 0 and 100", 0,0,0,0,0 );
	    specbpercent = 1;
	    }
	else if ( strncmp(argv[argn],"-bvalue",max(strlen(argv[argn]),3)) == 0 )
	    {
	    if ( specbpercent )
		pm_error(
		    "only one of -bpercent and -bvalue may be specified",
		    0,0,0,0,0 );
	    if ( sscanf( argv[argn+1], "%d", &bvalue ) != 1 )
		pm_usage( usage );
	    if ( bvalue < 0 )
		pm_error( "black value must be >= 0", 0,0,0,0,0 );
	    specbvalue = 1;
	    }
	else if ( strncmp(argv[argn],"-wpercent",max(strlen(argv[argn]),3)) == 0 )
	    {
	    if ( specwvalue )
		pm_error(
		    "only one of -wpercent and -wvalue may be specified",
		    0,0,0,0,0 );
	    if ( sscanf( argv[argn+1], "%g", &wpercent ) != 1 )
		pm_usage( usage );
	    if ( wpercent < 0.0 || wpercent > 100.0 )
		pm_error(
		    "white percentage must be between 0 and 100", 0,0,0,0,0 );
	    specwpercent = 1;
	    }
	else if ( strncmp(argv[argn],"-wvalue",max(strlen(argv[argn]),3)) == 0 )
	    {
	    if ( specwpercent )
		pm_error(
		    "only one of -wpercent and -wvalue may be specified",
		    0,0,0,0,0 );
	    if ( sscanf( argv[argn+1], "%d", &wvalue ) != 1 )
		pm_usage( usage );
	    if ( wvalue < 0 )
		pm_error( "white value must be >= 0", 0,0,0,0,0 );
	    specwvalue = 1;
	    }
	else
	    pm_usage( usage );
	argn += 2;
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

    if ( specbvalue && specwvalue )
	{
	/* Rescale so that bvalue maps to 0, wvalue maps to maxval. */
	pgm_readpgminit( ifd, &cols, &rows, &maxval, &format );
	grayrow = pgm_allocrow( cols );
	pgm_writepgminit( stdout, cols, rows, maxval );
	range = wvalue - bvalue;
	for ( row = 0; row < rows; row++ )
	    {
	    pgm_readpgmrow( ifd, grayrow, cols, maxval, format );
	    for ( col = 0, gP = grayrow; col < cols; col++, gP++ )
		{
		if ( *gP <= bvalue )
		    *gP = 0;
		else if ( *gP >= wvalue )
		    *gP = maxval;
		else
		    *gP = ( *gP - bvalue ) * maxval / range;
		}
	    pgm_writepgmrow( stdout, grayrow, cols, maxval );
	    }
	pm_close( ifd );
	}
    else
	{
	grays = pgm_readpgm( ifd, &cols, &rows, &maxval );
	pm_close( ifd );

	/* Build histogram. */
	for ( i = 0; i <= maxval; i++ )
	    hist[i] = 0;
	for ( row = 0; row < rows; row++ )
	    for ( col = 0, gP = grays[row]; col < cols; col++, gP++ )
		hist[*gP]++;
	size = rows * cols;
	if ( ! specbvalue )
	    { /* Compute bvalue from bpercent. */
	    cutoff = size * bpercent / 100.0;
	    count = 0;
	    for ( bvalue = 0; bvalue <= maxval; bvalue++ )
		{
		count += hist[bvalue];
		if ( count > cutoff )
		break;
		}
	    }
	if ( ! specwvalue )
	    { /* Compute wvalue from wpercent. */
	    cutoff = size * wpercent / 100.0;
	    count = 0;
	    for ( wvalue = maxval; wvalue >= 0; wvalue-- )
		{
		count += hist[wvalue];
		if ( count > cutoff )
		    break;
		}
	    }

	/* Now rescale so that bvalue maps to 0, wvalue maps to maxval. */
	fprintf(
	    stderr, "(Remapping %d..%d to %d..%d.)\n", bvalue, wvalue, 0,
	    maxval );
	pgm_writepgminit( stdout, cols, rows, maxval );
	range = wvalue - bvalue;
	for ( row = 0; row < rows; row++ )
	    {
	    for ( col = 0, gP = grays[row]; col < cols; col++, gP++ )
		{
		if ( *gP <= bvalue )
		    *gP = 0;
		else if ( *gP >= wvalue )
		    *gP = maxval;
		else
		    *gP = ( *gP - bvalue ) * maxval / range;
		}
	    pgm_writepgmrow( stdout, grays[row], cols, maxval );
	    }
	}

    exit( 0 );
    }
