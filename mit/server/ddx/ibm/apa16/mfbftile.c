/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/***********************************************************
		Copyright IBM Corporation 1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: mfbftile.c,v 1.1 89/04/19 17:15:28 mckie Exp $ */
/* $Source: /space/X11/server/ddx/ibm/apa16/RCS/mfbftile.c,v $ */

#ifndef lint
static char *rcsid = "$Header: mfbftile.c,v 1.1 89/04/19 17:15:28 mckie Exp $";
static char sccsid[] = "@(#)mfbftile.c	3.1 88/09/22 09:31:26";
#endif

#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "mfb.h"
#include "maskbits.h"

/* 
   this code could be called by the paint window background stuff,
too; there would be some speed hit because of the different
parameters and the need to check for a rop when filling
with a tile.

   the boxes are already translated.

   NOTE:
   iy = ++iy < tileHeight ? iy : 0
is equivalent to iy%= tileheight, and saves a division.
*/

static void
fpclear( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
    for ( bitmask = ~bitmask ; h-- ; dst += djump )
	*dst &= bitmask ;
return ;
}

static void
fpand( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{

    for ( bitmask = ~bitmask ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	*dst &= ( *src | bitmask ) ;
    }
return ;
}

static void
fpandr( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int orig_dbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	orig_dbits = *dst ;
	*dst = ( *src & ~ orig_dbits & bitmask ) | ( ~bitmask & orig_dbits ) ;
    }
return ;
}

static void
fpcopy( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	*dst = ( *src & bitmask ) | ( ~bitmask & *dst ) ;
    }
return ;
}

static void
fpandi( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int orig_dbits ;
    for ( bitmask = ~bitmask ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	orig_dbits = *dst ;
	*dst = ( bitmask & orig_dbits )
	     | ( ( bitmask | ~ *src ) & ~ orig_dbits ) ;
    }
return ;
}

static void
fpxor( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	*dst ^= ( bitmask & *src ) ;
    }
return ;
}

static void
fpor( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	*dst |= ( *src & bitmask ) ;
    }
return ;
}

static void
fpnor( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int orig_dbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	orig_dbits = *dst ;
	*dst = ( orig_dbits & ~bitmask ) | ~( bitmask & ( *src | orig_dbits ) );
    }
return ;
}

static void
fpxnor( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	*dst ^= ( bitmask & ~ *src ) ;
    }
return ;
}

static void
fpinv( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{

    for ( ; h-- ; dst += djump )
    {
	*dst ^= bitmask ;
    }
return ;
}

static void
fporr( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int orig_dbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	orig_dbits = *dst ;
	*dst = ( orig_dbits & ~bitmask ) | ( ( *src | orig_dbits ) & bitmask ) ;
    }
return ;
}

static void
fpcopyr( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	*dst = ( bitmask & ~ *src ) | ( *dst & ~bitmask ) ;
    }
return ;
}

static void
fpori( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	*dst |= ( bitmask & ~ *src ) ;
    }
return ;
}

static void
fpnand( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int orig_dbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	orig_dbits = *dst ;
	*dst = ( orig_dbits & ~bitmask ) | ~( bitmask & ( *src & orig_dbits ) );
    }
return ;
}

static void
fpset( src, dst, h, bitmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register unsigned long int bitmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
    for ( ; h-- ; dst += djump )
	*dst |= bitmask ;
return ;
}

static void fNoop()
{
return ;
}

void (*mfb_fp_tile_funcs[])() = {
	fpclear,	/* GXclear:		0x0 Zero 0 */
	fpand,		/* GXand:		0x1 src AND dst */
	fpandr,		/* GXandReverse:	0x2 src AND NOT dst */
	fpcopy,		/* GXcopy:		0x3 src */
	fpandi,		/* GXandInverted:	0x4 NOT src AND dst */
	fNoop,		/* GXnoop:		0x5 dst */
	fpxor,		/* GXxor:		0x6 src XOR dst */
	fpor,		/* GXor:		0x7 src OR dst */
	fpnor,		/* GXnor:		0x8 NOT src AND NOT dst */
	fpxnor,		/* GXequiv:		0x9 NOT src XOR dst */
	fpinv,		/* GXinvert:		0xa NOT dst */
	fporr,		/* GXorReverse:		0xb src OR NOT dst */
	fpcopyr,	/* GXcopyInverted:	0xc NOT src */
	fpori,		/* GXorInverted:	0xd NOT src OR dst */
	fpnand,		/* GXnand:		0xe NOT src OR NOT dst */
	fpset		/* GXset:		0xf 1 */
} ;

