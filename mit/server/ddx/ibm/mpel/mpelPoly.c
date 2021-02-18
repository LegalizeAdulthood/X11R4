/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/***********************************************************
		Copyright IBM Corporation 1987,1988

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

/* $Header: /andrew/X11/R3src/tape/server/ddx/ibm/mpel/RCS/mpelPoly.c,v 6.2 88/10/25 01:54:51 kbg Exp $ */
/* $Source: /andrew/X11/R3src/tape/server/ddx/ibm/mpel/RCS/mpelPoly.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/tape/server/ddx/ibm/mpel/RCS/mpelPoly.c,v 6.2 88/10/25 01:54:51 kbg Exp $";
#endif

/* 
 * solid polygon fill
 */

#include 	"X.h"
#include        "misc.h"
#include        "gcstruct.h"
#include        "windowstr.h"
#include        "scrnintstr.h"
#include	"region.h"
#include        "regionstr.h"

#include        "OScompiler.h"
#include        "ppc.h"

#include        "ibmTrace.h"

#include	"mpel.h"
#include	"mpelHdwr.h"
#include	"mpelFifo.h"

/* Cursor Stuff */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

/***==================================================================***/

void
mpelFillPolygon(pDraw, pGC, shape, mode, count, pPts)
    DrawablePtr		pDraw;
    GCPtr		pGC;
    int			shape, mode;
    int			count;
    DDXPointPtr		pPts;
{
    int cursor_saved ;
    RegionPtr		pCC;
    int			nbox;
    int			i;
    BoxRec		*pbox,bbox;
    mpelRectangle	scissor;
    DDXPointPtr		ppt;
    int			scissorOn,allIn;
    unsigned long int	fg ;
    unsigned long int	pm ;
    int			alu ;
    ppcPrivGC		*pPriv = (ppcPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr;


    TRACE(("mpelFillPolygon( pDraw=x%x, pGC=x%x, count=%d, pPts=x%x )\n",
						pDraw, pGC, count, pPts));

    if (count == 0)
	 return;

    if ( ( alu = pPriv->colorRrop.alu ) == GXnoop )
	return ;
    pm = pPriv->colorRrop.planemask ;
    fg = pPriv->colorRrop.fgPixel ;
    if ((pDraw->type != DRAWABLE_WINDOW)||(pGC->fillStyle!=FillSolid)) {
	ErrorF("WSGO! bad call to mpelFillPolygon\n");
	ErrorF("\tpDraw->type = %d =? %d\n",pDraw->type, DRAWABLE_WINDOW);
	ErrorF("\tpGC->fillStyle = %d =? %d\n",pGC->fillStyle, FillSolid);
	miFillPolygon(pDraw, pGC, shape, mode, count, pPts);
	return;
    }

    if (mode == CoordModePrevious) {
	ppt = &pPts[1];
	for (i = 1; i<count; i++) {
	    ppt->x += (ppt-1)->x;
	    ppt->y += (ppt-1)->y;
	    ppt++;
	}
    }

    pCC = pPriv->pCompositeClip;
    nbox= REGION_NUM_RECTS(pCC);
    if (nbox==0)
	return;

    bbox.x1= bbox.y1= 1024;
    bbox.x2= bbox.y2= 0;

    for (i=0;i<count;i++) {
	pPts[i].x+= pDraw->x;
	pPts[i].y+= pDraw->y;
	bbox.x1= MIN(bbox.x1,pPts[i].x);
	bbox.y1= MIN(bbox.y1,pPts[i].y);
	bbox.x2= MAX(bbox.x2,pPts[i].x);
	bbox.y2= MAX(bbox.y2,pPts[i].y);
	pPts[i].y= 1023-pPts[i].y;
    }

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore
    	    && mpelCheckCursor( bbox.x1, bbox.y1,
			        bbox.x2-bbox.x1, bbox.y2-bbox.y1 ) ;

    MPELSetPgonEdgeColor( fg );
    MPELSetPgonEdgeFlag(DRAW_EDGE);
    MPELSetPgonInteriorColor( fg );
    MPELSetPgonInteriorStyle(2);
    mpelSetALU( alu );
    mpelSetPlaneMask( pm );

    allIn= scissorOn= FALSE;

    pbox = REGION_RECTS(pCC);
    for (i=0;(i<nbox)&&(!allIn);i++,pbox++) {
	switch (mpelRectIntersect(pbox,&bbox)) {
	    case rgnOUT:	continue;
	    case rgnPART:	scissor.uright.x= 	pbox->x2-1;
				scissor.uright.y= 	1023-pbox->y1;
				scissor.lleft.x=  	pbox->x1;
				scissor.lleft.y=	1023-(pbox->y2-1);
				MPELSetScissor(&scissor);
				scissorOn= TRUE;
				break;
	    case rgnIN:		allIn= TRUE;
				if (scissorOn)  {
				    MPELResetScissor();
				    scissorOn= FALSE;
				}
				break;
	}
#ifdef AIXrt
	if (shape==Convex) {
	    if ((pPts[0].x==pPts[count-1].x)&&(pPts[0].y==pPts[count-1].y)) {
		MPELConvexPoly(count,pPts);
	    }
	    else {
		MPELOpenConvexPoly(count,pPts);
		MPELCloseConvexPoly(&pPts[0]);
	    }
	    continue;
	}
#endif
	MPELBeginPgon();
    	MPELPolyline(count,pPts);
    	MPELEndPgon();
    }
    if (scissorOn)
	MPELResetScissor();

    if ( cursor_saved )
	mpelReplaceCursor() ;

    return ;
}


/* 
 * Tiled polygon fill
 * Paquin 5/16/88, plagiarized above solid fill with user pattern for tile.
 * Only works on tiles which can expand to 16x16
 */

void
mpelTilePolygon(pDraw, pGC, shape, mode, count, pPts)
    DrawablePtr		pDraw;
    GCPtr		pGC;
    int			shape, mode;
    int			count;
    DDXPointPtr		pPts;
{
    int cursor_saved ;
    RegionPtr		pCC;
    int			nbox;
    int			i;
    BoxRec		*pbox,bbox;
    mpelRectangle	scissor;
    DDXPointPtr		ppt;
    int			scissorOn,allIn;
    unsigned long int	fg ;
    unsigned long int	pm ;
    int			alu ;
    int			expand_w, expand_h;
    ppcPrivGC		*pPriv = (ppcPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr;


    TRACE(("mpelTilePolygon( pDraw=x%x, pGC=x%x, count=%d, pPts=x%x )\n",
						pDraw, pGC, count, pPts));

    if (count == 0)
	 return;

    if ( ( alu = pPriv->colorRrop.alu ) == GXnoop )
	return ;
    pm = pPriv->colorRrop.planemask ;
    fg = pPriv->colorRrop.fgPixel ;

    if (
	(pDraw->type != DRAWABLE_WINDOW) ||	
	(pGC->fillStyle!=FillTiled)	
	) {
	ErrorF("WSGO! bad call to mpelTilePolygon\n");
	miFillPolygon(pDraw, pGC, shape, mode, count, pPts);
	return;
    }

    switch(pGC->tile.pixmap->drawable.width)
	{
	case 1:	
		expand_w = 16; break;
	case 2: 	
		expand_w = 8; break;
	case 4: 	
		expand_w = 4; break;
	case 8: 	
		expand_w = 2; break;
	case 16:	
		expand_w = 1; break;
	default:
		miFillPolygon(pDraw, pGC, shape, mode, count, pPts);
		return;
	}

    switch(pGC->tile.pixmap->drawable.height)
	{
	case 1:	
		expand_h = 16; break;
	case 2: 	
		expand_h = 8; break;
	case 4: 	
		expand_h = 4; break;
	case 8: 	
		expand_h = 2; break;
	case 16:	
		expand_h = 1; break;
	default:
		miFillPolygon(pDraw, pGC, shape, mode, count, pPts);
		return;
	}

    if (mode == CoordModePrevious) {
	ppt = &pPts[1];
	for (i = 1; i<count; i++) {
	    ppt->x += (ppt-1)->x;
	    ppt->y += (ppt-1)->y;
	    ppt++;
	}
    }

    pCC = pPriv->pCompositeClip;
    nbox= REGION_NUM_RECTS(pCC);
    if (nbox==0)
	return;

    bbox.x1= bbox.y1= 1024;
    bbox.x2= bbox.y2= 0;

    for (i=0;i<count;i++) {
	pPts[i].x+= pDraw->x;
	pPts[i].y+= pDraw->y;
	bbox.x1= MIN(bbox.x1,pPts[i].x);
	bbox.y1= MIN(bbox.y1,pPts[i].y);
	bbox.x2= MAX(bbox.x2,pPts[i].x);
	bbox.y2= MAX(bbox.y2,pPts[i].y);
	pPts[i].y= 1023-pPts[i].y;
    }

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore
    	    && mpelCheckCursor( bbox.x1, bbox.y1,
			        bbox.x2-bbox.x1, bbox.y2-bbox.y1 ) ;

    MPELSetPgonEdgeColor( fg );
    MPELSetPgonEdgeFlag(DONT_DRAW_EDGE);
    MPELSetPgonInteriorStyle(3);
    MPELSetPgonInteriorStyleIndex(1);
    mpelSetALU( alu );
    mpelSetPlaneMask( pm );

    {/* SET THE PATTERN IN MPEL PATTERN AREA */
    register unsigned volatile char *data = MPEL_PAT1;
    register unsigned char *src;
    register int tile_row, iw;
    int pix, tlx, tly;
    PixmapRec *pTile;
    int xSrc, ySrc, hoffset, voffset;

    pTile = pGC->tile.pixmap;
    tlx = pTile->drawable.width;
    tly = pTile->drawable.height;

    xSrc = pGC->patOrg.x + pDraw->x;
    ySrc = pGC->patOrg.y + pDraw->y;

    if ((hoffset = ((-xSrc)%tlx)) < 0) hoffset += tlx;
    if ((voffset = ((-ySrc)%tly)) < 0) voffset += tly;

    while (expand_h--)
	{
	src = pTile->devPrivate.ptr + (voffset * pTile->devKind);
	for (tile_row = voffset; tile_row < tly; tile_row++)
		{
		for(iw = 0; iw< expand_w; iw++) 
			{
			for (pix = hoffset; pix < tlx; pix++)
				{
				*data++ = src[pix];
				}
			for (pix = 0; pix < hoffset; pix++)
				{
				*data++ = src[pix];
				}
			}
		src += pTile->devKind;
		}

	src = pTile->devPrivate.ptr;
	for (tile_row = 0; tile_row < voffset; tile_row++)
		{
		for(iw = 0; iw< expand_w; iw++) 
			{
			for (pix = hoffset; pix < tlx; pix++)
				{
				*data++ = src[pix];
				}
			for (pix = 0; pix < hoffset; pix++)
				{
				*data++ = src[pix];
				}
			}
		src += pTile->devKind;
		}
	}
    }


    allIn= scissorOn= FALSE;

    pbox = REGION_RECTS(pCC);
    for (i=0;(i<nbox)&&(!allIn);i++,pbox++) {
	switch (mpelRectIntersect(pbox,&bbox)) {
	    case rgnOUT:	continue;
	    case rgnPART:	scissor.uright.x= 	pbox->x2-1;
				scissor.uright.y= 	1023-pbox->y1;
				scissor.lleft.x=  	pbox->x1;
				scissor.lleft.y=	1023-(pbox->y2-1);
				MPELSetScissor(&scissor);
				scissorOn= TRUE;
				break;
	    case rgnIN:		allIn= TRUE;
				if (scissorOn)  {
				    MPELResetScissor();
				    scissorOn= FALSE;
				}
				break;
	}
#ifdef AIXrt
	if (shape==Convex) {
	    if ((pPts[0].x==pPts[count-1].x)&&(pPts[0].y==pPts[count-1].y)) {
		MPELConvexPoly(count,pPts);
	    }
	    else {
		MPELOpenConvexPoly(count,pPts);
		MPELCloseConvexPoly(&pPts[0]);
	    }
	    continue;
	}
#endif
	MPELBeginPgon();
    	MPELPolyline(count,pPts);
    	MPELEndPgon();
    }
    if (scissorOn)
	MPELResetScissor();

    MPELResetPlaneMask();
    if ( cursor_saved )
	mpelReplaceCursor() ;

    return ;
}
