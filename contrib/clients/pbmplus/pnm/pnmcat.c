/* pnmcat.c - concatenate portable anymaps
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
#include "pnm.h"
#ifdef SYSV
#include <string.h>
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define MAXFILES 100
#define max(a,b) ((a) > (b) ? (a) : (b))

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd[MAXFILES];
    register xel *newxelrow;
    xel *xelrow[MAXFILES], background[MAXFILES];
    xelval maxval[MAXFILES], newmaxval;
    int argn, backdefault, backblack, lrflag, tbflag, nfiles;
    int rows[MAXFILES], cols[MAXFILES], format[MAXFILES], newformat;
    register i, row, col;
    int newrows, newcols, new, pad;
    char *usage = "[-white|-black] pnmfile pnmfile ...";

    pm_progname = argv[0];

    argn = 1;
    backdefault = 1;
    lrflag = tbflag = 0;

    /* Check for flags. */
    while ( argn < argc && argv[argn][0] == '-' && argv[argn][1] != '\0' )
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
	else if ( strncmp(argv[argn],"-lr",max(strlen(argv[argn]),2)) == 0 ||
	          strncmp(argv[argn],"-leftright",max(strlen(argv[argn]),2)) == 0 )
	    lrflag = 1;
	else if ( strncmp(argv[argn],"-tb",max(strlen(argv[argn]),2)) == 0 ||
	          strncmp(argv[argn],"-topbottom",max(strlen(argv[argn]),2)) == 0 )
	    tbflag = 1;
	else
	    pm_usage( usage );
	argn++;
	}

    if ( lrflag && tbflag )
	pm_error( "only one of -lr and -tb may be specified", 0,0,0,0,0 );
    if ( ! ( lrflag || tbflag ) )
	pm_error( "one of -lr or -tb must be specified", 0,0,0,0,0 );

    if ( argn < argc )
	{
	nfiles = argc - argn;
	for ( i = 0; i < nfiles; i++ )
	    ifd[i] = pm_openr( argv[argn+i] );
	}
    else
	{
	nfiles = 1;
	ifd[0] = stdin;
	}

    newcols = 0;
    newrows = 0;
    for ( i = 0; i < nfiles; i++ )
	{
	pnm_readpnminit( ifd[i], &cols[i], &rows[i], &maxval[i], &format[i] );
	if ( i == 0 )
	    {
	    newmaxval = maxval[i];
	    newformat = format[i];
	    }
	else if ( format[i] > newformat )
	    {
	    newmaxval = maxval[i];
	    newformat = format[i];
	    }
	xelrow[i] = pnm_allocrow( cols[i] );
	if ( lrflag )
	    {
	    newcols += cols[i];
	    if ( rows[i] > newrows )
		newrows = rows[i];
	    }
	else
	    {
	    newrows += rows[i];
	    if ( cols[i] > newcols )
		newcols = cols[i];
	    }
	}
    for ( i = 0; i < nfiles; i++ )
	{
	/* Read first row just to get a good guess at the background. */
	pnm_readpnmrow( ifd[i], xelrow[i], cols[i], maxval[i], format[i] );
	pnm_promoteformatrow(
	    xelrow[i], cols[i], maxval[i], format[i], newmaxval, newformat );
	if (  backdefault )
	    background[i] =
		pnm_backgroundxelrow(
		    xelrow[i], cols[i], newmaxval, newformat );
	else
	    if ( backblack )
		background[i] = pnm_blackxel( newmaxval, newformat );
	    else
		background[i] = pnm_whitexel( newmaxval, newformat );
	}

    newxelrow = pnm_allocrow( newcols );

    pnm_writepnminit( stdout, newcols, newrows, newmaxval, newformat );

    if ( lrflag )
	{
	for ( row = 0; row < newrows; row++ )
	    {
	    new = 0;
	    for ( i = 0; i < nfiles; i++ )
		{
		pad = ( newrows - rows[i] ) / 2;
		if ( row >= pad + 1 && row < pad + rows[i] )
		    {
		    pnm_readpnmrow(
			ifd[i], xelrow[i], cols[i], maxval[i], format[i] );
		    pnm_promoteformatrow(
			xelrow[i], cols[i], maxval[i], format[i],
			newmaxval, newformat );
		    }
		for ( col = 0; col < cols[i]; col++ )
		    if ( row >= pad && row < pad + rows[i] )
			newxelrow[new+col] = xelrow[i][col];
		    else
			newxelrow[new+col] = background[i];
		new += cols[i];
		}
	    pnm_writepnmrow( stdout, newxelrow, newcols, newmaxval, newformat );
	    }
	}
    else
	{
	new = 0;
	i = 0;
	pad = ( newcols - cols[i] ) / 2;
	for ( row = 0; row < newrows; row++ )
	    {
	    if ( row - new >= rows[i] )
		{
		new += rows[i];
		++i;
		if ( i >= nfiles )
		    pm_error( "shouldn't happen", 0,0,0,0,0 );
		pad = ( newcols - cols[i] ) / 2;
		}
	    if ( row - new > 0 )
		{
		pnm_readpnmrow(
		    ifd[i], xelrow[i], cols[i], maxval[i], format[i] );
		pnm_promoteformatrow(
		    xelrow[i], cols[i], maxval[i], format[i],
		    newmaxval, newformat );
		}
	    for ( col = 0; col < pad; col++ )
		newxelrow[col] = background[i];
	    for ( col = 0; col < cols[i]; col++ )
		newxelrow[pad+col] = xelrow[i][col];
	    for ( col = pad+cols[i]; col < newcols; col++ )
		newxelrow[col] = background[i];
	    pnm_writepnmrow( stdout, newxelrow, newcols, newmaxval, newformat );
	    }
	}

    for ( i = 0; i < nfiles; i++ )
	pm_close( ifd[i] );

    exit( 0 );
    }