static void
fsclear( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register int midcnt ;
    for ( startmask = ~startmask ; h-- ; dst += djump ) {
	*dst++ &= startmask ;
	for ( midcnt = centerWidth ; midcnt-- ; dst++ )
	    *dst = 0 ;
    }
return ;
}

static void
fsand( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register int midcnt ;
register unsigned long int srcbits ;
    for ( startmask = ~startmask ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	*dst++ &= ( startmask | ( srcbits = *src ) ) ;
	for ( midcnt = centerWidth ; midcnt-- ; dst++ )
	    *dst &= srcbits ;
    }
return ;
}

static void
fsandr( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( startmask = ~startmask ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	temp = *dst ;
	srcbits = *src ;
	*dst++ = ( startmask & srcbits & ~ temp ) | ( temp & ~startmask ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits & ~*dst ;
    }
return ;
}

static void
fscopy( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	temp = *dst ;
	srcbits = *src ;
	*dst++ = ( srcbits & startmask ) | ( ~startmask & temp ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits ;
    }
return ;
}

static void
fsandi( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( startmask = ~startmask ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	temp = *dst ;
	srcbits = *src ;
	*dst++ = temp & ( startmask | ~ ( startmask | srcbits ) ) ;
	for ( srcbits = ~srcbits, temp = centerWidth ; temp-- ; dst++ )
	    *dst &= srcbits ;
    }
return ;
}

static void
fsxor( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	*dst ^= ( startmask & srcbits ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst ^= srcbits ;
    }
return ;
}

static void
fsor( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	*dst++ |= ( srcbits & startmask ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst |= srcbits ;
    }
return ;
}

static void
fsnor( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	temp = *dst ;
	srcbits = *src ;
	*dst++ = ( temp & ~startmask ) | ~( startmask & ( srcbits | temp ) ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ ( srcbits | *dst ) ;
    }
return ;
}

static void
fsxnor( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	*dst++ ^= ( startmask & ~ *src ) ;
	for ( srcbits = ~srcbits, temp = centerWidth ; temp-- ; dst++ )
	    *dst ^= srcbits ;
    }
return ;
}

static void
fsinv( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;

    for ( ; h-- ; dst += djump )
    {
	*dst++ ^= startmask ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ *dst ;
    }
return ;
}

static void
fsorr( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	temp = *dst ;
	srcbits = *src ;
	*dst++ = ( temp & ~startmask ) | ( ( srcbits | temp ) & startmask ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits | ~ *dst ;
    }
return ;
}

static void
fscopyr( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	temp = *dst ;
	*dst++ = ( startmask & srcbits ) | ( temp & ~startmask ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits ;
    }
return ;
}

static void
fsori( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	*dst++ |= ( startmask & srcbits ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst |= srcbits ;
    }
return ;
}

static void
fsnand( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	temp = *dst ;
	srcbits = *src ;
	*dst++ = ( temp & ~startmask ) | ~( startmask & ( srcbits & temp ) );
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ ( srcbits & *dst ) ;
    }
return ;
}

static void
fsset( src, dst, h, centerWidth, startmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;

    for ( ; h-- ; dst += djump ) {
	*dst++ |= startmask ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ 0 ;
    }
return ;
}

void (*mfb_fs_tile_funcs[])() = {
	fsclear,	/* GXclear:		0x0 Zero 0 */
	fsand,		/* GXand:		0x1 src AND dst */
	fsandr,		/* GXandReverse:	0x2 src AND NOT dst */
	fscopy,		/* GXcopy:		0x3 src */
	fsandi,		/* GXandInverted:	0x4 NOT src AND dst */
	fNoop,		/* GXnoop:		0x5 dst */
	fsxor,		/* GXxor:		0x6 src XOR dst */
	fsor,		/* GXor:		0x7 src OR dst */
	fsnor,		/* GXnor:		0x8 NOT src AND NOT dst */
	fsxnor,		/* GXequiv:		0x9 NOT src XOR dst */
	fsinv,		/* GXinvert:		0xa NOT dst */
	fsorr,		/* GXorReverse:		0xb src OR NOT dst */
	fscopyr,	/* GXcopyInverted:	0xc NOT src */
	fsori,		/* GXorInverted:	0xd NOT src OR dst */
	fsnand,		/* GXnand:		0xe NOT src OR NOT dst */
	fsset		/* GXset:		0xf 1 */
} ;

