/*******************************************************************
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

************************************************************************/
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

/* $Header: mpelText.c,v 1.2 89/12/07 20:35:03 keith Exp $ */
/* $Source: /xsrc/mit/server/ddx/ibm/mpel/RCS/mpelText.c,v $ */

#ifndef lint
static char *rcsid = "$Header: mpelText.c,v 1.2 89/12/07 20:35:03 keith Exp $" ;
#endif

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"region.h"
#include	"regionstr.h"

#include 	"OScompiler.h"

#include	"ibmScreen.h"

#include	"ppc.h"
#include	"ppcProcs.h"

#include	"mpel.h"
#include	"mpelHdwr.h"
#include	"mpelFifo.h"
#include	"mpelFont.h"

#include	"ibmTrace.h"

#if !defined(NO_FUNCTION_PROTOTYPES)
extern void mpelRemap(
    mpelXFont		*xFont,
    int			 nCh,
    unsigned 	short	*inStr,
    unsigned	char	*outStr,
    CharInfoPtr 	*ppci,		/* array of character info */
    unsigned	char	*pglyphBase	/* start of array of glyphs */
) ;
#else
extern void mpelRemap() ;
#endif

/* Cursor Stuff */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

/***==================================================================***/

typedef struct {
	void		(*bltfun)() ;
	DrawablePtr	pDraw ;
	GC		*pGC ;
	int		 baseline ;
	char		*pglyphs ;
} glyphBltData ;

#define	callGlyphBlt(g,x,n,ppci) \
	 ((*(g)->bltfun)((g)->pDraw,(g)->pGC,(x),\
		(g)->baseline-((g)->pDraw)->y,(n),\
		(ppci),(g)->pglyphs))

/* text requests are broken in to MAX_CHARS_PER_CALL chunks for output */
#define MAX_CHARS_PER_CALL 80

static int
mpelClipString(gblt,pbox,n,charinfo,brect,txt)
    glyphBltData	*gblt ;
    BoxPtr		 pbox ;
    int			 n ;
    CharInfoPtr		*charinfo ;
    xRectangle		*brect ;
    mpelAnnotationText	*txt ;
{
    int			x,end,start,i,xCoord ;

    TRACE(("mpelClipString(...)\n")) ;

	/*
	 * Don't even try to deal with characters clipped horizontally,
	 * just call GlyphBlt to deal with it.
	 */
    if ((pbox->y1>(brect->y))||(pbox->y2<(brect->y+brect->height+1))) {
	xCoord= txt->point.x-(gblt->pDraw)->x ;
	callGlyphBlt(gblt,xCoord,n,charinfo) ;
	return FALSE ;
    }

	/*
	 * Find first unclipped character in txt 
	 * (left edge of character >= pbox->x1)
	 * We know we're dealing with fixed width fonts, so at most
	 * one character is partially clipped.  Draw partially clipped
	 * character with GlyphBlt, and note new starting x in txt.
	 */
    i= 0 ;
    x= txt->point.x ;
    while ((i<n)&&(x<pbox->x1)) {
	x+= charinfo[i]->metrics.characterWidth ;
	i++ ;
    } 
    if ( i > 0 ) { /* Some characters clipped out */
	xCoord = x - charinfo[i-1]->metrics.characterWidth ;
	xCoord -= (  gblt->pDraw )->x ;
	callGlyphBlt( gblt, xCoord, 1, &charinfo[i-1] ) ;
	txt->point.x = x ;
	brect->width -=	x - brect->x ;
	brect->x = x ;
    }
    start = i ;

	/*
	 * Find first clipped character in (adjusted) txt
	 * (right edge of character >= pbox->x2
	 * Draw partially clipped character (at most 1) with
	 * glyphBlt, and note the new length of txt.
	 */
    while ( ( i < n )
	 && ( x + charinfo[i]->metrics.characterWidth < pbox->x2 ) ) {
	x += charinfo[i]->metrics.characterWidth ;
	i++ ;
    } 
    if ( i < n ) { /* Didn't reach the end of the string, last char clipped */
	brect->width = x - brect->x ;
	xCoord = x - ( gblt->pDraw )->x ;
	callGlyphBlt( gblt, xCoord, 1, &charinfo[i] ) ;
    }
    end = i ;

	/*
	 * fix up txt to contain only unclipped characters
	 */
    if ( !( txt->length = end - start ) ) /* no unclipped characters */
	return FALSE ;

	/*
	 * fix up string
	 */
    for ( i = 0 ; i < txt->length ; i++ )
	txt->string[ i ] = txt->string[ start + i ] ;

	/*
	 * fix up background rectangle
	 */
    brect->x = MAX( pbox->x1, brect->x ) ;
    brect->y = MAX( pbox->y1, brect->y ) ;
    brect->width = MIN( brect->x + brect->width, pbox->x2 ) - brect->x ;
    brect->height= MIN( brect->y + brect->height, pbox->y2 ) - brect->y ;
    return TRUE ;
}

