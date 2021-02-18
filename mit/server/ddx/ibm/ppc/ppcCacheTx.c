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
#include "X.h"
#include "Xmd.h"
#include "Xproto.h"

#include "scrnintstr.h"
#include "dixfontstr.h"
#include "fontstruct.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "windowstr.h"

#include "ppcCache.h"
#include "ppc.h"
#include "ppcProcs.h"
#include "OScompiler.h"
extern int mfbGCPrivateIndex;

static unsigned int
miWidth(n, charinfo)
    register unsigned int n;
    register CharInfoPtr charinfo[];
{
    register unsigned int w = 0;

    while ( n-- )
	w += charinfo[n]->metrics.characterWidth ;

    return w;
}


ppcBMInfoPtr
ppcFaultChar(cacheInfo, pci, pglyphBase)
	ppcCacheInfoPtr cacheInfo;
	CharInfoPtr	pci;
	char 		*pglyphBase;
	{
	char *pglyph;
	ppcBMInfoPtr bm;

	pglyph = pglyphBase + pci->byteOffset;

	if ( ( bm = newBM(cacheInfo, GLYPHWIDTHPIXELS(pci), GLYPHHEIGHTPIXELS(pci) ) ) == NULL )
		return((ppcBMInfoPtr)NULL);

	(* cacheInfo->blitToCache)( pglyph, bm );

	return(bm);

	}

/*
 * ppcGetGlyphs - ppc version of GetGlyphs routine.
 */

void
ppcGetGlyphs(pScreen, font, count, chars, fontEncoding, glyphcount, glyphs, bms)
    ScreenPtr pScreen;
    FontPtr font;
    unsigned long count;
    register unsigned char *chars;
    FontEncoding fontEncoding;
    unsigned long *glyphcount;	/* RETURN */
    CharInfoPtr glyphs[];	/* RETURN */
    ppcBMInfoPtr bms[];		/* RETURN */
{
    CharInfoPtr		pCI = font->pCI;
    FontInfoPtr		pFI = font->pFI;
    unsigned int	firstCol = pFI->firstCol;
    unsigned int	numCols = pFI->lastCol - firstCol + 1;
    unsigned int	firstRow = pFI->firstRow;
    unsigned int	numRows = pFI->lastRow - firstRow + 1;
    unsigned int	chDefault = pFI->chDefault;
    unsigned int	cDef = chDefault - firstCol;
    register unsigned long	i;
    unsigned long		n;
    register unsigned int	c;
    register CharInfoPtr	ci;
    ppcBMInfoPtr		*bmtable;
    ppcCacheInfoPtr		cacheInfo;
    char 			*pglyphBase;

    bmtable = (ppcBMInfoPtr *)font->devPriv[pScreen->myNum];
    cacheInfo = ((ppcScrnPrivPtr)pScreen->devPrivate)->cacheInfo;
    pglyphBase = font->pGlyphs;

    n = 0;
    switch (fontEncoding) {

	case Linear8Bit:
	case TwoD8Bit:
	    if (pFI->allExist && (cDef < numCols)) {
		for (i=0; i < count; i++) {

		    c = (*chars++) - firstCol;
		    if (c >= numCols) {
			c = cDef;
		    }
		    ci = &pCI[c];
		    glyphs[i] = ci;
		    if ( ( bms[i] = bmtable[c] ) == NULL )
			bmtable[c] = bms[i] = 
					ppcFaultChar(cacheInfo, ci, pglyphBase);
		}
		n = count;
	    } else {
		for (i=0; i < count; i++) {
    
		    c = (*chars++) - firstCol;
		    if (c < numCols) {
			ci = &pCI[c];
			if (ci->exists) {
			    glyphs[n] = ci;
			    if ( ( bms[n] = bmtable[c] ) == NULL )
				bmtable[c] = bms[n++] = 
					ppcFaultChar(cacheInfo, ci, pglyphBase);
			    else
				n++;
			    continue;
			}
		    }
    
		    if (cDef < numCols) {
			ci = &pCI[cDef];
			if (ci->exists) {
			    glyphs[n] = ci;
			    if ( ( bms[n] = bmtable[cDef] ) == NULL )
				bmtable[cDef] = bms[n++] = 
					ppcFaultChar(cacheInfo, ci, pglyphBase);
			    else
				n++;
			}
		    }
		}
	    }
	    break;

	case Linear16Bit:
	    if (pFI->allExist && (cDef < numCols)) {
		for (i=0; i < count; i++) {

		    c = *chars++ << 8;
		    c = (c | *chars++) - firstCol;
		    if (c >= numCols) {
			c = cDef;
		    }
		    ci = &pCI[c];
		    glyphs[i] = ci;
		    if ( ( bms[i] = bmtable[c] ) == NULL )
			bmtable[c] = bms[i] = 
					ppcFaultChar(cacheInfo, ci, pglyphBase);
		}
		n = count;
	    } else {
		for (i=0; i < count; i++) {
    
		    c = *chars++ << 8;
		    c = (c | *chars++) - firstCol;
		    if (c < numCols) {
			ci = &pCI[c];
			if (ci->exists) {
			    glyphs[n] = ci;
			    if ( ( bms[n] = bmtable[c] ) == NULL )
				bmtable[c] = bms[n++] = 
					ppcFaultChar(cacheInfo, ci, pglyphBase);
			    else
				n++;
			    continue;
			}
		    }
    
		    if (cDef < numCols) {
			ci = &pCI[cDef];
			if (ci->exists) {
			    glyphs[n] = ci;
			    if ( ( bms[n] = bmtable[cDef] ) == NULL )
				bmtable[cDef] = bms[n++] = 
					ppcFaultChar(cacheInfo, ci, pglyphBase);
			    else
				n++;
			}
		    }
		}
	    }
	    break;

	case TwoD16Bit:
	    for (i=0; i < count; i++) {
		register unsigned int row;
		register unsigned int col;

		row = (*chars++) - firstRow;
		col = (*chars++) - firstCol;
		if ((row < numRows) && (col < numCols)) {
		    c = row*numCols + col;
		    ci = &pCI[c];
		    if (ci->exists) {
			glyphs[n] = ci;
			if ( ( bms[n] = bmtable[c] ) == NULL )
			    bmtable[c] = bms[n++] = 
					ppcFaultChar(cacheInfo, ci, pglyphBase);
			else
			    n++;
			continue;
		    }
		}

		row = (chDefault >> 8)-firstRow;
		col = (chDefault & 0xff)-firstCol;
		if ((row < numRows) && (col < numCols)) {
		    c = row*numCols + col;
		    ci = &pCI[c];
		    if (ci->exists) {
			glyphs[n] = ci;
			if ( ( bms[n] = bmtable[c] ) == NULL )
			    bmtable[c] = bms[n++] = 
					ppcFaultChar(cacheInfo, ci, pglyphBase);
			else
			    n++;
		    }
		}
	    }
	    break;
    }
    *glyphcount = n;
}