static void
feclear( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register int midcnt ;
    for ( endmask = ~endmask ; h-- ; dst += djump ) {
	for ( midcnt = centerWidth ; midcnt-- ; dst++ )
	    *dst = 0 ;
	*dst++ &= endmask ;
    }
return ;
}

static void
feand( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register int midcnt ;
register unsigned long int srcbits ;
    for ( endmask = ~endmask ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( midcnt = centerWidth ; midcnt-- ; dst++ )
	    *dst &= srcbits ;
	*dst++ &= ( endmask | srcbits ) ;
    }
return ;
}

static void
feandr( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( endmask = ~endmask ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits & ~*dst ;
	temp = *dst ;
	*dst++ = ( endmask & srcbits & ~ temp ) | ( temp & ~endmask ) ;
    }
return ;
}

static void
fecopy( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits ;
	temp = *dst ;
	*dst++ = ( srcbits & endmask ) | ( ~endmask & temp ) ;
    }
return ;
}

static void
feandi( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( endmask = ~endmask ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( srcbits = ~srcbits, temp = centerWidth ; temp-- ; dst++ )
	    *dst &= srcbits ;
	temp = *dst ;
	*dst++ = temp & ( endmask | ~ ( endmask | srcbits ) ) ;
    }
return ;
}

static void
fexor( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst ^= srcbits ;
	*dst ^= ( endmask & srcbits ) ;
    }
return ;
}

static void
feor( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst |= srcbits ;
	*dst++ |= ( srcbits & endmask ) ;
    }
return ;
}

static void
fenor( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ ( srcbits | *dst ) ;
	temp = *dst ;
	*dst++ = ( temp & ~endmask ) | ~( endmask & ( srcbits | temp ) ) ;
    }
return ;
}

static void
fexnor( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( srcbits = ~srcbits, temp = centerWidth ; temp-- ; dst++ )
	    *dst ^= srcbits ;
	*dst++ ^= ( endmask & ~ *src ) ;
    }
return ;
}

static void
feinv( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;

    for ( ; h-- ; dst += djump )
    {
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ *dst ;
	*dst++ ^= endmask ;
    }
return ;
}

static void
feorr( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits | ~ *dst ;
	temp = *dst ;
	*dst++ = ( temp & ~endmask ) | ( ( srcbits | temp ) & endmask ) ;
    }
return ;
}

static void
fecopyr( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits ;
	temp = *dst ;
	*dst++ = ( endmask & srcbits ) | ( temp & ~endmask ) ;
    }
return ;
}

static void
feori( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst |= srcbits ;
	*dst++ |= ( endmask & srcbits ) ;
    }
return ;
}

static void
fenand( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ ( srcbits & *dst ) ;
	temp = *dst ;
	*dst++ = ( temp & ~endmask ) | ~( endmask & ( srcbits & temp ) );
    }
return ;
}

static void
feset( src, dst, h, centerWidth, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;

    for ( ; h-- ; dst += djump ) {
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ 0 ;
	*dst++ |= endmask ;
    }
return ;
}
void (*mfb_fe_tile_funcs[])() = {
	feclear,	/* GXclear:		0x0 Zero 0 */
	feand,		/* GXand:		0x1 src AND dst */
	feandr,		/* GXandReverse:	0x2 src AND NOT dst */
	fecopy,		/* GXcopy:		0x3 src */
	feandi,		/* GXandInverted:	0x4 NOT src AND dst */
	fNoop,		/* GXnoop:		0x5 dst */
	fexor,		/* GXxor:		0x6 src XOR dst */
	feor,		/* GXor:		0x7 src OR dst */
	fenor,		/* GXnor:		0x8 NOT src AND NOT dst */
	fexnor,		/* GXequiv:		0x9 NOT src XOR dst */
	feinv,		/* GXinvert:		0xa NOT dst */
	feorr,		/* GXorReverse:		0xb src OR NOT dst */
	fecopyr,	/* GXcopyInverted:	0xc NOT src */
	feori,		/* GXorInverted:	0xd NOT src OR dst */
	fenand,		/* GXnand:		0xe NOT src OR NOT dst */
	feset		/* GXset:		0xf 1 */
} ;

