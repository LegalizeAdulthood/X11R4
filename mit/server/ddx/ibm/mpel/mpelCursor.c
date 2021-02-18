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

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelCursor.c,v 6.5 88/11/04 19:35:58 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelCursor.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelCursor.c,v 6.5 88/11/04 19:35:58 paul Exp $" ;
#endif

/*
 * xcursor.c -- software cursor
 */

#include "X.h"
#include "miscstruct.h"
#include "cursorstr.h"
#include "misc.h"
#include "scrnintstr.h"
#include "screenint.h"
#include "colormapst.h"
#include "colormap.h"
#include "pixmapstr.h"

#include "maskbits.h"

#include "OScompiler.h"

#include "ibmScreen.h"
#include "ibmColor.h"

#include "ppc.h"

#include "mpel.h"
#include "mpelHdwr.h"
#include "mpelFifo.h"

#include "ibmTrace.h"

#define	CURSOR_MAX_SIZE		( ( 32 / 8 ) * 32 )

/* Global Cursor State Semaphore */
int mpelcursorSemaphore = 0 ;

static int screen_index ;
static short c_x ;
static short c_y ;
static unsigned long int c_fg = MPEL_WHITE_PIXEL ;
static unsigned long int c_bg = MPEL_BLACK_PIXEL ;
static int cursor_not_displayed = TRUE ;
static int cursor_is_valid = FALSE ;

static unsigned short int forePtrn[ CURSOR_MAX_SIZE / 2 ] ;
static unsigned short int backPtrn[ CURSOR_MAX_SIZE / 2 ] ;

/*** ==================================================================***/

/*
 * mpelPutCursorOn( x, y )
 */

static void
mpelPutCursorOn( x, y )
    register short int x, y ;
{
    register CursorPtr curCursor ;
    register unsigned int byteSize, w ;
    register unsigned short int *fSrc ;
    register unsigned short int *bSrc ;
    unsigned short int tempFore[CURSOR_MAX_SIZE] ;
    unsigned short int tempBack[CURSOR_MAX_SIZE] ;
    mpelVPMBLTDestination saveBlit ;
    mpelBLTImmedWColorExpansion	cursorBlit ;

    TRACE( ( "mpelPutCursorOn(x=%d,y=%d)\n", x, y )) ;

    if ( ibmScreenState( screen_index ) != SCREEN_ACTIVE )
	return ;
    curCursor = ibmCurrentCursor( screen_index ) ;
    fSrc = (unsigned short int *) forePtrn ;
    bSrc = (unsigned short int *) backPtrn ;

    /* Save The Existing Image */
    saveBlit.destaddr = mpelAddr( MPEL_CURSOR_SAVE ) ;
    saveBlit.source.lleft.x = x ;
    saveBlit.source.lleft.y = MPEL_HEIGHT - ( y + MIN( curCursor->bits->height, 32 ) ) ;
    saveBlit.source.uright.x = x + ( curCursor->bits->width > 16 ? 32 : 16 ) - 1 ;
    saveBlit.source.uright.y = MPEL_HEIGHT - 1 - y ;
    saveBlit.comp = 0x0001 ;

    MPELSetPlaneMask( 0 ) ;
    MPELVPMBLTDest( &saveBlit ) ;

    if ( x > MPEL_WIDTH - ( curCursor->bits->width > 16 ? 32 : 16 ) ) {
	int i ;
	unsigned short int *fDst, *bDst ;
	unsigned short int mask ;

	mask = endtab[ ( MPEL_WIDTH - x ) & 0xF ] >> 16 ;
	w = ( ( ( MPEL_WIDTH - x ) + 0xF ) >> 4 ) << 4 ;
	fDst = tempFore ;
	bDst = tempBack ;
	for ( i = MIN( curCursor->bits->height, 32 ) ; i-- ; ) {
	    *fDst = *fSrc++ ;
	    *bDst = *bSrc++ ;
	    if ( ( curCursor->bits->width > 16 ? 32 : 16 ) > 16 ) {
		if ( w > 16 ) {
		    *++fDst = *fSrc ;
		    *++bDst = *bSrc ;
		}
		fSrc++ ;
		bSrc++ ;
	    }
	    *fDst++ &= mask ;
	    *bDst++ &= mask ;
	}
	fSrc = tempFore ;
	bSrc = tempBack ;
    }
    else {
	w = ( curCursor->bits->width > 16 ? 32 : 16 ) ;
    }

    cursorBlit.dest.lleft.x = x ;
    cursorBlit.dest.lleft.y = MPEL_HEIGHT - ( y + MIN( curCursor->bits->height, 32 ) ) ;
    cursorBlit.dest.uright.x = x + w - 1 ;
    cursorBlit.dest.uright.y = MPEL_HEIGHT - 1 - y ;
    cursorBlit.alu = GXcopy + 1 ;
    cursorBlit.color = c_fg ;
    byteSize = ( w >> 3 ) * MIN( curCursor->bits->height, 32 ) ;

    MPELBLTImmedWColorExpansion( byteSize, &cursorBlit ) ;
    MPELSendData( byteSize, fSrc ) ;

    cursorBlit.color = c_bg ;
    MPELBLTImmedWColorExpansion( byteSize, &cursorBlit ) ;
    MPELSendData( byteSize, bSrc ) ;

    /* Cursor Is Now Active */
    cursor_not_displayed = 0 ;

    return ;
}

