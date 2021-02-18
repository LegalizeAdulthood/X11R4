/* picttopbm.c - read a PICT file and produce a portable bitmap
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
#include "pbm.h"
#include "pict.h"
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
    int argn, picsize, bytesRead, oldBytesRead, endOfPicture, gotBits, opcode;
    int x1, y1, x2, y2;
    int rows, cols, row, col;
    int extraskip, version;
    bit **bits, *bP;
    char *usage = "[-extraskip <n>] [pictfile]";

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

    /* Skip blank header. */
    (void) skipBytes( ifd, extraskip );
    (void) skipBytes( ifd, HEADER_SIZE );

    /* Read size. */
    bytesRead = getBigShort( ifd, &picsize );

    /* Read picFrame. */
    bytesRead += getRect( ifd, &x1, &y1, &x2, &y2 );
    rows = y2 - y1;
    cols = x2 - x1;
    bits = pbm_allocarray( cols, rows );
    for ( row = 0; row < rows; row++ )
	for ( col = 0, bP = bits[row]; col < cols; col++, bP++ )
	    *bP = PBM_WHITE;

    endOfPicture = 0;
    gotBits = 0;
    version = 1;

    while ( ! endOfPicture )
	{
	switch ( version )
	    {
	    case 1:
	    bytesRead += getByte( ifd, &opcode );
	    break;

	    case 2:
	    bytesRead += getBigShort( ifd, &opcode );
	    break;

	    default:
	    pm_error( "unknown version - can't happen", 0,0,0,0,0 );
	    }

	oldBytesRead = bytesRead;
	switch ( opcode )
	    {
	    /* This group of opcodes is handled correctly. */

	    case PICT_NOP:
	    break;

	    case PICT_picVersion:
	    bytesRead += getByte( ifd, &version );
	    if ( version != 1 && version != 2 )
		pm_error( "unknown PICT version - %d", version, 0,0,0,0 );
	    break;

	    case PICT_shortComment:
	    bytesRead += skipBytes( ifd, 2 );
	    break;

	    case PICT_longComment:
	    {
	    int size;
	    bytesRead += skipBytes( ifd, 2 );
	    bytesRead += getBigShort( ifd, &size );
	    bytesRead += skipBytes( ifd, size );
	    break;
	    }

	    case PICT_BitsRect:
	    case PICT_BitsRgn:
	    case PICT_PackBitsRect:
	    case PICT_PackBitsRgn:
	    bytesRead += getBits( ifd, opcode, bits, rows, cols );
	    gotBits = 1;
	    break;

	    case PICT_EndOfPicture:
	    endOfPicture = 1;
	    break;

	    /* This group of opcodes is skipped correctly. */

	    case PICT_clipRgn:
	    pm_message( "clipRgn - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 10 );
	    break;

	    case PICT_bkPat:
	    pm_message( "bkPat - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_txFace:
	    pm_message( "txFace - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 1 );
	    break;

	    case PICT_pnSize:
	    pm_message( "pnPat - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 4 );
	    break;


	    case PICT_pnPat:
	    pm_message( "pnPat - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_thePat:
	    pm_message( "thePat - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_origin:
	    pm_message( "origin - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 4 );
	    break;

	    case PICT_frameRect:
	    pm_message( "frameRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_paintRect:
	    pm_message( "paintRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_eraseRect:
	    pm_message( "eraseRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_invertRect:
	    pm_message( "invertRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_fillRect:
	    pm_message( "fillRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_frameSameRect:
	    pm_message( "frameSameRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_paintSameRect:
	    pm_message( "paintSameRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_eraseSameRect:
	    pm_message( "eraseSameRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_invertSameRect:
	    pm_message( "invertSameRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_fillSameRect:
	    pm_message( "fillSameRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_frameRRect:
	    pm_message( "frameRRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_paintRRect:
	    pm_message( "paintRRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_eraseRRect:
	    pm_message( "eraseRRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_invertRRect:
	    pm_message( "invertRRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_fillRRect:
	    pm_message( "fillRRect - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 8 );
	    break;

	    case PICT_frameSameRRect:
	    pm_message( "frameSameRRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_paintSameRRect:
	    pm_message( "paintSameRRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_eraseSameRRect:
	    pm_message( "eraseSameRRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_invertSameRRect:
	    pm_message( "invertSameRRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_fillSameRRect:
	    pm_message( "fillSameRRect - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_frameRgn:
	    pm_message( "frameRgn - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 10 );
	    break;

	    case PICT_paintRgn:
	    pm_message( "paintRgn - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 10 );
	    break;

	    case PICT_eraseRgn:
	    pm_message( "eraseRgn - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 10 );
	    break;

	    case PICT_invertRgn:
	    pm_message( "invertRgn - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 10 );
	    break;

	    case PICT_fillRgn:
	    pm_message( "fillRgn - skipping", 0,0,0,0,0 );
	    bytesRead += skipBytes( ifd, 10 );
	    break;

	    case PICT_frameSameRgn:
	    pm_message( "frameSameRgn - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_paintSameRgn:
	    pm_message( "paintSameRgn - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_eraseSameRgn:
	    pm_message( "eraseSameRgn - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_invertSameRgn:
	    pm_message( "invertSameRgn - skipping", 0,0,0,0,0 );
	    break;

	    case PICT_fillSameRgn:
	    pm_message( "fillSameRgn - skipping", 0,0,0,0,0 );
	    break;


	    /* And this group of opcodes is of unknown length, and can't
	    ** be handled or skipped correctly. */

	    case PICT_txFont:
	    pm_error( "txFont - can't handle", 0,0,0,0,0 );
	    case PICT_txMode:
	    pm_error( "txMode - can't handle", 0,0,0,0,0 );
	    case PICT_spExtra:
	    pm_error( "spExtra - can't handle", 0,0,0,0,0 );
	    case PICT_pnMode:
	    pm_error( "pnMode - can't handle", 0,0,0,0,0 );
	    case PICT_ovSize:
	    pm_error( "ovSize - can't handle", 0,0,0,0,0 );
	    case PICT_txSize:
	    pm_error( "txSize - can't handle", 0,0,0,0,0 );
	    case PICT_fgColor:
	    pm_error( "fgColor - can't handle", 0,0,0,0,0 );
	    case PICT_bkColor:
	    pm_error( "bkColor - can't handle", 0,0,0,0,0 );
	    case PICT_txRatio:
	    pm_error( "txRatio - can't handle", 0,0,0,0,0 );
	    case PICT_line:
	    pm_error( "line - can't handle", 0,0,0,0,0 );
	    case PICT_line_from:
	    pm_error( "from - can't handle", 0,0,0,0,0 );
	    case PICT_short_line:
	    pm_error( "line - can't handle", 0,0,0,0,0 );
	    case PICT_short_line_from:
	    pm_error( "from - can't handle", 0,0,0,0,0 );
	    case PICT_long_text:
	    pm_error( "text - can't handle", 0,0,0,0,0 );
	    case PICT_DH_text:
	    pm_error( "text - can't handle", 0,0,0,0,0 );
	    case PICT_DV_text:
	    pm_error( "text - can't handle", 0,0,0,0,0 );
	    case PICT_DHDV_text:
	    pm_error( "text - can't handle", 0,0,0,0,0 );
	    case PICT_frameOval:
	    pm_error( "frameOval - can't handle", 0,0,0,0,0 );
	    case PICT_paintOval:
	    pm_error( "paintOval - can't handle", 0,0,0,0,0 );
	    case PICT_eraseOval:
	    pm_error( "eraseOval - can't handle", 0,0,0,0,0 );
	    case PICT_invertOval:
	    pm_error( "invertOval - can't handle", 0,0,0,0,0 );
	    case PICT_fillOval:
	    pm_error( "fillOval - can't handle", 0,0,0,0,0 );
	    case PICT_frameSameOval:
	    pm_error( "frameSameOval - can't handle", 0,0,0,0,0 );
	    case PICT_paintSameOval:
	    pm_error( "paintSameOval - can't handle", 0,0,0,0,0 );
	    case PICT_eraseSameOval:
	    pm_error( "eraseSameOval - can't handle", 0,0,0,0,0 );
	    case PICT_invertSameOval:
	    pm_error( "invertSameOval - can't handle", 0,0,0,0,0 );
	    case PICT_fillSameOval:
	    pm_error( "fillSameOval - can't handle", 0,0,0,0,0 );
	    case PICT_frameArc:
	    pm_error( "frameArc - can't handle", 0,0,0,0,0 );
	    case PICT_paintArc:
	    pm_error( "paintArc - can't handle", 0,0,0,0,0 );
	    case PICT_eraseArc:
	    pm_error( "eraseArc - can't handle", 0,0,0,0,0 );
	    case PICT_invertArc:
	    pm_error( "invertArc - can't handle", 0,0,0,0,0 );
	    case PICT_fillArc:
	    pm_error( "fillArc - can't handle", 0,0,0,0,0 );
	    case PICT_frameSameArc:
	    pm_error( "frameSameArc - can't handle", 0,0,0,0,0 );
	    case PICT_paintSameArc:
	    pm_error( "paintSameArc - can't handle", 0,0,0,0,0 );
	    case PICT_eraseSameArc:
	    pm_error( "eraseSameArc - can't handle", 0,0,0,0,0 );
	    case PICT_invertSameArc:
	    pm_error( "invertSameArc - can't handle", 0,0,0,0,0 );
	    case PICT_fillSameArc:
	    pm_error( "fillSameArc - can't handle", 0,0,0,0,0 );
	    case PICT_framePoly:
	    pm_error( "framePoly - can't handle", 0,0,0,0,0 );
	    case PICT_paintPoly:
	    pm_error( "paintPoly - can't handle", 0,0,0,0,0 );
	    case PICT_erasePoly:
	    pm_error( "erasePoly - can't handle", 0,0,0,0,0 );
	    case PICT_invertPoly:
	    pm_error( "invertPoly - can't handle", 0,0,0,0,0 );
	    case PICT_fillPoly:
	    pm_error( "fillPoly - can't handle", 0,0,0,0,0 );
	    case PICT_frameSamePoly:
	    pm_error( "frameSamePoly - can't handle", 0,0,0,0,0 );
	    case PICT_paintSamePoly:
	    pm_error( "paintSamePoly - can't handle", 0,0,0,0,0 );
	    case PICT_eraseSamePoly:
	    pm_error( "eraseSamePoly - can't handle", 0,0,0,0,0 );
	    case PICT_invertSamePoly:
	    pm_error( "invertSamePoly - can't handle", 0,0,0,0,0 );
	    case PICT_fillSamePoly:
	    pm_error( "fillSamePoly - can't handle", 0,0,0,0,0 );

	    default:
	    if ( version == 2 )
		pm_error(
		    "unknown PICT2 opcode - 0x%04x - can't handle", opcode,
		    0,0,0,0 );
	    else
		pm_error(
		    "unknown PICT opcode - 0x%02x - can't handle", opcode,
		    0,0,0,0 );
	    }

	if ( version == 2 && ( bytesRead - oldBytesRead ) & 1 )
	    bytesRead += skipBytes( ifd, 1 );
	}

    pm_close( ifd );

    if ( ! gotBits )
	pm_error( "no bits rectangles seen", 0,0,0,0,0 );
    if ( bytesRead % 65536L != picsize )
	pm_message(
	    "bytes read (%d) mod 65536 != piczise (%d) - warning only",
	    bytesRead, picsize, 0,0,0 );

    pbm_writepbm( stdout, bits, cols, rows );

    exit( 0 );
    }

getBits( fd, opcode, bits, rows, cols )
FILE *fd;
int opcode, rows, cols;
bit **bits;
    {
    int rowBytes, mode;
    int bnd_x1, bnd_y1, bnd_x2, bnd_y2;
    int src_x1, src_y1, src_x2, src_y2;
    int dst_x1, dst_y1, dst_x2, dst_y2;
    int padRight, b, br, row;
    register int col;
    register bit *bP;

    br = getBigShort( fd, &rowBytes );

    br += getRect( fd, &bnd_x1, &bnd_y1, &bnd_x2, &bnd_y2 );
    br += getRect( fd, &src_x1, &src_y1, &src_x2, &src_y2 );
    br += getRect( fd, &dst_x1, &dst_y1, &dst_x2, &dst_y2 );

    br += getBigShort( fd, &mode );
    if ( mode != 1 )
	pm_message( "bits rectangle mode %d != 1 -- ignoring", mode, 0,0,0,0 );

    if ( opcode == PICT_BitsRgn || opcode == PICT_PackBitsRgn )
	br += skipBytes( fd, 10 );

    padRight = rowBytes * 8 - ( bnd_x2 - bnd_x1 );

    getInit( opcode );

    for ( row = bnd_y1; row < bnd_y2; row++ )
	{
	br += getInitRow( fd );

	if ( row >= 0 && row < rows )
	    {
	    for ( col = bnd_x1, bP = &(bits[row][bnd_x1]);
		  col < bnd_x2; col++, bP++ )
		{
		br += getBit( fd, &b );
		if ( col >= 0 && col < cols )
		    *bP = b ? PBM_BLACK : PBM_WHITE;
		}
	    for ( col = 0; col < padRight; col++ )
		br += getBit( fd, &b );
	    }
	else
	    {
	    for ( col = bnd_x1; col < bnd_x2; col++ )
		br += getBit( fd, &b );
	    for ( col = 0; col < padRight; col++ )
		br += getBit( fd, &b );
	    }
	}

    return br;
    }

getByte( fd, cP )
FILE *fd;
int *cP;
    {
    *cP = getc( fd );
    if ( *cP == EOF )
	pm_error( "premature EOF", 0,0,0,0 );
    return 1;
    }

skipBytes( fd, n )
FILE *fd;
int n;
    {
    int i, j;

    for ( i = 0; i < n; i++ )
	(void) getByte( fd, &j );
    return n;
    }

getBigShort( fd, sP )
FILE *fd;
int *sP;
    {
    int br, h, l;

    br = getByte( fd, &h );
    br += getByte( fd, &l );
    *sP = ( h << 8 ) + l;
    return br;
    }

getBigLong( fd, lP )
FILE *fd;
long *lP;
    {
    int br, b1, b2, b3, b4;

    br = getByte( fd, &b1 );
    br += getByte( fd, &b2 );
    br += getByte( fd, &b3 );
    br += getByte( fd, &b4 );
    *lP = ( b1 << 24 ) + ( b2 << 16 ) + ( b3 << 8 ) + b4;
    return br;
    }

getRect( fd, x1P, y1P, x2P, y2P )
FILE *fd;
int *x1P, *y1P, *x2P, *y2P;
    {
    int br;

    br = getBigShort( fd, y1P );
    br += getBigShort( fd, x1P );
    br += getBigShort( fd, y2P );
    br += getBigShort( fd, x2P );
    return br;
    }

int packed;
int count, repeat, item, bitshift;

getInit( opcode )
int opcode;
    {
    packed = ( opcode == PICT_PackBitsRect || opcode == PICT_PackBitsRgn );
    bitshift = -1;
    count = 0;
    return 0;
    }

getInitRow( fd )
FILE *fd;
    {
    int br;

    br = 0;
    if ( packed )
	br += skipBytes( fd, 1 );	/* skip junk byte */
    return br;
    }

getBit( fd, iP )
FILE *fd;
int *iP;
    {
    int br;

    br = 0;
    if ( bitshift < 0 )
	{
	if ( packed )
	    {
	    if ( count <= 0 )
		{
		br += getByte( fd, &count );
		if ( count < 128 )
		    {
		    repeat = 0;
		    br += getByte( fd, &item );
		    }
		else
		    {
		    repeat = 1;
		    br += getByte( fd, &item );
		    count = 256 - count;
		    }
		}
	    else
		{
		if ( ! repeat )
		    {
		    br += getByte( fd, &item );
		    }
		count--;
		}
	    }
	else
	    br += getByte( fd, &item );
	bitshift = 7;
	}
    *iP = ( item >> bitshift-- ) & 1;
    return br;
    }
