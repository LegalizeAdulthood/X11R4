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

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelLine.c,v 6.5 88/11/04 19:37:45 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelLine.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelLine.c,v 6.5 88/11/04 19:37:45 paul Exp $" ;
#endif

#include "X.h"
#include "misc.h"
#include "gc.h"
#include "gcstruct.h"
#include "regionstr.h"
#include "colormapst.h"
#include "pixmapstr.h"
#include "windowstr.h"

#include "OScompiler.h"

#include "ppc.h"

#include "mpel.h"
#include "mpelHdwr.h"
#include "mpelFifo.h"

#include "ibmTrace.h"

#define X_AXIS 0

/* Cursor Stuff */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

/*****************************************************************
 * mpelPolySegment
 *
 *    For each segment, draws a line between (x1, y1 ) and (x2, y2 ) .  The
 *    lines are drawn in the order listed.
 *
 *****************************************************************/

void
mpelPolySegment( pDraw, pGC, nseg, pSegs )
    DrawablePtr pDraw ;
    GCPtr 	pGC ;
    int		nseg ;
    xSegment	*pSegs ;
{
    register DDXPointPtr ppt ;
    register int nbox ;
    register BoxPtr pbox ;
    int nptTmp ;
    RegionPtr pRegion ;
    mpelRectangle scissor ;
    int cursor_saved ;
    ppcPrivGCPtr pPriv = (ppcPrivGCPtr ) pGC->devPrivates[mfbGCPrivateIndex].ptr ;
    mpelPrivGCPtr mpelPriv = (mpelPrivGCPtr ) pPriv->devPriv ;

    if (  pPriv->colorRrop.alu == GXnoop )
	return ;

    pRegion = pPriv->pCompositeClip ;
    if ( !( nbox = REGION_NUM_RECTS(pRegion) ) )
	return ;

    /* translate the point list */
    nptTmp = nseg * 2 ;
    ppt = (DDXPointPtr ) pSegs ;

    while ( nptTmp-- )
    {
    	register int xorg = pDraw->x ;
    	register int yorg = pDraw->y ;

	ppt->x += xorg ;
	ppt->y += yorg ;
	ppt->y = MPEL_HEIGHT - 1 - ppt->y ;
	ppt++ ;
    }

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore
   	&& mpelCheckCursor(
		pRegion->extents.x1,
 		pRegion->extents.y1,
		pRegion->extents.x2 - pRegion->extents.x1,
		pRegion->extents.y2 - pRegion->extents.y1
    ) ;

    switch(pGC->lineStyle ) {
    case LineOnOffDash:
        switch ( pPriv->colorRrop.fillStyle ) {
	    case FillStippled :
	    case FillSolid : /* Can Use Optimized Color Raster-Op */
    		mpelSetALU( pPriv->colorRrop.alu ) ;
    		mpelSetPlaneMask( pPriv->colorRrop.planemask ) ;
    		MPELSetPolylineColor( pPriv->colorRrop.fgPixel ) ;
	        break ;
	    case FillOpaqueStippled :
	    case FillTiled : /* Can't Use Optimized Color Raster-Op */
    		mpelSetALU(pGC->alu ) ;
    		mpelSetPlaneMask(pGC->planemask ) ;
    		MPELSetPolylineColor( pGC->fgPixel ) ;
	        break ;
        }
	break ;
    case LineDoubleDash:
        switch ( pPriv->colorRrop.fillStyle ) {
	    case FillStippled :
	    case FillSolid :
	    case FillTiled : /* Can't Use Optimized Color Raster-Op */
    		mpelSetALU(pGC->alu ) ;
    		mpelSetPlaneMask(pGC->planemask ) ;
    		MPELSetPolylineColor( pGC->fgPixel ) ;
    		if ( mpelPriv->LineType == MPEL_USERLINESTYLE_CONT )
			MPELSetUserLineStyle(&mpelPriv->LineStyle ) ;
    		MPELSetLineType(mpelPriv->LineType ) ;
/* SCISSOR CODE STINKS -- IT MUST GO AWAY !! */
    		for ( pbox = REGION_RECTS(pRegion) ;nbox-- ;pbox++ ) {
			scissor.uright.x= 	pbox->x2-1 ;
			scissor.uright.y= 	MPEL_HEIGHT - 1 - pbox->y1 ;
			scissor.lleft.x=  	pbox->x1 ;
			scissor.lleft.y=	MPEL_HEIGHT - pbox->y2 ;
			MPELSetScissor(&scissor ) ;
    			MPELMultiline(nseg * 2, pSegs ) ;
    		}
    		MPELSetPolylineColor( pGC->bgPixel ) ;
		mpelPriv->LineStyle.mask = ~mpelPriv->LineStyle.mask ;
	        break ;
	    case FillOpaqueStippled : /* Can Use Optimized Color Raster-Op */
    		mpelSetALU( pPriv->colorRrop.alu ) ;
    		mpelSetPlaneMask( pPriv->colorRrop.planemask ) ;
    		MPELSetPolylineColor( pPriv->colorRrop.fgPixel ) ;
    		if ( mpelPriv->LineType == MPEL_USERLINESTYLE_CONT )
			MPELSetUserLineStyle(&mpelPriv->LineStyle ) ;
    		MPELSetLineType(mpelPriv->LineType ) ;
/* SCISSOR CODE STINKS -- IT MUST GO AWAY !! */
    		for ( pbox = REGION_RECTS(pRegion) ; nbox-- ; pbox++ ) {
			scissor.uright.x= 	pbox->x2-1 ;
			scissor.uright.y= 	MPEL_HEIGHT - 1 - pbox->y1 ;
			scissor.lleft.x=  	pbox->x1 ;
			scissor.lleft.y=	MPEL_HEIGHT - pbox->y2 ;
			MPELSetScissor(&scissor ) ;
    			MPELMultiline( nseg*2, pSegs ) ;
    		}
    		MPELSetPolylineColor(
	        	pPriv->colorRrop.bgPixel ) ;
		mpelPriv->LineStyle.mask = ~mpelPriv->LineStyle.mask ;
	        break ;
        }
	break ;
    case LineSolid:
    	mpelSetALU( pGC->alu ) ;
    	mpelSetPlaneMask( pGC->planemask ) ;
    	MPELSetPolylineColor( pGC->fgPixel ) ;
	break ;
    default:
	ErrorF( "unknown line type\n" ) ;
	break ;
    }

    if ( mpelPriv->LineType == MPEL_USERLINESTYLE_CONT )
	MPELSetUserLineStyle( &mpelPriv->LineStyle ) ;
    MPELSetLineType( mpelPriv->LineType ) ;

    pbox = REGION_RECTS(pRegion) ;
/* SCISSOR CODE STINKS -- IT MUST GO AWAY !! */
    for ( nbox = REGION_NUM_RECTS(pRegion) ; nbox-- ; pbox++ ) {
	scissor.uright.x= 	pbox->x2-1 ;
	scissor.uright.y= 	MPEL_HEIGHT - 1 - pbox->y1 ;
	scissor.lleft.x=  	pbox->x1 ;
	scissor.lleft.y=	MPEL_HEIGHT - pbox->y2 ;
	MPELSetScissor( &scissor ) ;
    	MPELMultiline( nseg*2, pSegs ) ;
    }

    MPELResetScissor() ;

    if ( pGC->lineStyle == LineDoubleDash )
	mpelPriv->LineStyle.mask = ~mpelPriv->LineStyle.mask ;

    if ( cursor_saved )
	mpelReplaceCursor() ;
    return ;
}

