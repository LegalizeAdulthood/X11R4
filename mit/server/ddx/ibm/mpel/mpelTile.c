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

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelTile.c,v 6.6 89/04/29 21:38:28 jeff Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelTile.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelTile.c,v 6.6 89/04/29 21:38:28 jeff Exp $" ;
#endif

/*
 * mpeltile.c
 * fast tiles for the megapel
 * by Jeff
 */

#include "X.h"
#include "Xmd.h"
#include "pixmapstr.h"
#include "regionstr.h"

#include "OScompiler.h"

#include "mpel.h"
#include "mpelHdwr.h"
#include "mpelFifo.h"

#include "ibmTrace.h"

extern PixmapPtr ppcCreatePixmap() ;

/* Cursor Stuff */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

/***==================================================================***/

void
mpelTile( pTile, alu, planemask, x, y, w, h, xSrc, ySrc )
    PixmapPtr pTile ;
    int alu ;
    unsigned long int planemask ;
    int x, y, w, h, xSrc, ySrc ;
{
    register int		line ;
    int 			tilew, tileh ;
    int 			xrot, yrot ;
    register unsigned char	*psrc ;
    register volatile unsigned char 	*pdst ;
    unsigned char		*psrcSave ;
    int				cursor_saved ;
    mpelTileBLTVPMWMask 	tilearg ;
    PixmapPtr			pRotTile = (PixmapPtr)NULL ;
    PixmapPtr			pTmpTile ;
    int 			widthIterations ;
    int 			heightIterations ;
    int 			widthLeftOver ;
    int 			heightLeftOver ;

    TRACE(("mpelTile(0x%x,%d,0x%x,%d,%d,%d,%d,0x%x,0x%x)\n",
		pTile,alu,planemask,x,y,w,h,xSrc,ySrc)) ;

    if ( w <= 0 || h <= 0 )
	return ;

    switch ( alu ) {
	case GXclear:
	case GXset:
	case GXinvert:
		mpelFillSolid( MPEL_ALLPLANES, alu, planemask, x, y, w, h ) ;
	case GXnoop:
		return ;

	case GXcopy:
	case GXcopyInverted:
#if (HCVERSION>21000) && (HCVERSION<21007)
		/* 10/21/88 (ef) -- hc version 2.1d passes a bad	*/
		/*	argument to ppcTileRect.  a call to NoopDDA()	*/
		/*	(or any other function) fakes it out.		*/
		/*	Ain't compiler bugs fun??			*/
		NoopDDA() ;
#endif
		ppcTileRect( pTile, alu, planemask, x, y, w, h, xSrc, ySrc ) ;
		return ; /* ppc is fast for those cases */
	default: break ;
		/* fall through */
    }

    tilew = pTile->drawable.width ;
    tileh = pTile->drawable.height ;

    if ( ( tilew * tileh ) > ( MPEL_BLIT_STAGE_SIZE / 2 ) ) {
	ppcTileRect( pTile, alu, planemask, x, y, w, h, xSrc, ySrc ) ;
	return ;
    }

    if ( (xrot = (( x - xSrc ) % tilew )) < 0 ) xrot += tilew ;
    if ( (yrot = (( y - ySrc ) % tileh )) < 0 ) yrot += tileh ;

    if ( xrot || yrot ) {
	if ( !( pRotTile = ppcCreatePixmap( pTile->drawable.pScreen,
					    tilew, tileh,
					    pTile->drawable.depth ) ) ) {
	    ErrorF( "mpelTile: failed to create rot pixmap\n" ) ;
	    return ;
	}

	if ( !( pTmpTile = ppcCreatePixmap( pTile->drawable.pScreen,
					    tilew, tileh,
					    pTile->drawable.depth ) ) ) {
	    ErrorF( "mpelTile: failed to create tmp pixmap\n" ) ;
	    return ;
	}

	if ( xrot && !yrot )
	    ppcRotZ8mapLeft( pTile, pRotTile, xrot ) ;
	else if ( yrot && !xrot )
	    ppcRotZ8mapUp( pTile, pRotTile, yrot ) ;
	else {
	    ppcRotZ8mapLeft( pTile, pTmpTile, xrot ) ;
	    ppcRotZ8mapUp( pTmpTile, pRotTile, yrot ) ;
	}
	mfbDestroyPixmap( pTmpTile ) ;
	psrcSave = psrc = pRotTile->devPrivate.ptr ;
    }
    else
	psrcSave = psrc = pTile->devPrivate.ptr ;

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore && mpelCheckCursor( x, y, w, h ) ;

    mpelSetPlaneMask( planemask ) ;

    heightLeftOver = h % tileh ;
    heightIterations = h / tileh ;
    widthLeftOver = w % tilew ;
    widthIterations = w / tilew ;

    tilearg.alu =		alu + 1 ;

    if ( widthIterations ) {
	MPELWaitFifo() ; /* If The 'SetPlaneMask is done Go On */
	pdst = (volatile unsigned char *) MPEL_BLIT_STAGE ;
	for ( line = tileh ; line-- ;  ) {
	    MOVE( psrc, pdst, tilew ) ;
	    psrc += pTile->devKind ;
	    pdst += tilew ;
	}
	/*
	 * tile the upper left area
	 */
	if ( heightIterations ) {
	    tilearg.tileaddr= 		mpelAddr(MPEL_BLIT_STAGE) ;
	    tilearg.twidth= 		tilew ;
	    tilearg.theight= 		tileh ;
	    tilearg.dest.lleft.x= 	x ;
	    tilearg.dest.lleft.y=	1023-(y+(heightIterations*tileh)-1) ;
	    tilearg.dest.uright.x=	x + ( widthIterations * tilew ) - 1 ;
	    tilearg.dest.uright.y= 	1023 - y ;
	    MPELTileBLTVPMNoMask( &tilearg ) ;
	}
	if ( heightLeftOver ) {
	    tilearg.tileaddr=		mpelAddr(MPEL_BLIT_STAGE) ;
	    tilearg.twidth=		tilew ;
	    tilearg.theight=		heightLeftOver ;
	    tilearg.dest.lleft.x=	x ;
	    tilearg.dest.lleft.y=	1023 - ( y + h - 1 ) ;
	    tilearg.dest.uright.x=	x + ( widthIterations * tilew ) - 1 ;
	    tilearg.dest.uright.y=	1023 - ( y + heightIterations * tileh ) ;
	    MPELTileBLTVPMNoMask( &tilearg ) ;
	}
    }

    if ( widthLeftOver ) {
	MPELNoOp( 2, &MPELNoOpData ) ;
	pdst = ( (volatile unsigned char *)MPEL_BLIT_STAGE )
	     + MPEL_BLIT_STAGE_SIZE/2 ;
	psrc = psrcSave ;
	MPELWaitFifo() ;
	for ( line = tileh ; line-- ;  ) {
	    MOVE( psrc, pdst, widthLeftOver ) ;
	    psrc += pTile->devKind ;
	    pdst += widthLeftOver ;
	}
	/*
	 * tile the upper left area
	 */
	if ( heightIterations ) {
	    tilearg.tileaddr=	(long) mpelAddr( MPEL_BLIT_STAGE ) +
						( MPEL_BLIT_STAGE_SIZE / 4 ) ;
	    tilearg.twidth=		widthLeftOver ;
	    tilearg.theight=		tileh ;
	    tilearg.dest.lleft.x=	x + widthIterations * tilew ;
	    tilearg.dest.lleft.y=	MPEL_HEIGHT
					- ( y + ( heightIterations * tileh ) ) ;
	    tilearg.dest.uright.x=	x + w - 1 ;
	    tilearg.dest.uright.y=	MPEL_HEIGHT - 1 - y ;
	    MPELTileBLTVPMNoMask( &tilearg ) ;
	}
	if ( heightLeftOver ) {
	    tilearg.tileaddr= (long) mpelAddr(MPEL_BLIT_STAGE)
				+ (MPEL_BLIT_STAGE_SIZE / 4) ;
	    tilearg.twidth=		widthLeftOver ;
	    tilearg.theight=		heightLeftOver ;
	    tilearg.dest.lleft.x=	x + widthIterations * tilew ;
	    tilearg.dest.lleft.y=	MPEL_HEIGHT - ( y + h ) ;
	    tilearg.dest.uright.x=	x + w - 1 ;
	    tilearg.dest.uright.y=	MPEL_HEIGHT - 1
					- ( y + heightIterations * tileh ) ;
	    MPELTileBLTVPMNoMask( &tilearg ) ;
	}
    }
    /*
     * destroy the rotated pixmap
     */
    if ( pRotTile )
	mfbDestroyPixmap( pRotTile ) ;

    if ( cursor_saved )
	mpelReplaceCursor() ;

    return ;
}
