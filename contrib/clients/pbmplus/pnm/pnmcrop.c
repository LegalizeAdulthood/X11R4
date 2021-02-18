/* pnmcrop.c - crop a portable anymap
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
#include "pnm.h"
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
    register xel **xels, *xP;
    xel background;
    xelval maxval;
    int argn, format, backdefault, backblack;
    int rows, cols, row, col, newrows, newcols;
    int top, bottom, left, right;
    char *usage = "[-white|-black] [pnmfile]";

    pm_progname = argv[0];

    argn = 1;
    backdefault = 1;

    /* Check for flags. */
    if ( argn < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-white",max(strlen(argv[argn]),2)) == 0 )
	    {
	    backdefault = 0;
	    backblack = 0;
	    }
	else if ( strncmp(argv[argn],"-black",max(strlen(argv[argn]),2)) == 0 )
	    {
	    backdefault = 0;
	    backblack = 1;
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

    xels = pnm_readpnm( ifd, &cols, &rows, &maxval, &format );
    pm_close( ifd );

    if ( backdefault )
	background = pnm_backgroundxel( xels, cols, rows, maxval, format );
    else
	if ( backblack )
	    background = pnm_blackxel( maxval, format );
	else
	    background = pnm_whitexel( maxval, format );

    /* Find first non-background line. */
    for ( top = 0; top < rows; top++ )
	for ( col = 0, xP = xels[top]; col < cols; col++, xP++ )
	    if ( ! PNM_EQUAL( *xP, background ) )
		goto gottop;
gottop:

    /* Find last non-background line. */
    for ( bottom = rows - 1; bottom >= top; bottom-- )
	for ( col = 0, xP = xels[bottom]; col < cols; col++, xP++ )
	    if ( ! PNM_EQUAL( *xP, background ) )
		goto gotbottom;
gotbottom:

    /* Find first non-background column.  To avoid massive paging on
    ** large anymaps, we use a different loop than the above two cases. */
    left = cols - 1;
    for ( row = top; row <= bottom; row++ )
	{
	int thisleft;

	for ( thisleft = 0; thisleft < left; thisleft++ )
	    if ( ! PNM_EQUAL( xels[row][thisleft], background ) )
		{
		left = thisleft;
		break;
		}
	}

    /* Find last non-background column.  Again, use row-major loop. */
    right = left + 1;
    for ( row = top; row <= bottom; row++ )
	{
	int thisright;

	for ( thisright = cols - 1; thisright > right; thisright-- )
	    if ( ! PNM_EQUAL( xels[row][thisright], background ) )
		{
		right = thisright;
		break;
		}
	}

    if ( top > 0 )
	fprintf( stderr, "(cropping %d rows off the top)\n", top );
    if ( bottom < rows - 1 )
	fprintf( stderr, "(cropping %d rows off the bottom)\n", rows - 1 - bottom );
    if ( left > 0 )
	fprintf( stderr, "(cropping %d cols off the left)\n", left );
    if ( right < cols - 1 )
	fprintf( stderr, "(cropping %d cols off the right)\n", cols - 1 - right );

    /* Now write out the new anymap. */
    newcols = right - left + 1;
    newrows = bottom - top + 1;
    pnm_writepnminit( stdout, newcols, newrows, maxval, format );
    for ( row = top; row <= bottom; row++ )
	pnm_writepnmrow( stdout, &(xels[row][left]), newcols, maxval, format );

    exit( 0 );
    }