void
mpelHorzLine( color, alu, mask, x, y, w )
unsigned long int color ;
int alu ;
unsigned long int mask ;
int x, y, w ;
{
	mpelFillSolid( color, alu, mask, x, y, w, 1 ) ;
	return ;
}

void
mpelVertLine( color, alu, mask, x, y, h )
unsigned long int color ;
int alu ;
unsigned long int mask ;
int x, y, h ;
{
	mpelFillSolid( color, alu, mask, x, y, 1, h ) ;
	return ;
}

/* Solid bresenham line */

void
mpelBresLine( color, alu, mask, signdx, signdy, axis, x1, y1, e, e1, e2, len )
unsigned long int color ;
int alu ;
unsigned long int mask ;
int signdx, signdy ;	/* signs of directions */
int axis ;		/* major axis (Y_AXIS or X_AXIS ) */
int x1, y1 ;		/* initial point */
register int e ;		/* error accumulator */
register int e1 ;	/* bresenham increments */
int e2 ;
register int len ;	/* length of line */
{
    register int x = x1 ;
    register int y = y1 ;

    if (axis == X_AXIS )
    {
	    if (signdx > 0 )
	    {
	        while ( len-- )
	        {
			mpelFillSolid( color, alu, mask, x, y, 1, 1 ) ;
		    	if (e < 0 )
		        	e += e1 ;
		    	else
		    	{
				if ( signdy > 0 )
		        		y++ ;
				else
					y-- ;
		        	e += e2 ;
		    	}
			x++ ;
	        }
	    }
	    else
	    {
	        while ( len-- )
	        {
			mpelFillSolid( color, alu, mask, x, y, 1, 1 ) ;
		    	if (e <= 0 )
		        	e += e1 ;
		    	else
		    	{
				if ( signdy > 0 )
		        		y++ ;
				else
					y-- ;
		        	e += e2 ;
		    	}
			x-- ;
	        }
	    }
   } /* if X_AXIS */
   else
   {
	    if (signdx > 0 )
	    {
	        while ( len-- )
	        {
			mpelFillSolid( color, alu, mask, x, y, 1, 1 ) ;
		    	if (e < 0 )
		        	e += e1 ;
		    	else
		    	{
				x++ ;
		        	e += e2 ;
		    	}
			if ( signdy > 0 )
		       		y++ ;
			else
				y-- ;
	        }
	    }
	    else
	    {
	        while ( len-- )
	        {
			mpelFillSolid( color, alu, mask, x, y, 1, 1 ) ;
		    	if (e <= 0 )
		        	e += e1 ;
		    	else
		    	{
				x-- ;
		        	e += e2 ;
		    	}
			if (signdy > 0 )
		       		y++ ;
			else
				y-- ;
	        }
	    }
    } /* else Y_AXIS */
    return ;
}

