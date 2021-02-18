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

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelStip.c,v 6.4 88/11/03 22:11:48 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelStip.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelStip.c,v 6.4 88/11/03 22:11:48 paul Exp $" ;
#endif

#include "X.h"
#include "Xmd.h"
#include "pixmapstr.h"

#include "mpel.h"
#include "mpelHdwr.h"
#include "mpelFifo.h"

#include "ibmTrace.h"

/* Cursor Stuff */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

/***==================================================================***/

static void
XStipple(bitmap, stipWidth, stipHeight, x, y, w, h, fg, alu, mask)
    unsigned char *bitmap ;
    int stipWidth, stipHeight, x, y, w, h ;
    unsigned long int fg ;
    int alu ;
    unsigned long int mask ;
{
    unsigned char *srcPtr ;
    volatile unsigned char *dstPtr ;
    int		i,j ;
    int		MainPadBytes, ExcessPadBytes ;
    int		stipWidthInPaddedBytes, stipWidthInBytes, ExcessWidthInBytes ;
    int		xDestination, yDestination ;
    int		ExcessHeight, ExcessWidth, blth ;
    int		widthleft, heightleft ;
    int 	Mainmask, Excessmask ;
    int		PaddedWidthInBits ;
    mpelSrcBLTVPMWColorExpansion	blt ;

    TRACE(("XStipple(0x%x,%d,%d,%d,%d,%d,%d,%d,%d,0x%x)\n",
	bitmap,stipWidth,stipHeight,x,y,w,h,fg,alu,mask)) ;

    mpelSetPlaneMask( mask ) ;

    /* copy image to shared area */
    stipWidthInPaddedBytes = ((stipWidth+31)/32)<<2 ;
    stipWidthInBytes = (stipWidth+7)/8 ;

    /* check for bitmap > 64K bytes and printf-it */
    if ( ( stipWidthInPaddedBytes * stipHeight ) > ( MPEL_BLIT_STAGE_SIZE/2 ) )
	ErrorF("Stipple too big") ;

    MainPadBytes = stipWidthInPaddedBytes - stipWidthInBytes ;

    ExcessWidth = w % stipWidth ;		/* pad width to multiple of 8 */
    if (i=(ExcessWidth%8)) {
	Excessmask = 8 - i ;
	ExcessWidth = (ExcessWidth & ~7) + 8 ;
    } 
    else
	Excessmask = 0 ;

    ExcessHeight = h % stipHeight ;


    ExcessWidthInBytes = (ExcessWidth+7)/8 ;
    ExcessPadBytes = stipWidthInPaddedBytes - ExcessWidthInBytes ;

    if (i = (stipWidth%8)) {
	Mainmask = 8 - i ;
	PaddedWidthInBits = (stipWidth & ~7) + 8 ;
    }
    else {
	Mainmask = 0 ;
	PaddedWidthInBits = stipWidth ;
    }

    /* If the 'SetPlaneMask' has been swallowed we can continue */
    MPELWaitFifo() ;

    /* copy image TO adapter buffer area */
    srcPtr = bitmap ;
    dstPtr = (volatile unsigned char *) MPEL_BLIT_STAGE ;
    for (j = stipHeight ; j-- ; ) {
	for ( i = 0 ; i < stipWidthInBytes - 1 ; i++ ) 
	    *dstPtr++ = *srcPtr++ ;
	/* mask last byte */
	if (Mainmask) 
	     *dstPtr = *srcPtr & ((~0) << Mainmask) ;
	else
	     *dstPtr = *srcPtr ;
	srcPtr++ ; dstPtr++ ;
	/* skip over padding */
	srcPtr += MainPadBytes ;
    }

    /* copy image for rightmost (possibly clipped region */
    if (ExcessWidthInBytes) {
	srcPtr = bitmap ;
	dstPtr = ( (volatile unsigned char *) MPEL_BLIT_STAGE )
		+ ( MPEL_BLIT_STAGE_SIZE / 2 ) ;

	for ( j = stipHeight ; j-- ; ) {
	    for (i=0 ; i<ExcessWidthInBytes-1 ; i++) 
		*dstPtr++ = *srcPtr++ ;

	    /* mask last byte */
	    if (Excessmask) 
		*dstPtr = *srcPtr & ((~0) << Excessmask) ;
	    else
		*dstPtr = *srcPtr ;
	}
	srcPtr++ ;
	dstPtr++ ;
	/* skip over padding */
	srcPtr += ExcessPadBytes ;
    }
    /* start the blits */
    xDestination = x ; yDestination = y ;
    widthleft = w ; heightleft = h ;

    blt.color=		fg ;
    blt.alu=		alu+1 ;
    while (heightleft) {
	if (heightleft < stipHeight) {
	    blth = ExcessHeight ;
	    heightleft = 0 ;
	}
	else  {
	    blth = stipHeight ;
	    heightleft -= stipHeight ;
	}

	while (widthleft) {
	    blt.dest.lleft.x=	xDestination ;
	    if (widthleft < stipWidth) {
		/* blit use BANK3 */
		blt.srcaddr=
			mpelAddr( MPEL_BLIT_STAGE + (MPEL_BLIT_STAGE_SIZE/2)) ;
		blt.dest.lleft.y=	MPEL_HEIGHT - ( yDestination + blth ) ;
		blt.dest.uright.y=	MPEL_HEIGHT - 1 - yDestination ;
	        blt.dest.uright.x=	xDestination+ExcessWidth-1 ;
		MPELSrcBLTVPMWColorExpansion(&blt) ;
		/* xDestination = x ; */
		widthleft = 0 ;
		break ;
	    }
	    else {
		blt.srcaddr=		mpelAddr(MPEL_BLIT_STAGE) ;
		blt.dest.lleft.y=	MPEL_HEIGHT - ( yDestination + blth ) ;
		blt.dest.uright.y=	MPEL_HEIGHT - 1 - yDestination ;
	        blt.dest.uright.x=	xDestination+PaddedWidthInBits-1 ;
		MPELSrcBLTVPMWColorExpansion(&blt) ;

		xDestination += stipWidth ;
		widthleft -= stipWidth ;
	    }
	}
	xDestination = x ;
	yDestination += stipHeight ;
	widthleft = w ;
    }

    return ;
}

/***==================================================================***/

/*
 * mpelFillStipple(pStipple, fg, alu, planes,x,y,w,h,xSrc,ySrc)
 */

void
mpelFillStipple( pStipple, fg, alu, planes, x, y, w, h, xSrc, ySrc )
    PixmapPtr	pStipple ;
    unsigned long int fg ;
    int alu ;
    unsigned long int planes ;
    int  x, y, w, h, xSrc, ySrc ;
{
    int cursor_saved ;

    /* rotate image if needed */

    /* $$$$$$$$$$$$$$$$$$$$$$$$$$$
       $$$$$$$$$$$$$$$$$$$$$$$$$$$ */

    TRACE(("mpelFillStipple(0x%x,%d,%d,0x%x,%d,%d,%d,%d,0x%x,0x%x)\n",
	pStipple,fg,alu,planes,x,y,w,h,xSrc,ySrc)) ;

    if ( w <= 0 || h <= 0 || alu == GXnoop || !( planes &= MPEL_ALLPLANES ) )
	return ;

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore && mpelCheckCursor( x, y, w, h ) ;
    XStipple( pStipple->devPrivate, pStipple->width, pStipple->height,
	      x, y, w, h, fg, alu, planes ) ;
    if ( cursor_saved )
	mpelReplaceCursor() ;

    return ;
}