static void
fbclear( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register int midcnt ;
    for ( startmask = ~startmask, endmask = ~endmask ; h-- ; dst += djump ) {
	*dst++ &= startmask ;
	for ( midcnt = centerWidth ; midcnt-- ; dst++ )
	    *dst = 0 ;
	*dst++ &= endmask ;
    }
return ;
}

static void
fband( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int endmask ;
register unsigned long int startmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register int midcnt ;
register unsigned long int srcbits ;

    for ( startmask = ~startmask, endmask = ~endmask ; h-- ; dst += djump ) {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	*dst++ &= ( startmask | srcbits ) ;
	for ( midcnt = centerWidth ; midcnt-- ; dst++ )
	    *dst &= srcbits ;
	*dst++ &= ( endmask | srcbits ) ;
    }
return ;
}

static void
fbandr( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( startmask = ~startmask, endmask = ~endmask ; h-- ; dst += djump ) {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	temp = *dst ;
	*dst++ = ( startmask & srcbits & ~ temp ) | ( temp & ~startmask ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits & ~*dst ;
	temp = *dst ;
	*dst++ = ( endmask & srcbits & ~ temp ) | ( temp & ~endmask ) ;
    }
return ;
}

static void
fbcopy( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	temp = *dst ;
	*dst++ = ( srcbits & startmask ) | ( ~startmask & temp ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits ;
	temp = *dst ;
	*dst++ = ( srcbits & endmask ) | ( ~endmask & temp ) ;
    }
return ;
}

static void
fbandi( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( startmask = ~startmask, endmask = ~endmask ; h-- ; dst += djump ) {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	temp = *dst ;
	*dst++ = temp & ( startmask | ~ ( startmask | srcbits ) ) ;
	for ( srcbits = ~srcbits, temp = centerWidth ; temp-- ; dst++ )
	    *dst &= srcbits ;
	temp = *dst ;
	*dst++ = temp & ( endmask | ~ ( endmask | srcbits ) ) ;
    }
return ;
}

static void
fbxor( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	*dst ^= ( startmask & srcbits ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst ^= srcbits ;
	*dst ^= ( endmask & srcbits ) ;
    }
return ;
}

static void
fbor( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	*dst++ |= ( srcbits & startmask ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst |= srcbits ;
	*dst++ |= ( srcbits & endmask ) ;
    }
return ;
}

static void
fbnor( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	temp = *dst ;
	*dst++ = ( temp & ~startmask ) | ~( startmask & ( srcbits | temp ) ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ ( srcbits | *dst ) ;
	temp = *dst ;
	*dst++ = ( temp & ~endmask ) | ~( endmask & ( srcbits | temp ) ) ;
    }
return ;
}

static void
fbxnor( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	*dst++ ^= ( startmask & srcbits ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst ^= srcbits ;
	*dst++ ^= ( endmask & srcbits ) ;
    }
return ;
}

static void
fbinv( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;

    for ( ; h-- ; dst += djump )
    {
	*dst++ ^= startmask ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ *dst ;
	*dst++ ^= endmask ;
    }
return ;
}

static void
fborr( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	temp = *dst ;
	*dst++ = ( temp & ~startmask ) | ( ( srcbits | temp ) & startmask ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits | ~ *dst ;
	temp = *dst ;
	*dst++ = ( temp & ~endmask ) | ( ( srcbits | temp ) & endmask ) ;
    }
return ;
}

static void
fbcopyr( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	temp = *dst ;
	*dst++ = ( startmask & srcbits ) | ( temp & ~startmask ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits ;
	temp = *dst ;
	*dst++ = ( endmask & srcbits ) | ( temp & ~endmask ) ;
    }
return ;
}

static void
fbori( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	*dst++ |= ( startmask & srcbits ) ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst |= srcbits ;
	*dst++ |= ( endmask & srcbits ) ;
    }
return ;
}

static void
fbnand( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	temp = *dst ;
	*dst++ = ( temp & ~startmask ) | ~( startmask & ( srcbits & temp ) );
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ ( srcbits & *dst ) ;
	temp = *dst ;
	*dst++ = ( temp & ~endmask ) | ~( endmask & ( srcbits & temp ) );
    }
return ;
}

static void
fbset( src, dst, h, centerWidth, startmask, endmask, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register unsigned long int startmask ;
register unsigned long int endmask ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;

    for ( ; h-- ; dst += djump ) {
	*dst++ |= startmask ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ 0 ;
	*dst++ |= endmask ;
    }
return ;
}