/***==================================================================***/

void
mpelZeroLine( dst, pGC, mode, n, pPoint )
    DrawablePtr dst ;
    GCPtr pGC ;
    int mode ;
    int n ;
    DDXPointPtr pPoint ;
{
    register DDXPointPtr ppt ;
    register int nbox ;
    register BoxPtr pbox ;
    int nptTmp ;
    RegionPtr pRegion ;
    int cursor_saved ;
    mpelRectangle scissor ;
    ppcPrivGCPtr pPriv = (ppcPrivGCPtr ) pGC->devPrivates[mfbGCPrivateIndex].ptr ;
    mpelPrivGCPtr mpelPriv = (mpelPrivGCPtr ) pPriv->devPriv ;

    TRACE( ("mpelZeroLine(0x%x,0x%x,%d,%d,0x%x)\n", dst, pGC, mode, n, pPoint));

    if ( pPriv->colorRrop.alu == GXnoop )
	return ;

    pRegion = pPriv->pCompositeClip ;
    if ( !( nbox = REGION_NUM_RECTS(pRegion) ) )
	return ;

    /* translate the point list */
    ppt = pPoint ;
    nptTmp = n ;

    if (mode == CoordModeOrigin )
    {
	while ( nptTmp-- )
	{
    	    register int xorg = dst->x ;
    	    register int yorg = dst->y ;

	    ppt->x += xorg ;
	    ppt->y += yorg ;
	    ppt->y = MPEL_HEIGHT - 1 - ppt->y ;
	    ppt++ ;
	}
    }
    else
    {
    	register int xorg = dst->x ;
    	register int yorg = dst->y ;

	ppt->x += xorg ;
	ppt->y += yorg ;
	nptTmp-- ;
	while ( nptTmp-- )
	{
	    ppt++ ;
	    ppt->x += (ppt-1)->x ;
	    ppt->y += (ppt-1)->y ;
	    (ppt-1)->y = MPEL_HEIGHT - 1 - (ppt-1)->y ;
	}
	ppt->y = MPEL_HEIGHT - 1 - ppt->y ;
    }

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore
   	&& mpelCheckCursor(
		pRegion->extents.x1,
 		pRegion->extents.y1,
		pRegion->extents.x2 - pRegion->extents.x1,
		pRegion->extents.y2 - pRegion->extents.y1
    ) ;

    switch ( pGC->lineStyle ) {
    case LineOnOffDash:
        switch ( pPriv->colorRrop.fillStyle ) {
	    case FillStippled :
	    case FillSolid : /* Can Use Optimized Color Raster-Op */
    		mpelSetALU( pPriv->colorRrop.alu ) ;
    		mpelSetPlaneMask( pPriv->colorRrop.planemask ) ;
    		MPELSetPolylineColor( pPriv->colorRrop.fgPixel ) ;
	        break ;
	    case FillOpaqueStippled :
	    case FillTiled : /* Can't Use Optimized Color Raster-Op */
    		mpelSetALU( pGC->alu ) ;
    		mpelSetPlaneMask( pGC->planemask ) ;
    		MPELSetPolylineColor( pGC->fgPixel ) ;
	        break ;
        }
	break ;
    case LineDoubleDash:
        switch ( pPriv->colorRrop.fillStyle ) {
	    case FillStippled :
	    case FillSolid :
	    case FillTiled : /* Can't Use Optimized Color Raster-Op */
    		mpelSetALU( pGC->alu ) ;
    		mpelSetPlaneMask( pGC->planemask ) ;
    		MPELSetPolylineColor( pGC->fgPixel ) ;
    		if ( mpelPriv->LineType == MPEL_USERLINESTYLE_CONT )
			MPELSetUserLineStyle(&mpelPriv->LineStyle ) ;
    		MPELSetLineType( mpelPriv->LineType ) ;
/* SCISSOR CODE STINKS -- IT MUST GO AWAY !! */
    		for ( pbox = REGION_RECTS(pRegion) ; nbox-- ; pbox++ ) {
			scissor.uright.x= 	pbox->x2-1 ;
			scissor.uright.y= 	MPEL_HEIGHT - 1 - pbox->y1 ;
			scissor.lleft.x=  	pbox->x1 ;
			scissor.lleft.y=	MPEL_HEIGHT - pbox->y2 ;
			MPELSetScissor( &scissor ) ;
    			MPELPolyline( n, pPoint ) ;
    		}
    		MPELSetPolylineColor( pGC->bgPixel ) ;
		mpelPriv->LineStyle.mask = ~mpelPriv->LineStyle.mask ;
	        break ;
	    case FillOpaqueStippled : /* Can Use Optimized Color Raster-Op */
    		mpelSetALU( pPriv->colorRrop.alu ) ;
    		mpelSetPlaneMask( pPriv->colorRrop.planemask ) ;
    		MPELSetPolylineColor( pPriv->colorRrop.fgPixel ) ;
    		if ( mpelPriv->LineType == MPEL_USERLINESTYLE_CONT )
			MPELSetUserLineStyle( &mpelPriv->LineStyle ) ;
    		MPELSetLineType( mpelPriv->LineType ) ;
/* SCISSOR CODE STINKS -- IT MUST GO AWAY !! */
    		for ( pbox = REGION_RECTS(pRegion); nbox-- ; pbox++ ) {
			scissor.uright.x= 	pbox->x2-1 ;
			scissor.uright.y= 	MPEL_HEIGHT - 1 - pbox->y1 ;
			scissor.lleft.x=  	pbox->x1 ;
			scissor.lleft.y=	MPEL_HEIGHT - pbox->y2 ;
			MPELSetScissor( &scissor ) ;
    			MPELPolyline( n, pPoint ) ;
    		}
    		MPELSetPolylineColor( pPriv->colorRrop.bgPixel ) ;
		mpelPriv->LineStyle.mask = ~mpelPriv->LineStyle.mask ;
	        break ;
        }
	break ;
    case LineSolid:
    	mpelSetALU( pGC->alu ) ;
    	mpelSetPlaneMask( pGC->planemask ) ;
    	MPELSetPolylineColor( pGC->fgPixel ) ;
	break ;
    default:
	ErrorF( "unknown line type\n" ) ;
	break ;
    }

    if ( mpelPriv->LineType == MPEL_USERLINESTYLE_CONT )
	MPELSetUserLineStyle( &mpelPriv->LineStyle ) ;
    MPELSetLineType( mpelPriv->LineType ) ;

    pbox = REGION_RECTS(pRegion) ;
/* SCISSOR CODE STINKS -- IT MUST GO AWAY !! */
    for ( nbox = REGION_NUM_RECTS(pRegion) ; nbox-- ; pbox++ ) {
	scissor.uright.x= 	pbox->x2-1 ;
	scissor.uright.y= 	MPEL_HEIGHT - 1 - pbox->y1 ;
	scissor.lleft.x=  	pbox->x1 ;
	scissor.lleft.y=	MPEL_HEIGHT - pbox->y2 ;
	MPELSetScissor( &scissor ) ;
    	MPELPolyline( n, pPoint ) ;
    }

    MPELResetScissor() ;

    if ( pGC->lineStyle == LineDoubleDash )
	mpelPriv->LineStyle.mask = ~mpelPriv->LineStyle.mask ;

    if ( cursor_saved )
	mpelReplaceCursor() ;
    return ;
}
