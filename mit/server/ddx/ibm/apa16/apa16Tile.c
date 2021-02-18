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
/***********************************************************
Copyright 1989 by the Massachusetts Institute of Technology

                     All rights reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the Massachusetts
Institute of Technology (M.I.T.) not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.

M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Tile.c,v 3.0 89/11/02 15:20:08 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Tile.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Tile.c,v 3.0 89/11/02 15:20:08 probe Exp $";
static char sccsid[] = "@(#)apa16TILE.c	3.1 88/09/22 09:30:17";
#endif

#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "OScompiler.h"

#include "mfb.h"
#include "maskbits.h"

#include "apa16Hdwr.h"
#include "ibmTrace.h"

/***====================================================================***/

/* This is globals so that other modules can invalidate off-screen
 * scratch area
 */
unsigned int	*apa16CurrentTileBits=	NULL;

static void
apa16StageTile32( pTile )
PixmapPtr	pTile;
{
register volatile unsigned int	*pDst;
register unsigned int		*pSrc;
register int		 	 tileHeight;

    TRACE(("apa16StageTile32( pTile= 0x%x )\n"));
    pSrc = (unsigned int *)(pTile->devPrivate.ptr);
    apa16CurrentTileBits= pSrc;

    tileHeight = pTile->drawable.height;
    pDst = (volatile unsigned int *)SCREEN_ADDR(STAGE_X_OFFSET,STAGE_AREA_TOP);
    QUEUE_WAIT();
    while (tileHeight--) {
	*pDst= *pSrc++;
	pDst+= APA16_WIDTH/32;
    }
    apa16replicateArea( STAGE_X_OFFSET, STAGE_AREA_TOP, 
			STAGE_WIDTH, STAGE_HEIGHT, 
			pTile->drawable.width,
		        pTile->drawable.height ) ;
    return;
}

/***====================================================================***/

/* 
   this code could be called by the paint window background stuff,
too ; there would be some speed hit because of the different
parameters and the need to check for a rop when filling
with a tile.

   the boxes are already translated.

   NOTE:
   iy = ++iy < tileHeight ? iy : 0
is equivalent to iy%= tileheight, and saves a division.
*/