int
ppcCachedImageText(pDraw, pGC, x, y, count, chars, fontEncoding)
	DrawablePtr pDraw;
	GCPtr pGC;
	int x, y;
	int count;
	char *chars;
	FontEncoding fontEncoding;
	{
	CharInfoPtr *charinfo;
	ppcBMInfoPtr *bms;
	unsigned long n;
	FontPtr font = pGC->font;
	unsigned int w;
	ppcCacheInfoPtr		cacheInfo;

	cacheInfo = ((ppcScrnPrivPtr)pDraw->pScreen->devPrivate)->cacheInfo;
	if ( ! (charinfo = (CharInfoPtr *)ALLOCATE_LOCAL(
						count * sizeof(CharInfoPtr))))
		return(x);
	if ( ! (bms = (ppcBMInfoPtr *)ALLOCATE_LOCAL(
						count * sizeof(ppcBMInfoPtr))))
		{
		DEALLOCATE_LOCAL(charinfo);
		return(x);
		}
	ppcGetGlyphs(pDraw->pScreen, font, (unsigned long)count, 
		(unsigned char *)chars, fontEncoding, &n, charinfo, bms);

	w = miWidth(n, charinfo);
	if ( n != 0 )
		{
		(* ((ppcPrivGCPtr) pGC->devPrivates[mfbGCPrivateIndex].ptr )->cachedIGBlt)(pDraw, pGC, x, y, n, charinfo, bms, font->pGlyphs);
		}
	DEALLOCATE_LOCAL(charinfo);
	DEALLOCATE_LOCAL(bms);

	return x+w;
	}

