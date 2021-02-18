/* libpbm1.c - pbm utility library part 1
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
#include <varargs.h>
#include "pbm.h"
#include "libpbm.h"

/* Variable-sized arrays. */

char *
pm_allocrow( cols, size )
int cols;
    {
    register char *itrow;

    itrow = (char *) malloc( cols * size );
    if ( itrow == (char *) 0 )
	pm_error( "out of memory allocating a row", 0,0,0,0,0 );
    return itrow;
    }

void
pm_freerow( itrow )
char *itrow;
    {
    free( itrow );
    }


char **
pm_allocarray( cols, rows, size )
int cols, rows;
int size;
    {
    char **its;
    int i;

    its = (char **) malloc( rows * sizeof(char *) );
    if ( its == (char **) 0 )
	pm_error( "out of memory allocating an array", 0,0,0,0,0 );
    its[0] = (char *) malloc( rows * cols * size );
    if ( its[0] == (char *) 0 )
	pm_error( "out of memory allocating an array", 0,0,0,0,0 );
    for ( i = 1; i < rows; i++ )
	its[i] = &(its[0][i * cols * size]);
    return its;
    }

void
pm_freearray( its, rows )
char **its;
int rows;
    {
    free( its[0] );
    free( its );
    }


/* Error handling. */

char *pm_progname;

/* I'd use varargs here, but it can't be done portably, because (a) vfprintf()
** is not very widespread, and (b) varargs itself is not powerful enough to
** allow me to include a portable vfprintf().
**
** So instead, we have the gross hack of a fixed number of args.
*/

void
pm_message( fmt, v1, v2, v3, v4, v5 )
char *fmt;
char *v1, *v2, *v3, *v4, *v5;
    {
    fprintf( stderr, "%s: ", pm_progname );
    fprintf( stderr, fmt, v1, v2, v3, v4, v5 );
    fputc( '\n', stderr );
    }

void
pm_error( fmt, v1, v2, v3, v4, v5 )
char *fmt;
char *v1, *v2, *v3, *v4, *v5;
    {
    pm_message( fmt, v1, v2, v3, v4, v5 );
    exit( 1 );
    }

void
pm_perror( reason )
char *reason;
    {
    extern char *sys_errlist[];
    extern int errno;
    char *e;

    if ( errno == 0 )
	e = "end of file";	/* not particularly kosher */
    else
	e = sys_errlist[errno];

    if ( reason != 0 && reason[0] != '\0' )
	pm_error( "%s - %s", reason, e, 0,0,0 );
    else
	pm_error( "%s", e, 0,0,0,0 );
    }

void
pm_usage( usage )
char *usage;
    {
    fprintf( stderr, "usage:  %s %s\n", pm_progname, usage );
    exit( 1 );
    }


/* File open/close that handles "-" as stdin and checks errors. */

FILE *
pm_openr( name )
char *name;
    {
    FILE *f;

    if ( strcmp( name, "-" ) == 0 )
	f = stdin;
    else
	{
	f = fopen( name, "r" );
	if ( f == NULL )
	    {
	    pm_perror( name );
	    exit( 1 );
	    }
	}
    return f;
    }

void
pm_close( f )
FILE *f;
    {
    if ( f != stdin )
	if ( fclose( f ) != 0 )
	    pm_perror( 0 );
    }

/* Broken putc() fix. */

#ifdef PBMPLUS_BROKENPUTC2
int
putc( c, stream )
char c;
FILE *stream;
    {
    return fwrite( &c, 1, 1, stream ) == 1 ? c : EOF;
    }
#endif /*PBMPLUS_BROKENPUTC2*/

/* Endian I/O.
*/

int
pm_readbigshort( in, sP )
FILE *in;
short *sP;
    {
    int c;

    if ( (c = getc( in )) == EOF )
	return -1;
    *sP = ( c & 0xff ) << 8;
    if ( (c = getc( in )) == EOF )
	return -1;
    *sP |= c & 0xff;
    return 0;
    }

int
pm_writebigshort( out, s )
FILE *out;
short s;
    {
    if ( putc( ( s >> 8 ) & 0xff, out ) == EOF )
	return -1;
    if ( putc( s & 0xff, out ) == EOF )
	return -1;
    return 0;
    }

int
pm_readbiglong( in, lP )
FILE *in;
long *lP;
    {
    int c;

    if ( (c = getc( in )) == EOF )
	return -1;
    *lP = ( c & 0xff ) << 24;
    if ( (c = getc( in )) == EOF )
	return -1;
    *lP |= ( c & 0xff ) << 16;
    if ( (c = getc( in )) == EOF )
	return -1;
    *lP |= ( c & 0xff ) << 8;
    if ( (c = getc( in )) == EOF )
	return -1;
    *lP |= c & 0xff;
    return 0;
    }

int
pm_writebiglong( out, l )
FILE *out;
long l;
    {
    if ( putc( ( l >> 24 ) & 0xff, out ) == EOF )
	return -1;
    if ( putc( ( l >> 16 ) & 0xff, out ) == EOF )
	return -1;
    if ( putc( ( l >> 8 ) & 0xff, out ) == EOF )
	return -1;
    if ( putc( l & 0xff, out ) == EOF )
	return -1;
    return 0;
    }

int
pm_readlittleshort( in, sP )
FILE *in;
short *sP;
    {
    int c;

    if ( (c = getc( in )) == EOF )
	return -1;
    *sP = c & 0xff;
    if ( (c = getc( in )) == EOF )
	return -1;
    *sP |= ( c & 0xff ) << 8;
    return 0;
    }

int
pm_writelittleshort( out, s )
FILE *out;
short s;
    {
    if ( putc( s & 0xff, out ) == EOF )
	return -1;
    if ( putc( ( s >> 8 ) & 0xff, out ) == EOF )
	return -1;
    return 0;
    }

int
pm_readlittlelong( in, lP )
FILE *in;
long *lP;
    {
    int c;

    if ( (c = getc( in )) == EOF )
	return -1;
    *lP = c & 0xff;
    if ( (c = getc( in )) == EOF )
	return -1;
    *lP |= ( c & 0xff ) << 8;
    if ( (c = getc( in )) == EOF )
	return -1;
    *lP |= ( c & 0xff ) << 16;
    if ( (c = getc( in )) == EOF )
	return -1;
    *lP |= ( c & 0xff ) << 24;
    return 0;
    }

int
pm_writelittlelong( out, l )
FILE *out;
long l;
    {
    if ( putc( l & 0xff, out ) == EOF )
	return -1;
    if ( putc( ( l >> 8 ) & 0xff, out ) == EOF )
	return -1;
    if ( putc( ( l >> 16 ) & 0xff, out ) == EOF )
	return -1;
    if ( putc( ( l >> 24 ) & 0xff, out ) == EOF )
	return -1;
    return 0;
    }