/***==================================================================***/

int
mpelImageText( pDraw, pGC, x, y, count, chars, fontEncoding )
    DrawablePtr 	 pDraw ;
    GCPtr		 pGC ;
    int			 x, y ;
    int			 count ;
    char		*chars ;
    FontEncoding	 fontEncoding ;
{
    int cursor_saved ;
    glyphBltData	 gblt ;
    CharInfoPtr		*charinfo ;
    unsigned int	 n ;
    FontPtr 		 font = pGC->font ;
    ExtentInfoRec	 info ;
    int 		 i,nbox ;
    BoxPtr 		 pbox ;
    RegionPtr 		 pRegion ;
    xRectangle		 backrect,tmpBackrect ;
    BoxRec 		 bbox ;
    int			 allIn= FALSE ;
    mpelXFont 		*xFont ;
    mpelRectangle 	*brect,wholeBrect,partBrect ;
    mpelAnnotationText	*txt,wholeTxt,partTxt ;
    int			 ht ;
    char		 wholeStr[MAX_CHARS_PER_CALL] ;
    char		 partStr[MAX_CHARS_PER_CALL] ;
    CARD16		*charsOut ;
    int			bot_edge;	/* Does bottom edge of screen clip ? */

    TRACE(("mpelImageText( 0x%x, 0x%x, (%d,%d), %d, 0x%x)\n", pDraw, pGC, x, y, count, chars,fontEncoding)) ;

	/*
	 * if priv<40, font was not realized by mpelRealizeFont,
	 * if not a window, or merge mode not copy, the hardware can't 
	 * help us.
	 * (I guess we *could* write an glyph blt that uses blt and
	 *  the character as stored on the adapter....)
	 */

    if  ( ( ( (CARD32) font->devPriv[pDraw->pScreen->myNum] ) < 40 )
      || ( pDraw->type != DRAWABLE_WINDOW )
      || ( pGC->alu != GXcopy ) 
      || ( pGC->planemask == 0 ) ) 
	return miImageText( pDraw, pGC, x, y, count, chars, fontEncoding ) ;

    while ( count > MAX_CHARS_PER_CALL ) { /* Hardware Limit ??? */
	x= mpelImageText(pDraw,pGC,x,y,MAX_CHARS_PER_CALL,chars,fontEncoding) ;
	count -= MAX_CHARS_PER_CALL ;
	chars += MAX_CHARS_PER_CALL ;
    }

    xFont= (mpelXFont *) font->devPriv[pDraw->pScreen->myNum] ;

    x += pDraw->x ;
    y += pDraw->y ;

    if ( !( charinfo = (CharInfoPtr *)
		ALLOCATE_LOCAL( count * sizeof (CharInfoPtr) ) ) )
	return x ;
    if ( !( charsOut= (CARD16 *)
		ALLOCATE_LOCAL( count * sizeof (CARD16) ) ) ) {
	DEALLOCATE_LOCAL(charinfo) ;
	return x ;
    }

    GetGlyphsCharsAndExtents( font,count,chars,fontEncoding,
					&n,charinfo,charsOut,&info ) ;

    if ((count= n)==0)
	return x ;
    if (ibmScreenState(pDraw->pScreen->myNum)!=SCREEN_ACTIVE)
	return x+info.overallWidth ;

    backrect.x =	x + info.overallLeft ;
    backrect.y =	y - font->pFI->fontAscent ;
    backrect.width =	MAX( info.overallWidth,
			     info.overallRight - info.overallLeft ) ;
    backrect.height =	font->pFI->fontAscent + font->pFI->fontDescent ;

    bbox.x1 = x + info.overallLeft ;
    bbox.x2 = x + info.overallRight ;
    bbox.y1 = y - info.overallAscent ;
    bbox.y2 = y + info.overallDescent ;

    pRegion = ( (ppcPrivGC *) (pGC->devPrivates[mfbGCPrivateIndex].ptr) )->pCompositeClip ;
    nbox = REGION_NUM_RECTS(pRegion) ;
    if ( nbox == 0 ) 
	return x+info.overallWidth ;
    pbox = REGION_RECTS(pRegion) ;

	/* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore &&
		   ( mpelCheckCursor(	bbox.x1, bbox.y1, 
					bbox.x2-bbox.x1+1, bbox.y2-bbox.y1+1)||
    		     mpelCheckCursor(   backrect.x,backrect.y,
					backrect.width,backrect.height) ) ;

    MPELSetPgonInteriorColor( pGC->bgPixel ) ;
    MPELSetTextColor( pGC->fgPixel ) ;
    MPELSetTextFont( xFont->mpelId ) ;
    mpelSetPlaneMask( pGC->planemask ) ;

    mpelRemap( xFont, n, charsOut, wholeStr, charinfo, pGC->font->pGlyphs ) ;

    wholeBrect.uright.x =	backrect.x + backrect.width - 1 ;
    wholeBrect.uright.y =	MPEL_HEIGHT - 1 - backrect.y ;
    wholeBrect.lleft.x =	backrect.x ;
    wholeBrect.lleft.y =	MPEL_HEIGHT - ( backrect.y + backrect.height ) ;

    ht = ( ( ( fontHeight( xFont ) + 3 ) / 4 ) * 4 ) - fontHeight( xFont ) ;
    wholeTxt.point.x =	x ;
    bot_edge = ((wholeTxt.point.y = MPEL_HEIGHT - y - font->pFI->fontDescent - ht) < 0);
    wholeTxt.reserved =	0 ;
    wholeTxt.length =	n ;
    wholeTxt.string =	wholeStr ;

    setFontDimensions( xFont ) ;

    if(bot_edge)
      {
	TRACE(("Bottom edge effects in mpelImageText (ht = %d).\n", ht));
	backrect.height += ht;
	bbox.y2 += ht;
      }

    for ( i = 0 ; ( i < nbox ) && !allIn ; i++, pbox++ ) {
	switch ( mpelRectIntersect( pbox, &bbox ) ) {
	    case rgnOUT:
		continue ;
	    case rgnPART:
		MOVE( &backrect, &tmpBackrect, sizeof backrect ) ;
		MOVE( &wholeTxt, &partTxt, sizeof wholeTxt ) ;
		MOVE( wholeStr, partStr, count) ;
		partTxt.string= partStr ;
		gblt.bltfun=	ppcImageGlyphBlt ;
		gblt.pDraw=	pDraw ;
		gblt.pGC=	pGC ;
		gblt.baseline=	y ;
		gblt.pglyphs=	pGC->font->pGlyphs ;
		if ( !mpelClipString( &gblt, pbox, n, charinfo,
				      &tmpBackrect, &partTxt ) )
		    continue ;
		else {
		    partBrect.uright.x = tmpBackrect.x + tmpBackrect.width - 1 ;
		    partBrect.uright.y = MPEL_HEIGHT - 1 - tmpBackrect.y ;
		    partBrect.lleft.x = tmpBackrect.x ;
		    partBrect.lleft.y = MPEL_HEIGHT
				      - ( tmpBackrect.y + tmpBackrect.height ) ;
		    brect = &partBrect ;
		    txt = &partTxt ;
		}
		break ;
	    case rgnIN:
		allIn = TRUE ;
		brect = &wholeBrect ;
		txt =	&wholeTxt ;
		break ;
	}
	mpelSetALU( GXcopy ) ;
	MPELFillRectangle( brect ) ;
	mpelSetALU( pGC->alu ) ;
	MPELAnnotationText( txt->length, txt ) ;
	MPELSendData( txt->length, txt->string ) ;
    }

    DEALLOCATE_LOCAL( charinfo ) ;
    if ( cursor_saved )
	mpelReplaceCursor() ;

    return bbox.x2 - pDraw->x ;
}

/***==================================================================***/

int
mpelPolyText( pDraw, pGC, x, y, count, chars, fontEncoding )
    DrawablePtr 	pDraw ;
    GCPtr		pGC ;
    int			x, y ;
    int			count ;
    char		*chars ;
    FontEncoding	 fontEncoding ;
{
    int cursor_saved ;
    glyphBltData	 gblt ;
    CharInfoPtr		*charinfo ;
    unsigned int	 n ;
    FontPtr 		 font = pGC->font ;
    ExtentInfoRec	 info ;
    int 		 i,nbox,xOffset ;
    BoxPtr 		 pbox ;
    RegionPtr 		 pRegion ;
    BoxRec 		 bbox ;
    xRectangle		 bRect ;
    int			 allIn= FALSE ;
    mpelXFont 		*xFont ;
    mpelAnnotationText	*txt,wholeTxt,partTxt ;
    int			ht ;
    char		wholeStr[MAX_CHARS_PER_CALL] ;
    char		partStr[MAX_CHARS_PER_CALL] ;
    CARD16		*charsOut ;

    TRACE(("mpelPolyText( 0x%x, 0x%x, (%d,%d), %d, 0x%x, 0x%x)\n", pDraw, pGC, x, y, count, chars, fontEncoding)) ;

	/*
	 * if priv<40, font was not realized by mpelRealizeFont,
	 * if not a window, or merge mode not copy, the hardware can't 
	 * help us.
	 * (I guess we *could* write an glyph blt that uses blt and
	 *  the character as stored on the adapter....)
	 */

    if  ((((CARD32)font->devPriv[pDraw->pScreen->myNum]) < 40 )||
	 	(pDraw->type!=DRAWABLE_WINDOW)||(pGC->alu!=GXcopy)||
		(pGC->planemask==0)||(pGC->fillStyle != FillSolid))
	return miPolyText(pDraw, pGC, x, y, count, chars, fontEncoding) ;

    while ( count > MAX_CHARS_PER_CALL ) { /* Hardware Limit ??? */
	x= mpelPolyText(pDraw,pGC,x,y,MAX_CHARS_PER_CALL,chars,fontEncoding) ;
	count -= MAX_CHARS_PER_CALL ;
	chars += MAX_CHARS_PER_CALL ;
    }

    xFont= (mpelXFont *)font->devPriv[pDraw->pScreen->myNum] ;

    x += (xOffset=pDraw->x) ;
    y += pDraw->y ;

    if(!(charinfo = (CharInfoPtr *)ALLOCATE_LOCAL(count*sizeof(CharInfoPtr))))
	return x-xOffset ;
    if ( !( charsOut= (CARD16 *)
		ALLOCATE_LOCAL( count * sizeof (CARD16) ) ) ) {
	DEALLOCATE_LOCAL(charinfo) ;
	return x-xOffset ;
    }

    GetGlyphsCharsAndExtents(font, count, chars, fontEncoding, 
				&n, charinfo, charsOut, &info) ;
	
    if ((count=n) == 0)
	return x-xOffset ;

    if (ibmScreenState(pDraw->pScreen->myNum)!=SCREEN_ACTIVE) {
	return x+info.overallWidth-xOffset ;
    }

    bbox.x1 = x + info.overallLeft ;
    bbox.x2 = x + info.overallRight ;
    bbox.y1 = y - info.overallAscent ;
    bbox.y2 = y + info.overallDescent ;


    pRegion = ((ppcPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip ;
    nbox = REGION_NUM_RECTS(pRegion) ;
    if ( nbox == 0 )
	return x+info.overallWidth-xOffset ;

    pbox = REGION_RECTS(pRegion) ;

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore
    	    && mpelCheckCursor( bbox.x1, bbox.y1,
				bbox.x2 - bbox.x1 + 1, bbox.y2 - bbox.y1 + 1 ) ;

    MPELSetTextColor( pGC->fgPixel ) ;
    MPELSetTextFont(xFont->mpelId) ;
    mpelSetALU(pGC->alu) ;
    mpelSetPlaneMask(pGC->planemask) ;

    mpelRemap(xFont,n,charsOut,wholeStr,charinfo,pGC->font->pGlyphs) ;

    ht= (((fontHeight(xFont)+3)/4)*4)-fontHeight(xFont) ;
    wholeTxt.point.x=	x ;
    wholeTxt.point.y=	(1023-y)-font->pFI->fontDescent-ht+1 ;
    wholeTxt.reserved=	0 ;
    wholeTxt.length=	n ;
    wholeTxt.string=	wholeStr ;

    setFontDimensions(xFont) ;

    for (i=0 ;(i<nbox)&&(!allIn) ;i++,pbox++) {
	switch (mpelRectIntersect(pbox,&bbox)) {
	    case rgnOUT:
		continue ;
	    case rgnPART:
		bRect.x= bbox.x1 ; bRect.y= bbox.y1 ;
		bRect.width= bbox.x2-bbox.x1-1 ;
		bRect.height= bbox.y2-bbox.y1-1 ;
		MOVE( &wholeTxt, &partTxt, sizeof wholeTxt ) ;
		MOVE( wholeStr, partTxt.string = partStr, n ) ;
		gblt.bltfun=	ppcPolyGlyphBlt ;
		gblt.pDraw=	pDraw ;
		gblt.pGC=	pGC ;
		gblt.baseline=	y ;
		gblt.pglyphs=	pGC->font->pGlyphs ;
		if (!mpelClipString(&gblt,pbox,n,charinfo,&bRect,&partTxt))
		    continue ;
		else 
		    txt= &partTxt ;
		break ;
	    case rgnIN:
		allIn= 	TRUE ;
		txt=	&wholeTxt ;
		break ;
	}
	MPELAnnotationText(txt->length,txt) ;
	MPELSendData(txt->length,txt->string) ;
    }
    DEALLOCATE_LOCAL(charinfo) ;
    if ( cursor_saved )
	mpelReplaceCursor() ;

    return x+info.overallWidth-xOffset ;
}

/***====================================================================***/

int
mpelPolyText8( pDraw, pGC, x, y, count, chars )
DrawablePtr 	 pDraw ;
GCPtr		 pGC ;
int		 x, y ;
int		 count ;
char		*chars ;
{
    TRACE(("mpelPolyText8( 0x%x, 0x%x, (%d,%d), %d, 0x%x )\n",
	pDraw,pGC,x,y,count,chars)) ;
    return mpelPolyText( pDraw, pGC, x, y, count, chars,  Linear8Bit ) ;
}

/***====================================================================***/

int
mpelPolyText16( pDraw, pGC, x, y, count, chars )
DrawablePtr 	 pDraw ;
GCPtr		 pGC ;
int		 x, y ;
int		 count ;
char		*chars ;
{
    TRACE(("mpelPolyText16( 0x%x, 0x%x, (%d,%d), %d, 0x%x )\n",
	pDraw,pGC,x,y,count,chars)) ;
    return mpelPolyText( pDraw, pGC, x, y, count, chars,
			 ( pGC->font->pFI->lastRow )
			 ? TwoD16Bit : Linear16Bit ) ;
}

/***====================================================================***/

void
mpelImageText8( pDraw, pGC, x, y, count, chars )
DrawablePtr 	 pDraw ;
GCPtr		 pGC ;
int		 x, y ;
int		 count ;
char		*chars ;
{
    TRACE(("mpelImageText8( 0x%x, 0x%x, (%d,%d), %d, 0x%x )\n",
	pDraw,pGC,x,y,count,chars)) ;
    mpelImageText( pDraw, pGC, x, y, count, chars,  Linear8Bit ) ;
    return ;
}

/***====================================================================***/

void
mpelImageText16( pDraw, pGC, x, y, count, chars )
DrawablePtr 	 pDraw ;
GCPtr		 pGC ;
int		 x, y ;
int		 count ;
char		*chars ;
{
    TRACE(("mpelImageText16( 0x%x, 0x%x, (%d,%d), %d, 0x%x )\n",
	pDraw,pGC,x,y,count,chars)) ;

    mpelImageText( pDraw, pGC, x, y, count, chars,
		   ( pGC->font->pFI->lastRow )
		   ? TwoD16Bit : Linear16Bit ) ;
    return ;
}