void
ppcCachedImageGlyphBlt( pDrawable, pGC, x, y, nglyph, ppci, bms, pglyphBase )
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int 	x, y ;
    unsigned int nglyph ;
    CharInfoPtr *ppci ;		/* array of character info */
    ppcBMInfoPtr *bms ;
    unsigned char *pglyphBase ;	/* start of array of glyphs */
{
    register unsigned char	*pglyph ;
    register BoxPtr		pbox ;
    register int		nbox ;
    ExtentInfoRec 		info ;	
    BoxRec 			bbox ;	
    xRectangle 			backrect ;
    CharInfoPtr  		pci ;
    FontInfoPtr			pfi ;
    int 			fg, bg, pm, ax,ay,zx,zy,zw,zh,
 		   		w, h, widthGlyph ;

    RegionPtr 			pRegion ;

    PixmapPtr			pTempPixmap = NULL ;
    ScreenPtr			pScreen ;
    ppcScrnPriv			*pScrPriv ;
    int				been_here = FALSE ;
    void			(*rectFunc)(), (*glyphFunc)(), (*stipFunc)() ;
    int				CursorIsSaved ;
    ppcBMInfoPtr		bm;
    ppcCacheInfoPtr		cacheInfo;

    pRegion = ((ppcPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip ; 
    if ( !( nbox = REGION_NUM_RECTS(pRegion) ) )
	return ;

    pScreen = pGC->pScreen ;
    pScrPriv = (ppcScrnPriv *) pScreen->devPrivate ;
    cacheInfo = pScrPriv->cacheInfo;
    pfi = pGC->font->pFI;

    rectFunc = pScrPriv->solidFill ;
    stipFunc = pScrPriv->stipFill ;
    glyphFunc = pScrPriv->glyphFill ;

    QueryGlyphExtents(pGC->font, ppci, nglyph, &info) ;

    x += pDrawable->x ;
    y += pDrawable->y ;

    backrect.y = y - pfi->fontAscent ;
    backrect.height = pfi->fontAscent + pfi->fontDescent ;
    backrect.width =
	 MAX( ( info.overallRight - info.overallLeft ), info.overallWidth ) ;
    backrect.x = x + info.overallLeft ;

    /* High Level Software Cursor Support */
    CursorIsSaved = !(*(pScrPriv->CursorSemaphore))
		 && (* pScrPriv->CheckCursor)( backrect.x, backrect.y,
					       backrect.width,
					       backrect.height ) ;
    (*(pScrPriv->CursorSemaphore))++ ;

    bbox.x1 = x + info.overallLeft ;
    bbox.x2 = x + info.overallRight ;
    bbox.y1 = y - info.overallAscent ;
    bbox.y2 = y + info.overallDescent ;

    pm =  pGC->planemask ;
    bg =  pGC->bgPixel ;
    for ( pbox = REGION_RECTS(pRegion); nbox-- ; pbox++ ) {
	ax = MAX( pbox->x1, backrect.x ) ;
	ay = MAX( pbox->y1, backrect.y ) ;
	zx = MIN( pbox->x2, backrect.x + backrect.width ) ;
	zy = MIN( pbox->y2, backrect.y + backrect.height ) ;
	if ( ( ( zw = zx - ax ) > 0 ) && ( ( zh = zy - ay ) > 0 ) )
	    (* rectFunc)( bg, GXcopy, pm, ax, ay, zw, zh ) ;
    }

    fg =  pGC->fgPixel ;

    switch ( (* pScreen->RectIn)( pRegion, &bbox ) ) {
      case rgnIN:
        while ( nglyph-- ) {
	    pci = *ppci++ ;
	    if ( bm = *bms++ )
		{
		(* cacheInfo->blitToScreen)( bm, x + pci->metrics.leftSideBearing, y - pci->metrics.ascent, fg, GXcopy, pm );
		}
	    else
		{
		pglyph = pglyphBase + pci->byteOffset ;
		if ( ( w = GLYPHWIDTHPIXELS(pci) )
		  && ( h = GLYPHHEIGHTPIXELS(pci) ) )
		    (*glyphFunc)( pglyph,
				  x + pci->metrics.leftSideBearing,
				  y - pci->metrics.ascent,
				  w, h, fg, GXcopy, pm ) ;
		}
	    x += pci->metrics.characterWidth ;	/* update character origin */
        } /* while nglyph-- */
      case rgnOUT:
	break ;
      case rgnPART:
      {
	RegionPtr prgnClip ;

	BoxRec cbox ;
	int glx, gly ;

        while ( nglyph-- )
        {
	    pci = *ppci++ ;
	    bm = *bms++ ;
	    pglyph = pglyphBase + pci->byteOffset ;
	    w = GLYPHWIDTHPIXELS(pci) ;
	    h = GLYPHHEIGHTPIXELS(pci) ;
	    cbox.x1 = glx = x + pci->metrics.leftSideBearing ;
	    cbox.x2 = cbox.x1 + w ;
	    cbox.y1 = gly = y - pci->metrics.ascent ;
	    cbox.y2 = cbox.y1 + h ;

	    switch ( (* pScreen->RectIn)( pRegion,  &cbox ) ) {
	      case rgnIN:
		if ( bm )
		    (* cacheInfo->blitToScreen)( bm, glx, gly, fg, GXcopy, pm );
		else
	    	    (* glyphFunc)( pglyph, glx, gly, w, h, fg, GXcopy, pm ) ;
	      case rgnOUT:
		break ;
	      case rgnPART:
		if ( !been_here )
			{
			widthGlyph = pfi->maxbounds.metrics.rightSideBearing-
				        pfi->minbounds.metrics.leftSideBearing;
			widthGlyph = PADGLYPHWIDTHBYTES(widthGlyph);
    			pTempPixmap = ppcCreatePixmap( pScreen, 
				widthGlyph * 8,
				pfi->maxbounds.metrics.ascent +
					pfi->maxbounds.metrics.descent ,
				1 ) ;
			been_here = TRUE ;
			}
		prgnClip = (* pScreen->RegionCreate)( &cbox,
						      REGION_NUM_RECTS(pRegion)) ;
		(* pScreen->Intersect)( prgnClip, prgnClip, pRegion ) ;
		if ( !( nbox = REGION_NUM_RECTS(prgnClip) ) ) {
		    (* pScreen->RegionDestroy)( prgnClip ) ;
		    break ;
		}
		widthGlyph = GLYPHWIDTHBYTESPADDED(pci) ;
	    	filltempPixmap(pTempPixmap, pglyph, widthGlyph, h) ;
		for ( pbox = REGION_RECTS(prgnClip); nbox-- ; pbox++ )
			{
			ax = MAX(pbox->x1, cbox.x1) ;
			ay = MAX(pbox->y1, cbox.y1) ;
			zx = MIN(pbox->x2, cbox.x2) ;
			zy = MIN(pbox->y2, cbox.y2) ;
	    		if ( ( ( zw = zx - ax ) > 0 )
	    		  && ( ( zh = zy - ay ) > 0 ) )
				(*stipFunc)( pTempPixmap, 
					     fg, GXcopy, pm, ax, ay,
					     zw, zh, glx, gly ) ;
			}
		(* pScreen->RegionDestroy)( prgnClip ) ;
		break ;
	    }
	    /* update character origin */
	    x += pci->metrics.characterWidth ;
        } /* while nglyph-- */

	if ( pTempPixmap )
	    (* pScreen->DestroyPixmap)( pTempPixmap ) ;

      }
      default:
	break ;
    }
    if ( !--(*(pScrPriv->CursorSemaphore)) && CursorIsSaved )
	(* pScrPriv->ReplaceCursor)() ;

    return ;
}

int
ppcCachedImageText8(pDraw, pGC, x, y, count, chars)
	DrawablePtr pDraw;
	GCPtr pGC;
	int x, y;
	int count;
	char *chars;
	{
	return ppcCachedImageText(pDraw, pGC, x, y, count, chars, Linear8Bit);
	}

int
ppcCachedImageText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         x, y;
    int         count;
    unsigned short *chars;
{
    if (pGC->font->pFI->lastRow == 0)
        return ppcCachedImageText(pDraw, pGC, x, y, count, (char *)chars, 
								Linear16Bit);
    else
        return ppcCachedImageText(pDraw, pGC, x, y, count, (char *)chars, 
								TwoD16Bit);
}


int
ppcCachedPolyText(pDraw, pGC, x, y, count, chars, fontEncoding)
	DrawablePtr pDraw;
	GCPtr pGC;
	int x, y;
	int count;
	char *chars;
	FontEncoding fontEncoding;
	{
	CharInfoPtr *charinfo;
	ppcBMInfoPtr *bms;
	unsigned long n;
	FontPtr font = pGC->font;
	unsigned int w;
	ppcCacheInfoPtr		cacheInfo;

	cacheInfo = ((ppcScrnPrivPtr)pDraw->pScreen->devPrivate)->cacheInfo;
	if ( ! (charinfo = (CharInfoPtr *)ALLOCATE_LOCAL(
						count * sizeof(CharInfoPtr))))
		return(x);
	if ( ! (bms = (ppcBMInfoPtr *)ALLOCATE_LOCAL(
						count * sizeof(ppcBMInfoPtr))))
		{
		DEALLOCATE_LOCAL(charinfo);
		return(x);
		}
	ppcGetGlyphs(pDraw->pScreen, font, (unsigned long)count, 
		(unsigned char *)chars, fontEncoding, &n, charinfo, bms);

	w = miWidth(n, charinfo);
	if ( n != 0 )
		{
		(* ((ppcPrivGCPtr) pGC->devPrivates[mfbGCPrivateIndex].ptr )->cachedPGBlt)(pDraw, pGC, x, y, n, charinfo, bms, font->pGlyphs);
		}
	DEALLOCATE_LOCAL(charinfo);
	DEALLOCATE_LOCAL(bms);

	return x+w;

	}


void
ppcCachedPolyGlyphBlt( pDrawable, pGC, x, y, nglyph, ppci, bms, pglyphBase )
    DrawablePtr pDrawable ;
    GCPtr	pGC ;
    int 	x, y ;
    unsigned int nglyph ;
    CharInfoPtr *ppci ;		/* array of character info */
    ppcBMInfoPtr *bms ;
    unsigned char *pglyphBase ;	/* start of array of glyphs */
{
    register unsigned char	*pglyph ;
    register BoxPtr		pbox ;
    register int		nbox ;
    ExtentInfoRec 		info ;	
    BoxRec 			bbox ;	
    xRectangle 			backrect ;
    CharInfoPtr  		pci ;
    FontInfoPtr			pfi ;
    int 			fg, pm, ax,ay,zx,zy,zw,zh,
 		   		w, h, widthGlyph ;

    RegionPtr 			pRegion ;

    PixmapPtr			pTempPixmap = NULL ;
    ScreenPtr			pScreen ;
    ppcScrnPriv			*pScrPriv ;
    int				been_here = FALSE ;
    void			(*rectFunc)(), (*glyphFunc)(), (*stipFunc)() ;
    int				CursorIsSaved ;
    ppcBMInfoPtr		bm;
    ppcCacheInfoPtr		cacheInfo;
    int				alu;

    pRegion = ((ppcPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip ; 
    if ( !( nbox = REGION_NUM_RECTS(pRegion) ) )
	return ;

    pScreen = pGC->pScreen ;
    pScrPriv = (ppcScrnPriv *) pScreen->devPrivate ;
    cacheInfo = pScrPriv->cacheInfo;
    pfi = pGC->font->pFI;

    rectFunc = pScrPriv->solidFill ;
    stipFunc = pScrPriv->stipFill ;
    glyphFunc = pScrPriv->glyphFill ;

    QueryGlyphExtents(pGC->font, ppci, nglyph, &info) ;

    x += pDrawable->x ;
    y += pDrawable->y ;

    backrect.y = y - pfi->fontAscent ;
    backrect.height = pfi->fontAscent + pfi->fontDescent ;
    backrect.width =
	 MAX( ( info.overallRight - info.overallLeft ), info.overallWidth ) ;
    backrect.x = x + info.overallLeft ;

    /* High Level Software Cursor Support */
    CursorIsSaved = !(*(pScrPriv->CursorSemaphore))
		 && (* pScrPriv->CheckCursor)( backrect.x, backrect.y,
					       backrect.width,
					       backrect.height ) ;
    (*(pScrPriv->CursorSemaphore))++ ;

    bbox.x1 = x + info.overallLeft ;
    bbox.x2 = x + info.overallRight ;
    bbox.y1 = y - info.overallAscent ;
    bbox.y2 = y + info.overallDescent ;

    pm =  pGC->planemask ;
    fg =  pGC->fgPixel ;
    alu = pGC->alu ;

    switch ( (* pScreen->RectIn)( pRegion, &bbox ) ) {
      case rgnIN:
        while ( nglyph-- ) {
	    pci = *ppci++ ;
	    if ( bm = *bms++ )
		{
		(* cacheInfo->blitToScreen)( bm, x + pci->metrics.leftSideBearing, y - pci->metrics.ascent, fg, alu, pm );
		}
	    else
		{
		pglyph = pglyphBase + pci->byteOffset ;
		if ( ( w = GLYPHWIDTHPIXELS(pci) )
		  && ( h = GLYPHHEIGHTPIXELS(pci) ) )
		    (*glyphFunc)( pglyph,
				  x + pci->metrics.leftSideBearing,
				  y - pci->metrics.ascent,
				  w, h, fg, alu, pm ) ;
		}
	    x += pci->metrics.characterWidth ;	/* update character origin */
        } /* while nglyph-- */
      case rgnOUT:
	break ;
      case rgnPART:
      {
	RegionPtr prgnClip ;

	BoxRec cbox ;
	int glx, gly ;

        while ( nglyph-- )
        {
	    pci = *ppci++ ;
	    bm = *bms++ ;
	    pglyph = pglyphBase + pci->byteOffset ;
	    w = GLYPHWIDTHPIXELS(pci) ;
	    h = GLYPHHEIGHTPIXELS(pci) ;
	    cbox.x1 = glx = x + pci->metrics.leftSideBearing ;
	    cbox.x2 = cbox.x1 + w ;
	    cbox.y1 = gly = y - pci->metrics.ascent ;
	    cbox.y2 = cbox.y1 + h ;

	    switch ( (* pScreen->RectIn)( pRegion,  &cbox ) ) {
	      case rgnIN:
		if ( bm )
		    (* cacheInfo->blitToScreen)( bm, glx, gly, fg, alu, pm );
		else
	    	    (* glyphFunc)( pglyph, glx, gly, w, h, fg, alu, pm ) ;
	      case rgnOUT:
		break ;
	      case rgnPART:
		if ( !been_here )
			{
			widthGlyph = pfi->maxbounds.metrics.rightSideBearing-
				        pfi->minbounds.metrics.leftSideBearing;
			widthGlyph = PADGLYPHWIDTHBYTES(widthGlyph);
    			pTempPixmap = ppcCreatePixmap( pScreen, 
				widthGlyph * 8,
				pfi->maxbounds.metrics.ascent +
					pfi->maxbounds.metrics.descent ,
				1 ) ;
			been_here = TRUE ;
			}
		prgnClip = (* pScreen->RegionCreate)( &cbox,
						      REGION_NUM_RECTS(pRegion)) ;
		(* pScreen->Intersect)( prgnClip, prgnClip, pRegion ) ;
		if ( !( nbox = REGION_NUM_RECTS(prgnClip) ) ) {
		    (* pScreen->RegionDestroy)( prgnClip ) ;
		    break ;
		}
		widthGlyph = GLYPHWIDTHBYTESPADDED(pci) ;
	    	filltempPixmap(pTempPixmap, pglyph, widthGlyph, h) ;
		for ( pbox = REGION_RECTS(prgnClip); nbox-- ; pbox++ )
			{
			ax = MAX(pbox->x1, cbox.x1) ;
			ay = MAX(pbox->y1, cbox.y1) ;
			zx = MIN(pbox->x2, cbox.x2) ;
			zy = MIN(pbox->y2, cbox.y2) ;
	    		if ( ( ( zw = zx - ax ) > 0 )
	    		  && ( ( zh = zy - ay ) > 0 ) )
				(*stipFunc)( pTempPixmap, 
					     fg, alu, pm, ax, ay,
					     zw, zh, glx, gly ) ;
			}
		(* pScreen->RegionDestroy)( prgnClip ) ;
		break ;
	    }
	    /* update character origin */
	    x += pci->metrics.characterWidth ;
        } /* while nglyph-- */

	if ( pTempPixmap )
	    (* pScreen->DestroyPixmap)( pTempPixmap ) ;

      }
      default:
	break ;
    }
    if ( !--(*(pScrPriv->CursorSemaphore)) && CursorIsSaved )
	(* pScrPriv->ReplaceCursor)() ;

    return ;
}

int
ppcCachedPolyText8(pDraw, pGC, x, y, count, chars)
	DrawablePtr pDraw;
	GCPtr pGC;
	int x, y;
	int count;
	char *chars;
	{
	return ppcCachedPolyText(pDraw, pGC, x, y, count, chars, Linear8Bit);
	}

int
ppcCachedPolyText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         x, y;
    int         count;
    unsigned short *chars;
{
    if (pGC->font->pFI->lastRow == 0)
        return ppcCachedPolyText(pDraw, pGC, x, y, count, (char *)chars, 
								Linear16Bit);
    else
        return ppcCachedPolyText(pDraw, pGC, x, y, count, (char *)chars, 
								TwoD16Bit);
}