/*** ==================================================================***/

int
mpelRemoveCursor()
{
    if ( !cursor_not_displayed
      && ( ibmScreenState( screen_index ) == SCREEN_ACTIVE ) ) {
	register CursorPtr curCursor ;
	mpelSrcBLTVPM blt ;

	curCursor = ibmCurrentCursor( screen_index ) ;
	blt.srcaddr = mpelAddr( MPEL_CURSOR_SAVE ) ;
	blt.dest.lleft.x = c_x ;
	blt.dest.lleft.y = MPEL_HEIGHT - ( c_y + MIN( curCursor->bits->height, 32 ) ) ;
	blt.dest.uright.x = c_x + ( curCursor->bits->width > 16 ? 32 : 16 ) - 1 ;
	blt.dest.uright.y = MPEL_HEIGHT - 1 - c_y ;
	blt.bpixel = 0x0008 ;
	blt.alu = GXcopy + 1 ;

	MPELSetPlaneMask( 0 ) ;
	MPELSrcBLTVPM( &blt ) ;
    }

    return cursor_not_displayed = TRUE ;
}

/*** ==================================================================***/

void
mpelShowCursor( x, y )
    register short x, y ;
{
register CursorPtr curCursor = ibmCurrentCursor( screen_index ) ;

    if ( ibmScreenState( screen_index ) != SCREEN_ACTIVE ) {
	c_x = x - curCursor->bits->xhot ;
	c_y = y - curCursor->bits->yhot ;
    }
    if ( !cursor_not_displayed ) {
	(void) mpelRemoveCursor() ;
	x -= curCursor->bits->xhot ;
	y -= curCursor->bits->yhot ;
    }
    c_x = x ;
    c_y = y ;
    if ( cursor_is_valid )
	mpelPutCursorOn( x, y ) ;
    return ;
}

/*** ================================================================== ***/

/* check if the cursor is in this rectangle.  if so, remove and return TRUE
    else return FALSE */
int
mpelCheckCursor( x, y, lx, ly )
register const int x, y, lx, ly ;
{
register CursorPtr curCursor = ibmCurrentCursor( screen_index ) ;

if ( !mpelcursorSemaphore && !cursor_not_displayed 
  && ( ibmScreenState( screen_index ) == SCREEN_ACTIVE )
  && !( ( x >= ( c_x + ( curCursor->bits->width > 16 ? 32 : 16 ) ) )
     || ( y >= ( c_y + MIN( curCursor->bits->height, 32 ) ) )
     || ( ( x + lx ) <= c_x )
     || ( ( y + ly ) <= c_y ) ) )
	return mpelRemoveCursor() ;
else
	return FALSE ;
/*NOTREACHED*/
}

/*** ==================================================================***/

void mpelReplaceCursor()
{

if ( cursor_not_displayed && !mpelcursorSemaphore &&
	ibmScreenState( screen_index ) == SCREEN_ACTIVE )
	mpelShowCursor( c_x, c_y ) ;

return ;
}

/*** ==================================================================***/

void
mpelRecolorCursor( cmap )
    register ColormapPtr cmap ;
{
    register CursorPtr curCursor = ibmCurrentCursor( screen_index ) ;
    TRACE( ( "mpelRecolorCursor(0x%x)\n", cmap ) ) ;

    c_fg = ibmFindColor( cmap, curCursor->foreRed, curCursor->foreGreen,
			 curCursor->foreBlue ) ;
    c_bg = ibmFindColor( cmap, curCursor->backRed, curCursor->backGreen,
			 curCursor->backBlue ) ;
    return ;
}

/* ************************************************************************** */

void
mpelCursorInit( index )
register int index ;
{
    TRACE( ( "mpelCursorInit()\n" )) ;

    ibmCursorShow( index ) = mpelShowCursor ;

    ibmCurrentCursor( index ) = NULL ;
    cursor_is_valid = FALSE ;

    screen_index = index ;

    c_x = 0 ;
    c_y = 0 ;
    mpelcursorSemaphore = 0 ;
    cursor_not_displayed = TRUE ;
    return ;
}

/*** ============================================================***/

