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
		Copyright IBM Corporation 1987

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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedText.c,v 1.2 89/11/19 20:46:53 jfc Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedText.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedText.c,v 1.2 89/11/19 20:46:53 jfc Exp $";
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
#include	"regionstr.h"
#include	"mfb.h"

#include	"OScompiler.h"
#include	"aedHdwr.h"
#include	"ibmTrace.h"

static unsigned int
aedWidth(n, charinfo)
    register CharInfoPtr charinfo[];
    register unsigned int n;
{
    register unsigned int w = 0;

    while ( n-- )
	w += charinfo[n]->metrics.characterWidth ;

    return w ;
}

void
aedImageText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    char	*chars;
{

    CharInfoPtr *charinfo;
    unsigned int n;
    FontPtr font = pGC->font;
    ExtentInfoRec info;
    xRectangle backrect;
    int nbox;
    BoxPtr pbox;
    short merge;
    int i;
    RegionPtr pRegion;
    BoxRec bbox;

    TRACE(("aedImageText8( pDraw=0x%x, pGC=0x%x, x=%d, y=%d, count=%d, chars=0x%x\n)", pDraw, pGC, x, y, count, chars));

    if (!(pGC->planemask & 1))
	return;

    if ( font->devPriv[pDraw->pScreen->myNum] == 0 )
	{
	miImageText8(pDraw, pGC, x, y, count, chars);
        return;
	}

    x += pDraw->x;
    y += pDraw->y;

    if(!(charinfo = (CharInfoPtr *)ALLOCATE_LOCAL(count*sizeof(CharInfoPtr))))
	return;

    GetGlyphs(font, count, chars, Linear8Bit, &n, charinfo);

    QueryGlyphExtents(font, charinfo, n, &info);

    DEALLOCATE_LOCAL(charinfo);

    backrect.x = x;
    backrect.y = y - font->pFI->fontAscent;
    backrect.width = info.overallWidth;
    backrect.height = font->pFI->fontAscent + 
		      font->pFI->fontDescent;

    bbox.x1 = x + info.overallLeft;
    bbox.x2 = x + info.overallRight;
    bbox.y1 = y - info.overallAscent;
    bbox.y2 = y + info.overallDescent;

    if (pGC->bgPixel)
	merge = mergexlate[GXset];
    else
	merge = mergexlate[GXclear];

    pRegion = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip;
    nbox = REGION_NUM_RECTS(pRegion);
    if ( nbox == 0 )
	return;
    pbox = REGION_RECTS(pRegion);

    vforce();
    clear(2);
    vikint[ORMERGE] = merge;
    vikint[ORCLIPLX] = pbox->x1;
    vikint[ORCLIPLY] = pbox->y1;
    vikint[ORCLIPHX] = pbox->x2-1;
    vikint[ORCLIPHY] = pbox->y2-1;
    vikint[ORXPOSN] = backrect.x;
    vikint[ORYPOSN] = backrect.y;
    pbox++;
    nbox--;
    vikint[vikoff++] = 10;	/* tile order */
    vikint[vikoff++] = backrect.width;	/* rectangle width */
    vikint[vikoff++] = backrect.height;	/* rectangle height */
    vikint[vikoff++] = 1;	/* tile height */
    vikint[vikoff++] = 1;	/* tile height */
    vikint[vikoff++] = -1;	/* tile (all ones) */

    vforce();

    vikint[VIKCMD] = 2;	/* reprocess order */
    for(i = 0; i < nbox; i++, pbox++)
    {
	vikint[ORCLIPLX] = pbox->x1;
	vikint[ORCLIPLY] = pbox->y1;
	vikint[ORCLIPHX] = pbox->x2-1;
	vikint[ORCLIPHY] = pbox->y2-1;
	command(ORDATA);
    }
    /* reset clipping window */
    clear(2);

    if( pGC->fgPixel )
	merge = mergexlate[GXor];
    else
	merge = mergexlate[GXandInverted];

    switch ((*pGC->pScreen->RectIn)
	    (((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip, &bbox))
    {
      case rgnOUT:
	break;
      case rgnIN:

        vikint[ORFONTID] = (unsigned short)font->devPriv[pDraw->pScreen->myNum];
	vikint[ORXPOSN] = x;
	vikint[ORYPOSN] = y;
	vikint[ORMERGE] = merge;
	vikint[vikoff++] = 11;	/* draw string */
	vikint[vikoff++] = count;
/*
	vikint[vikoff++] = ((count+1)/2)*2;
*/
	MOVE(chars, &vikint[vikoff], ((count+1)/2)*2);
	vikoff = vikoff + (count+1)/2;
	vforce();
	aedCheckFault();

	clear(2);
	break;

      case rgnPART:
      {
	nbox = REGION_NUM_RECTS(pRegion);
	pbox = REGION_RECTS(pRegion);

        vikint[ORFONTID] = (unsigned short)font->devPriv[pDraw->pScreen->myNum];
	vikint[ORXPOSN] = x;
	vikint[ORYPOSN] = y;
	vikint[ORMERGE] = merge;
	vikint[ORCLIPLX] = pbox->x1;
	vikint[ORCLIPLY] = pbox->y1;
	vikint[ORCLIPHX] = pbox->x2-1;
	vikint[ORCLIPHY] = pbox->y2-1;

	pbox++;
	nbox--;

	vikint[vikoff++] = 11;	/* draw string */
	vikint[vikoff++] = count;
/*
	vikint[vikoff++] = ((count+1)/2)*2;
*/
	MOVE(chars, &vikint[vikoff], ((count+1)/2)*2);
	vikoff = vikoff + (count+1)/2;
	vforce();
	aedCheckFault();

	vikint[VIKCMD] = 2;	/* reprocess order */
	for(i = 0; i < nbox; i++, pbox++)
	{
            vikint[ORFONTID] = (unsigned short)font->devPriv[pDraw->pScreen->myNum];
	    vikint[ORXPOSN] = x;
	    vikint[ORYPOSN] = y;
	    vikint[ORMERGE] = merge;
	    vikint[ORCLIPLX] = pbox->x1;
	    vikint[ORCLIPLY] = pbox->y1;
	    vikint[ORCLIPHX] = pbox->x2-1;
	    vikint[ORCLIPHY] = pbox->y2-1;
	    command(ORDATA);
	    aedCheckFault();
	}
	/* reset clipping window */
	clear(2);
	break;

      }
      default:
	break;
    }
    return;
}


int
aedPolyText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    char	*chars;
{

    CharInfoPtr *charinfo;
    unsigned int n;
    FontPtr font = pGC->font;
    unsigned int w;
    ExtentInfoRec info;
    xRectangle backrect;
    int nbox;
    BoxPtr pbox;
    short merge;
    int i;
    RegionPtr pRegion;
    BoxRec bbox;
    int xinit;

    TRACE(("aedPolyText8( pDraw=0x%x, pGC=0x%x, x=%d, y=%d, count=%d, chars=0x%x\n)", pDraw, pGC, x, y, count, chars));

    if ( ( pGC->fillStyle != FillSolid ) || ( font->devPriv[pDraw->pScreen->myNum] == 0 ) )
	{
        return( miPolyText8(pDraw, pGC, x, y, count, chars) );
	}

    xinit = x;

    x += pDraw->x;
    y += pDraw->y;

    if(!(charinfo = (CharInfoPtr *)ALLOCATE_LOCAL(count*sizeof(CharInfoPtr))))
	return(0);

    GetGlyphs(font, count, chars, Linear8Bit, &n, charinfo);

    w = aedWidth(n, charinfo);

    if (!(pGC->planemask & 1))
	return(w+xinit);

    QueryGlyphExtents(font, charinfo, n, &info);

    DEALLOCATE_LOCAL(charinfo);

    backrect.x = x;
    backrect.y = y - font->pFI->fontAscent;
    backrect.width = info.overallWidth;
    backrect.height = font->pFI->fontAscent + 
		      font->pFI->fontDescent;

    bbox.x1 = x + info.overallLeft;
    bbox.x2 = x + info.overallRight;
    bbox.y1 = y - info.overallAscent;
    bbox.y2 = y + info.overallDescent;

    pRegion = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip;
    nbox = REGION_NUM_RECTS(pRegion);
    pbox = REGION_RECTS(pRegion);
    if ( nbox == 0 )
	return(xinit+w);

    if( pGC->fgPixel )
	merge = mergexlate[GXor];
    else
	merge = mergexlate[GXandInverted];

    switch ((*pGC->pScreen->RectIn)
	    (((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip, &bbox))
    {
      case rgnOUT:
	break;
      case rgnIN:

        vikint[ORFONTID] = (unsigned short)font->devPriv[pDraw->pScreen->myNum];
	vikint[ORXPOSN] = x;
	vikint[ORYPOSN] = y;
	vikint[ORMERGE] = merge;
	vikint[vikoff++] = 11;	/* draw string */
	vikint[vikoff++] = count;
/*
	vikint[vikoff++] = ((count+1)/2)*2;
*/
	MOVE(chars, &vikint[vikoff], ((count+1)/2)*2);
	vikoff = vikoff + (count+1)/2;
	vforce();
	aedCheckFault();

	clear(2);
	break;

      case rgnPART:
      {
	nbox = REGION_NUM_RECTS(pRegion);
	pbox = REGION_RECTS(pRegion);

        vikint[ORFONTID] = (unsigned short)font->devPriv[pDraw->pScreen->myNum];
	vikint[ORXPOSN] = x;
	vikint[ORYPOSN] = y;
	vikint[ORMERGE] = merge;
	vikint[ORCLIPLX] = pbox->x1;
	vikint[ORCLIPLY] = pbox->y1;
	vikint[ORCLIPHX] = pbox->x2-1;
	vikint[ORCLIPHY] = pbox->y2-1;

	pbox++;
	nbox--;

	vikint[vikoff++] = 11;	/* draw string */
	vikint[vikoff++] = count;
/*
	vikint[vikoff++] = ((count+1)/2)*2;
*/
	MOVE(chars, &vikint[vikoff], ((count+1)/2)*2);
	vikoff = vikoff + (count+1)/2;
	vforce();
	aedCheckFault();

	vikint[VIKCMD] = 2;	/* reprocess order */
	for(i = 0; i < nbox; i++, pbox++)
	{
            vikint[ORFONTID] = (unsigned short)font->devPriv[pDraw->pScreen->myNum];
	    vikint[ORXPOSN] = x;
	    vikint[ORYPOSN] = y;
	    vikint[ORMERGE] = merge;
	    vikint[ORCLIPLX] = pbox->x1;
	    vikint[ORCLIPLY] = pbox->y1;
	    vikint[ORCLIPHX] = pbox->x2-1;
	    vikint[ORCLIPHY] = pbox->y2-1;
	    command(ORDATA);
	    aedCheckFault();
	}
	/* reset clipping window */
	clear(2);
	break;

      }
      default:
	break;
    }
    return(xinit+w);
}
