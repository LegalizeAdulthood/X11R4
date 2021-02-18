/* pbmtog3.c - read a portable bitmap and produce a Group 3 FAX file
**
** Copyright (C) 1989 by Paul Haeberli <paul@manray.sgi.com>.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include <stdio.h>
#include "pbm.h"
#include "g3.h"
#ifdef SYSV
#include <string.h>
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define max(a,b) ((a) > (b) ? (a) : (b))

int reversebits;

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    bit *bitrow;
    int argn, rows, cols, format, row, col, i;
    char *usage = " [-reversebits] [pbmfile]";

    pm_progname = argv[0];

    argn = 1;
    reversebits = 0;

    if ( argn < argc  && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-reversebits",max(strlen(argv[argn]),2)) == 0 )
	    reversebits = 1;
	else
	    pm_usage( usage );
	argn++;
	}
	
    if ( argn == argc )
	ifd = stdin;
    else
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    
    if ( argn != argc )
	pm_usage( usage );

    pbm_readpbminit( ifd, &cols, &rows, &format );
    bitrow = pbm_allocrow( cols );

    /* Write out four extra rows to get things stabilized. */
    for ( col = 0; col < cols; col++ )
	bitrow[col] = PBM_WHITE;
    tofax( bitrow, cols );
    tofax( bitrow, cols );
    tofax( bitrow, cols );
    tofax( bitrow, cols );

    /* Write out bitmap. */
    putinit();
    for ( row = 0; row < rows; row++ )
	{
	pbm_readpbmrow( ifd, bitrow, cols, format );
	tofax( bitrow, cols );
	}

    /* And finish off. */
    for( i = 0; i < 6; i++ )
	puteol( );
    flushbits( );

    pm_close( ifd );

    exit( 0 );
    }

tofax(bitrow,n)
bit *bitrow;
int n;
{
    int c = 0;

    while(n>0) { 
	c = 0;
	while(*bitrow == PBM_WHITE && n>0) {
	    bitrow++;
	    c++;
	    n--;
	}
	putwhitespan(c);
	c = 0;
	if(n==0)
	    break;
	while(*bitrow == PBM_BLACK && n>0) {
	    bitrow++;
	    c++;
	    n--;
	}
	putblackspan(c);
    }
    puteol();
}

putwhitespan(c)
int c;
{
    int tpos;
    tableentry *te;

    if(c>=64) {
	tpos = (c/64)-1;
	te = mwtable+tpos;
	c -= te->count;
	putcode(te);
    }
    tpos = c;
    te = twtable+tpos;
    putcode(te);
}

putblackspan(c)
int c;
{
    int tpos;
    tableentry *te;

    if(c>=64) {
	tpos = (c/64)-1;
	te = mbtable+tpos;
	c -= te->count;
	putcode(te);
    }
    tpos = c;
    te = tbtable+tpos;
    putcode(te);
}

putcode(te)
tableentry *te;
{
    unsigned int mask;
    int code;

    mask = 1<<(te->length-1);
    code = te->code;
    while(mask) {
 	if(code&mask)
	    putbit(1);
	else
	    putbit(0);
	mask >>= 1;
    }

}

puteol()
{
    int i;

    for(i=0; i<11; i++)
	putbit(0);
    putbit(1);
}

int shdata;
int shbit = 0x80;

putinit()
{
    shdata = 0;
    shbit = reversebits ? 0x01 : 0x80;
}

int putbit(d)
int d;
{
    if(d) 
	shdata = shdata|shbit;
    if ( reversebits )
	shbit = shbit<<1;
    else
	shbit = shbit>>1;
    if((shbit&0xff) == 0) {
	putchar(shdata);
	shdata = 0;
	shbit = reversebits ? 0x01 : 0x80;
    }
}

flushbits( )
{
    if ( ( reversebits && shbit != 0x01 ) ||
	 ( ! reversebits && shbit != 0x80 ) ) {
	putchar(shdata);
	shdata = 0;
	shbit = reversebits ? 0x01 : 0x80;
    }
}