Bool
mpelRealizeCursor( pScr, pCurs )
    register ScreenPtr	pScr ;
    register CursorPtr	pCurs ;
{
    register unsigned long int *pFG, *pBG ;
    register unsigned long int *psrcImage ;
    register unsigned long int *psrcMask ;
    register int i ;
    register unsigned long int endbits ;
    int srcWidth ;
    int srcHeight ;
    int srcRealWidth ;

    TRACE( ( "mpelRealizeCursor(pScr=0x%x,pCurs=0x%x)\n", pScr, pCurs) ) ;
    if ( !( pCurs->devPriv[ pScr->myNum ] = (pointer) Xalloc( 256 ) ) )
	{
	ErrorF( "mpelRealizeCursor: can't malloc\n" ) ;
	return FALSE ;
	}
    pFG = (unsigned long int *) pCurs->devPriv[pScr->myNum] ;
    bzero( (char *) pFG, 256 ) ;
    pBG = (unsigned long int *) pFG + 32 ; /* words */
    psrcImage = (unsigned long int *) pCurs->bits->source ;
    psrcMask = (unsigned long int *) pCurs->bits->mask ;
    srcRealWidth = ( pCurs->bits->width + 31 ) / 32 ;

    srcWidth = MIN( pCurs->bits->width, 32 ) ;
    srcHeight = MIN( pCurs->bits->height, 32 ) ;

    endbits = -1 << ( 32 - srcWidth ) ;

    for ( i = srcHeight ; i-- ; )
	{
	*pFG++ = (*psrcMask & endbits) & *psrcImage ;
	*pBG++ = (*psrcMask & endbits) & ~ *psrcImage ;
	psrcImage = psrcImage + srcRealWidth ;
	psrcMask = psrcMask + srcRealWidth ;
	}

    if ( ! ibmCurrentCursor( pScr->myNum ) ) {
	ibmCursorHotX( i ) = pCurs->bits->xhot ;
	ibmCursorHotY( i ) = pCurs->bits->yhot ;
	ibmCurrentCursor( pScr->myNum ) = pCurs ;
	cursor_is_valid = TRUE ;
    }

    TRACE( ( "exiting mpelRealizeCursor\n" )) ;
    return TRUE ;
}

/*** ============================================================***/

Bool
mpelUnrealizeCursor( pScr, pCurs )
    register ScreenPtr 	pScr ;
    register CursorPtr 	pCurs ;
{

    TRACE( ("mpelUnrealizeCursor(pScr=0x%x,pCurs=0x%x)\n", pScr, pCurs )) ;

    Xfree( (char *) pCurs->devPriv[ pScr->myNum ] ) ;
    pCurs->devPriv[ pScr->myNum ] = 0 ;
    if ( ibmCurrentCursor( pScr->myNum ) == pCurs )
	cursor_is_valid = FALSE ;
    return TRUE ;
}

void
mpelColorCursor( fg, bg )
register unsigned long int fg, bg ;
{
TRACE( ( "mpelColorCursor( %d, %d )\n", fg, bg )) ;
if ( fg > 255 || bg > 255 )
    return ;

c_fg = fg ;
c_bg = bg ;

return ;
}

/*** ============================================================***/

Bool
mpelDisplayCursor( pScr, pCurs )
    ScreenPtr	pScr ;
    CursorPtr	pCurs ;
{
    register CursorPtr curCursor = ibmCurrentCursor( pScr->myNum ) ;
    register unsigned short int *srcFG, *srcBG, *dstFG, *dstBG ;
    register int i ;

    TRACE( ( "mpelDisplayCursor(pScr=0x%x,pCurs=0x%x)\n", pScr, pCurs )) ;

    /* cleanup old cursor */

    if ( cursor_is_valid ) {
	if ( !cursor_not_displayed )
	    (void) mpelRemoveCursor() ;
	curCursor = ibmCurrentCursor( pScr->myNum ) ;
	c_x += curCursor->bits->xhot ;
	c_y += curCursor->bits->yhot ;
    }

    i = pScr->myNum ;
    ibmCurrentCursor( i ) = curCursor = pCurs ;
    ibmCursorHotX( i ) = pCurs->bits->xhot ;
    ibmCursorHotY( i ) = pCurs->bits->yhot ;

    srcFG = (unsigned short int *) pCurs->devPriv[ pScr->myNum ] ;
    srcBG = srcFG + ( CURSOR_MAX_SIZE / 2 ) ;
    dstFG = (unsigned short int *) forePtrn ;
    dstBG = (unsigned short int *) backPtrn ;
    for ( i = MIN( curCursor->bits->height, 32 ) ; i-- ; ) {
	*dstFG++ = *srcFG++ ;
	*dstBG++ = *srcBG++ ;
	if ( ( curCursor->bits->width > 16 ? 32 : 16 ) > 16 ) {
	    *dstFG++ = *srcFG ;
	    *dstBG++ = *srcBG ;
	}
	srcFG++ ;
	srcBG++ ;
    }

    mpelRecolorCursor(
	( (ppcScrnPriv *) pScr->devPrivate )->InstalledColormap ) ;

    c_x -= curCursor->bits->xhot ;
    c_y -= curCursor->bits->yhot ;
    mpelPutCursorOn( c_x, c_y ) ;
    return cursor_is_valid = TRUE ;
}

/*** ==================================================================***/

void
mpelRevalidateCursor()
{
    cursor_not_displayed = FALSE ;
    if ( cursor_is_valid )
	mpelPutCursorOn( c_x, c_y ) ;
    return ;
}