/* 
    tile area with a 32 bit wide pixmap 
*/
void
apa16TileArea32( pDraw, nbox, pbox, alu, ptile )
    DrawablePtr pDraw ;
    int nbox ;
    register BoxPtr pbox ;
    int alu ;
    PixmapPtr ptile ;
{
    register unsigned int *psrc ;
			/* pointer to bits in tile, if needed */
    int tileHeight ;	/* height of the tile */
    int tileWidth ;	/* width of the tile */
    int nlwidth ;	/* width in longwords of the drawable */
    int x ;
    int y ;
    int iy,ix ;		/* index of current scanline in tile */
    int w ;		/* width of current box */
    int h ;		/* height of current box */
    int startmask ;
    int endmask ;	/* masks for reggedy bits at either end of line */
    int nlwMiddle ;	/* number of longwords between sides of boxes */
    register unsigned int *p ;	/* pointer to bits we're writing */


    unsigned int *pbits ;	/* pointer to start of drawable */

    if ( !nbox )
	return ;

    switch ( alu ) {
	case GXclear:		/* 0x0 Zero 0 */
	case GXinvert:		/* 0xa NOT dst */
	case GXset:		/* 0xf 1 */
		apa16SolidFillArea( pDraw, nbox, pbox, alu, ptile ) ;
	case GXnoop:		/* 0x5 dst */
		return ;
	default:
		break ;
    }

    tileWidth = ptile->drawable.width ;

    if ( pDraw->type != DRAWABLE_WINDOW ) {
        if ( tileWidth == 32 ) {
                mfbTileArea32( pDraw, nbox, pbox, alu, ptile ) ;
        }
	else
puts( "Tile Width != 32. I give up!" ) ;
/*		mfbTileAreaXX( pDraw, nbox, pbox, alu, ptile ) ; */
	return ;
    }

    pbits = (unsigned int *)
	(((PixmapPtr)(pDraw->pScreen->devPrivate))->devPrivate.ptr) ;
    nlwidth = (int)
	(((PixmapPtr)(pDraw->pScreen->devPrivate))->devKind) >> 2 ;

    tileHeight = ptile->drawable.height ;
    psrc = (unsigned int *)(ptile->devPrivate.ptr) ;

    for( ; nbox; pbox++, nbox--) {
	x = pbox->x1 ;
	w = pbox->x2 - x ;
	y = pbox->y1 ;
	h = pbox->y2 - y ;

	if ( h > tileHeight || w > tileWidth ) { /* Special Iteration !! */
	    BoxRec tmpbox ;
	    int currentHoriz, currentVert ;
	    short int mergeLF ;
	    short int invert_src = 0 ;

	    currentHoriz = MIN( w, tileWidth ) ;
	    currentVert = MIN( h, tileHeight ) ;
	    switch ( alu ) {
		case GXcopy:		/* 0x3 src */
		case GXcopyInverted:	/* 0xc NOT src */
			/* Special Case Code */
			/* Draw First Tile in place then blit-replicate
			 * until the entire box is filled
			 */
			tmpbox.x1 = x ;
			tmpbox.x2 = x + currentHoriz ;
			tmpbox.y1 = y ;
			tmpbox.y2 = y + currentVert ;
			apa16TileArea32( pDraw, 1, &tmpbox, alu, ptile ) ;
			apa16replicateArea( x, y, w, h,
					    currentHoriz, currentVert ) ;
			continue ; /* There May Be Other Boxes ! */

		/* Hard Cases Iterate over tile */
		/* if possible for the easy cases use Hardware support */
		case GXand:		/* 0x1 src AND dst */
			mergeLF = 0x1 ;
			break ;
		case GXnor:		/* 0x8 NOT src AND NOT dst */
			invert_src = 1 ;
		case GXandReverse:	/* 0x2 src AND NOT dst */
			mergeLF = 0x2 ;
			break ;
		case GXandInverted:	/* 0x4 NOT src AND dst */
			mergeLF = 0x8 ;
			break ;
		case GXequiv:		/* 0x9 NOT src XOR dst */
			invert_src = 1 ;
		case GXxor:		/* 0x6 src XOR dst */
			mergeLF = 0xA ;
			break ;
		case GXor:		/* 0x7 src OR dst */
			mergeLF = 0xB ;
			break ;
		case GXorInverted:	/* 0xd NOT src OR dst */
			mergeLF = 0xC ;
			break ;
		case GXorReverse:	/* 0xb src OR NOT dst */
			invert_src = 1 ;
		case GXnand:		/* 0xe NOT src OR NOT dst */
			mergeLF = 0xE ;
			break ;
		default:
			ErrorF( "apa16TileArea: unexpected rrop" ) ;
	    }
	    /* Draw First Tile into the staging area ( off screen )
	     * then iterate-blit until entire box is filled
	     */
/*
puts( "Hard Cases Only \"Noop\"'ed For Now!" ) ;
	    if ( ( tileWidth <= STAGE_WIDTH )
	      && ( tileHeight <= STAGE_HEIGHT ) ) {
		    tmpbox.x2 =
			    ( tmpbox.x1 = APA16_STAGE_X ) + currentHoriz ;
		    tmpbox.y2 =
			    ( tmpbox.y1 = APA16_STAGE_Y ) + currentVert ;
		    apa16TileArea32( pDraw, 1, &tmpbox, alu, ptile ) ;
		    apa16iterateArea( x, y, w, h,
				      currentHoriz, currentVert ) ;
	    }
	    else {
	    }
	    continue ;
*/
	}
	/* One copy or less of source tile */
	if ( tileWidth == 32 ) { /* Standard "mfb" */
	    unsigned cmd;

	    APA16_GET_CMD(ROP_RECT_COPY,alu,cmd);
            if (cmd==0) {
                QUEUE_WAIT();
		mfbTileArea32( pDraw, nbox, pbox, alu, ptile );
		return;
	    }
            if ( psrc!=apa16CurrentTileBits )
	        apa16StageTile32( ptile );	
	    iy = y % tileHeight;
	    ix = x % tileWidth;
	    COPY_RECT(cmd,	x+w,	y+h,
				APA16_STAGE_X+ix+w,
				APA16_STAGE_Y+iy+h,
				w,h);
	}
	else { /* Tile Width != 32 */
puts( "Tile Width != 32. I give up!" ) ;
	}
    }

    return ;
}
