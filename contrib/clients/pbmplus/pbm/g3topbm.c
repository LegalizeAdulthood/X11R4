/* g3topbm.c - read a Group 3 FAX file and produce a portable bitmap
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

#define TABSIZE(tab) (sizeof(tab)/sizeof(struct tableentry))
#define MAXCOLS 1728
#define MAXROWS 4300	/* up to two pages long */

int eof = 0;
int eols;
int rawzeros;
int shdata;
int kludge;
int reversebits;
int stretch;

#define WHASHA 3510
#define WHASHB 1178

#define BHASHA 293
#define BHASHB 2695

#define HASHSIZE 1021
tableentry *whash[HASHSIZE];
tableentry *bhash[HASHSIZE];

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    int argn, rows, cols, row, col, i;
    bit *bits[MAXROWS];
    char *usage = "[-kludge][-reversebits][-stretch] [g3file]";

    pm_progname = argv[0];

    argn = 1;
    kludge = 0;
    reversebits = 0;
    stretch = 0;

    /* Check for flags. */
    while ( argn < argc  && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-kludge",max(strlen(argv[argn]),2)) == 0 )
	    kludge = 1;
	else if ( strncmp(argv[argn],"-reversebits",max(strlen(argv[argn]),2)) == 0 )
	    reversebits = 1;
	else if ( strncmp(argv[argn],"-stretch",max(strlen(argv[argn]),2)) == 0 )
	    stretch = 1;
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

    rows = MAXROWS;
    cols = MAXCOLS;
    eols = 0;

    if ( kludge )
	{
	/* Skip extra lines to get in sync. */
	skiptoeol( ifd );
	skiptoeol( ifd );
	skiptoeol( ifd );
	}
    skiptoeol( ifd );
    for ( i = 0; i < HASHSIZE; ++i )
	whash[i] = bhash[i] = (tableentry *) 0;
    addtohash( whash, twtable, TABSIZE(twtable), WHASHA, WHASHB );
    addtohash( whash, mwtable, TABSIZE(mwtable), WHASHA, WHASHB );
    addtohash( whash, extable, TABSIZE(extable), WHASHA, WHASHB );
    addtohash( bhash, tbtable, TABSIZE(tbtable), BHASHA, BHASHB );
    addtohash( bhash, mbtable, TABSIZE(mbtable), BHASHA, BHASHB );
    addtohash( bhash, extable, TABSIZE(extable), BHASHA, BHASHB );

    cols = 0;
    for ( rows = 0; rows < MAXROWS; ++rows )
	{
	bits[rows] = pbm_allocrow( MAXCOLS );
	col = getfaxrow( ifd, rows, bits[rows] );
	if ( eof )
	    break;
	if ( col > cols )
	    cols = col;
	if ( stretch )
	    {
	    bits[rows + 1] = bits[rows];
	    ++rows;
	    }
	}

    pm_close( ifd );

    pbm_writepbminit( stdout, cols, rows );
    for ( row = 0; row < rows; ++row )
	pbm_writepbmrow( stdout, bits[row], cols );

    exit(0);
    }

addtohash(hash, te, n, a, b)
	tableentry *hash[];
	tableentry *te;
	int n, a, b;
{
	unsigned int pos;

	while (n--) {
		pos = ((te->length+a)*(te->code+b))%HASHSIZE;
		if (hash[pos] != 0)
			pm_error(
			    "internal error: addtohash fatal hash collision",
			    0,0,0,0,0 );
		hash[pos] = te;
		te++;
	}
}

tableentry *
hashfind(hash, length, code, a, b)
	tableentry *hash[];
	int length, code;
	int a, b;
{
	unsigned int pos;
	tableentry *te;

	pos = ((length+a)*(code+b))%HASHSIZE;
	if (pos < 0 || pos >= HASHSIZE)
		pm_error(
		  "internal error: bad hash position, length %d code %d pos %d",
		    length, code, pos, 0,0 );
	te = hash[pos];
	return ((te && te->length == length && te->code == code) ? te : 0);
}

getfaxrow( inf, row, bitrow )
    FILE *inf;
    int row;
    bit *bitrow;
{
	int col;
	bit *bP;
	int curlen, curcode, nextbit;
	int count, color;
	tableentry *te;

	for ( col = 0, bP = bitrow; col < MAXCOLS; ++col, ++bP )
	    *bP = PBM_WHITE;
	col = 0;
	rawzeros = 0;
	curlen = 0;
	curcode = 0;
	color = 1;
	count = 0;
	while (!eof) {
		if (col >= MAXCOLS) {
			skiptoeol(inf);
			return (col); 
		}
		do {
			if (rawzeros >= 11) {
				nextbit = rawgetbit(inf);
				if (nextbit) {
					if (col == 0)
						/* XXX should be 6 */
						eof = (++eols == 3);
					else
						eols = 0;
#ifdef notdef
					if (col && col < 1728)
						pm_message(
					       "warning, row %d short (len %d)",
						    row, col, 0,0,0 );
#endif /*notdef*/
					return (col); 
				}
			} else
				nextbit = rawgetbit(inf);
			curcode = (curcode<<1) + nextbit;
			curlen++;
		} while (curcode <= 0);
		if (curlen > 13) {
			pm_message(
	  "bad code word at row %d, col %d (len %d code 0x%x), skipping to EOL",
			    row, col, curlen, curcode, 0 );
			skiptoeol(inf);
			return (col);
		}
		if (color) {
			if (curlen < 4)
				continue;
			te = hashfind(whash, curlen, curcode, WHASHA, WHASHB);
		} else {
			if (curlen < 2)
				continue;
			te = hashfind(bhash, curlen, curcode, BHASHA, BHASHB);
		}
		if (!te)
			continue;
		switch (te->tabid) {
		case TWTABLE:
		case TBTABLE:
			count += te->count;
			if (col+count > MAXCOLS) 
				count = MAXCOLS-col;
			if (count > 0) {
				if (color) {
					col += count;
					count = 0;
				} else {
					for ( ; count > 0; --count, ++col )
						bitrow[col] = PBM_BLACK;
				}
			}
			curcode = 0;
			curlen = 0;
			color = !color;
			break;
		case MWTABLE:
		case MBTABLE:
			count += te->count;
			curcode = 0;
			curlen = 0;
			break;
		case EXTABLE:
			count += te->count;
			curcode = 0;
			curlen = 0;
			break;
		default:
			pm_error( "internal bad poop", 0,0,0,0,0 );
		}
	}
	return (0);
}

skiptoeol(file)
FILE *file;
{
    int b;

    while (rawzeros<11)
	b = rawgetbit(file);
    while(1) {
	if(rawgetbit(file))
	    break;
    }
}

int shbit = 0;

int rawgetbit( file )
    FILE *file;
    {
    int b;

    if ( ( shbit & 0xff ) == 0 )
	{
	shdata = getc( file );
	if ( shdata == EOF )
	    pm_error( "premature EOF at line %d", eols );
	shbit = reversebits ? 0x01 : 0x80;
	}
    if ( shdata & shbit )
	{
	rawzeros = 0;
	b = 1;
	}
    else
	{
	rawzeros++;
	b = 0;
	}
    if ( reversebits )
	shbit <<= 1;
    else
	shbit >>= 1;
    return b;
    }