void (*mfb_fb_tile_funcs[])() = {
	fbclear,	/* GXclear:		0x0 Zero 0 */
	fband,		/* GXand:		0x1 src AND dst */
	fbandr,		/* GXandReverse:	0x2 src AND NOT dst */
	fbcopy,		/* GXcopy:		0x3 src */
	fbandi,		/* GXandInverted:	0x4 NOT src AND dst */
	fNoop,		/* GXnoop:		0x5 dst */
	fbxor,		/* GXxor:		0x6 src XOR dst */
	fbor,		/* GXor:		0x7 src OR dst */
	fbnor,		/* GXnor:		0x8 NOT src AND NOT dst */
	fbxnor,		/* GXequiv:		0x9 NOT src XOR dst */
	fbinv,		/* GXinvert:		0xa NOT dst */
	fborr,		/* GXorReverse:		0xb src OR NOT dst */
	fbcopyr,	/* GXcopyInverted:	0xc NOT src */
	fbori,		/* GXorInverted:	0xd NOT src OR dst */
	fbnand,		/* GXnand:		0xe NOT src OR NOT dst */
	fbset		/* GXset:		0xf 1 */
} ;

static void
fnclear( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register int midcnt ;
    for ( ; h-- ; dst += djump ) {
	for ( midcnt = centerWidth ; midcnt-- ; dst++ )
	    *dst = 0 ;
    }
return ;
}

static void
fnand( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register int midcnt ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( midcnt = centerWidth ; midcnt-- ; dst++ )
	    *dst &= srcbits ;
    }
return ;
}

static void
fnandr( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits & ~*dst ;
    }
return ;
}

static void
fncopy( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits ;
    }
return ;
}

static void
fnandi( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( srcbits = ~srcbits, temp = centerWidth ; temp-- ; dst++ )
	    *dst &= srcbits ;
    }
return ;
}

static void
fnxor( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst ^= srcbits ;
    }
return ;
}

static void
fnor( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst |= srcbits ;
    }
return ;
}

static void
fnnor( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ ( srcbits | *dst ) ;
    }
return ;
}

static void
fnxnor( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst ^= srcbits ;
    }
return ;
}

static void
fninv( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;

    for ( ; h-- ; dst += djump )
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ *dst ;
return ;
}

static void
fnorr( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits | ~ *dst ;
    }
return ;
}

static void
fncopyr( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = srcbits ;
    }
return ;
}

static void
fnori( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;
    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = ~ *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst |= srcbits ;
    }
return ;
}

static void
fnnand( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;
register unsigned long int srcbits ;

    for ( ; h-- ; src++, dst += djump )
    {
	if ( src == srcend )
		src = srcstart ;
	srcbits = *src ;
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ ( srcbits & *dst ) ;
    }
return ;
}

static void
fnset( src, dst, h, centerWidth, djump, srcend, srcstart )
register unsigned long int *src ;
register unsigned long int *dst ;
register int h ;
register int centerWidth ;
register int djump ;
register unsigned long int *srcstart ;
register unsigned long int *srcend ;
{
register unsigned long int temp ;

    for ( ; h-- ; dst += djump )
	for ( temp = centerWidth ; temp-- ; dst++ )
	    *dst = ~ 0 ;
return ;
}

void (*mfb_fn_tile_funcs[])() = {
	fnclear,	/* GXclear:		0x0 Zero 0 */
	fnand,		/* GXand:		0x1 src AND dst */
	fnandr,		/* GXandReverse:	0x2 src AND NOT dst */
	fncopy,		/* GXcopy:		0x3 src */
	fnandi,		/* GXandInverted:	0x4 NOT src AND dst */
	fNoop,		/* GXnoop:		0x5 dst */
	fnxor,		/* GXxor:		0x6 src XOR dst */
	fnor,		/* GXor:		0x7 src OR dst */
	fnnor,		/* GXnor:		0x8 NOT src AND NOT dst */
	fnxnor,		/* GXequiv:		0x9 NOT src XOR dst */
	fninv,		/* GXinvert:		0xa NOT dst */
	fnorr,		/* GXorReverse:		0xb src OR NOT dst */
	fncopyr,	/* GXcopyInverted:	0xc NOT src */
	fnori,		/* GXorInverted:	0xd NOT src OR dst */
	fnnand,		/* GXnand:		0xe NOT src OR NOT dst */
	fnset		/* GXset:		0xf 1 */
} ;
