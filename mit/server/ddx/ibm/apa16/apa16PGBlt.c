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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16PGBlt.c,v 3.3 89/11/04 16:56:15 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16PGBlt.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16PGBlt.c,v 3.3 89/11/04 16:56:15 probe Exp $";
static char sccsid[] = "@(#)apa16pgblt.c	3.1 88/09/22 09:30:56";
#endif

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "fontstruct.h"
#include "dixfontstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "regionstr.h"
#include "mfb.h"
#include "maskbits.h"
#include "miscstruct.h"

#include "OScompiler.h"

#include "ibmTrace.h"

#include "apa16Hdwr.h"
#include "apa16GBlt.h"

/*
    we should eventually special-case fixed-width fonts, although
its more important for ImageText, which is meant for terminal
emulators.

    the clipping calculations are done for worst-case fonts.
we make no assumptions about the heights, widths, or bearings
of the glyphs.  if we knew that the glyphs are all the same height,
we could clip the tops and bottoms per clipping box, rather
than per character per clipping box.  if we knew that the glyphs'
left and right bearings were well-behaved, we could clip a single
character at the start, output until the last unclipped
character, and then clip the last one.  this is all straightforward
to determine based on max-bounds and min-bounds from the font.
    there is some inefficiency introduced in the per-character
clipping to make what's going on clearer.

    (it is possible, for example, for a font to be defined in which the
next-to-last character in a font would be clipped out, but the last
one wouldn't.  the code below deals with this.)

    PolyText looks at the fg color and the rasterop; mfbValidateGC
swaps in the right routine after looking at the reduced ratserop
in the private field of the GC.  

*/

void
apa16PolyGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci, pGlyphBase)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    char 	*pGlyphBase;	/* start of array of glyphs */
{
    ExtentInfoRec info;	/* used by QueryGlyphExtents() */
    BoxRec bbox;		/* string's bounding box */

    CharInfoPtr pci;
    int xorg, yorg;	/* origin of drawable in bitmap */
    int widthDst;	/* width of dst in longwords */

			/* these keep track of the character origin */
    unsigned int *pDstBase;
			/* points to longword with character origin */
    int xchar;		/* xorigin of char (mod 32) */

			/* these are used for placing the glyph */
    int xOff;		/* x offset of left edge of glyph (mod 32) */
    unsigned int *pDst;	/* pointer to current longword in dst */

    int w;		/* width of glyph in bits */
    int h;		/* height of glyph */
    int widthGlyph;	/* width of glyph, in bytes */
    char *pGlyph;	/* pointer to current row of glyph */

    int startmask;	/* used for putting down glyph */
    int endmask;
    int nFirst;		/* bits of glyph in current longword */
    int tmpSrc;		/* for getting bits from glyph */
    int	rrop;

    TRACE(("apa16PolyGlyphBlt(pDrawable= 0x%x, pGC= 0x%x, x= %d, y= %d, nglyph= %d, ppci= %d, pGlyphBase= 0x%x)\n",pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase));

    if (!(pGC->planemask & 1))
	return;

    rrop= ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop;
    if (rrop==RROP_NOP)
	return;

    if (pDrawable->type != DRAWABLE_WINDOW)
    {
	if (rrop==RROP_WHITE) 
	    mfbPolyGlyphBltWhite(pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase);
	else if (rrop==RROP_BLACK)
	    mfbPolyGlyphBltBlack(pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase);
	else if (rrop==RROP_INVERT)
	    mfbPolyGlyphBltInvert(pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase);
	else  {
	    ErrorF("unexpected rrop %d in apa16PolyGlyphBlt\n",rrop);
	    ErrorF("calling miPolyGlyphBlt\n");
	    miPolyGlyphBlt(pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase);
	}
	return;
    }

    if (rrop==RROP_WHITE)	{ SET_MERGE_MODE(MERGE_WHITE); }
    else if (rrop==RROP_BLACK)	{ SET_MERGE_MODE(MERGE_BLACK); }
    else if (rrop==RROP_INVERT)	{ SET_MERGE_MODE(MERGE_INVERT); }
    else if (rrop==RROP_NOP)	return;
    else {
	ErrorF("Unexpected rrop %d in apa16PolyGlyphBlt\n",rrop);
	ErrorF("calling miPolyGlyphBlt\n");
	miPolyGlyphBlt(pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase);
	return;
    }
    xorg = pDrawable->x;
    yorg = pDrawable->y;
    pDstBase= (unsigned int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
    widthDst = (int)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind)>>2;

    x += xorg;
    y += yorg;

    QueryGlyphExtents(pGC->font, ppci, nglyph, &info);
    bbox.x1 = x + info.overallLeft;
    bbox.x2 = x + info.overallRight;
    bbox.y1 = y - info.overallAscent;
    bbox.y2 = y + info.overallDescent;

    switch ((*pGC->pScreen->RectIn)(
		((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip, &bbox))
    {
      case rgnOUT:
	break;
      case rgnIN:
        pDstBase = pDstBase + (widthDst * y) + (x >> 5);
        xchar = x & 0x1f;

        while(nglyph--)
        {
	    pci = *ppci;
	    pGlyph = pGlyphBase + pci->byteOffset;
	    w = GLYPHWIDTHPIXELS(pci);
	    h = GLYPHHEIGHTPIXELS(pci);
	    widthGlyph = GLYPHWIDTHBYTESPADDED(pci);

	    /* start at top scanline of glyph */
	    pDst = pDstBase - (pci->metrics.ascent * widthDst);

	    /* find correct word in scanline and x offset within it
	       for left edge of glyph
	    */
	    xOff = xchar + pci->metrics.leftSideBearing;
	    while (xOff > 31)
	    {
	        pDst++;
	        xOff -= 32;
	    }
	    while (xOff < 0)
	    {
	        xOff += 32;
	        pDst--;
	    }

	    if ((xOff + w) <= 32) {
		GLYPH_NARROW(UNCLIPPED);
	    }
	    else if (w<=32) {
		GLYPH_CROSSES(UNCLIPPED);
	    } else {
		GLYPH_WIDE();
	    }

	    /* update character origin */
	    x += pci->metrics.characterWidth;
	    xchar += pci->metrics.characterWidth;
	    while (xchar > 31)
	    {
	        xchar -= 32;
	        pDstBase++;
	    }
	    while (xchar < 0)
	    {
	        xchar += 32;
	        pDstBase--;
	    }
	    ppci++;
        } /* while nglyph-- */
	break;
      case rgnPART:
      {
	TEXTPOS *ppos;
	int nbox;
	BoxPtr pbox;
	int xpos;		/* x position of char origin */
	int i;
	BoxRec clip;
	int leftEdge, rightEdge;
	int topEdge, bottomEdge;
	int glyphRow;		/* first row of glyph not wholly
				   clipped out */
	int glyphCol;		/* leftmost visible column of glyph */

	if(!(ppos = (TEXTPOS *)ALLOCATE_LOCAL(nglyph * sizeof(TEXTPOS))))
	    return;

        pDstBase = pDstBase + (widthDst * y) + (x >> 5);
        xpos = x;
	xchar = xpos & 0x1f;

	for (i=0; i<nglyph; i++)
	{
	    pci = ppci[i];

	    ppos[i].xpos = xpos;
	    ppos[i].xchar = xchar;
	    ppos[i].leftEdge = xpos + pci->metrics.leftSideBearing;
	    ppos[i].rightEdge = xpos + pci->metrics.rightSideBearing;
	    ppos[i].topEdge = y - pci->metrics.ascent;
	    ppos[i].bottomEdge = y + pci->metrics.descent;
	    ppos[i].pdstBase = pDstBase;
	    ppos[i].widthGlyph = GLYPHWIDTHBYTESPADDED(pci);

	    xpos += pci->metrics.characterWidth;
	    xchar += pci->metrics.characterWidth;
	    while (xchar > 31)
	    {
		xchar -= 32;
		pDstBase++;
	    }
	    while (xchar < 0)
	    {
		xchar += 32;
		pDstBase--;
	    }
	}

	pbox = REGION_RECTS(((mfbPrivGC *)
			     (pGC->devPrivates[mfbGCPrivateIndex].ptr))->
			    pCompositeClip);
	nbox = REGION_NUM_RECTS(((mfbPrivGC *)
			     (pGC->devPrivates[mfbGCPrivateIndex].ptr))->
				pCompositeClip);

	/* HACK ALERT
	   since we continue out of the loop below so often, it
	   is easier to increment pbox at the  top than at the end.
	   don't try this at home.
	*/
	pbox--;
	while(nbox--)
	{
	    pbox++;
	    clip.x1 = MAX(bbox.x1, pbox->x1);
	    clip.y1 = MAX(bbox.y1, pbox->y1);
	    clip.x2 = MIN(bbox.x2, pbox->x2);
	    clip.y2 = MIN(bbox.y2, pbox->y2);
	    if ((clip.x2<=clip.x1) || (clip.y2<=clip.y1))
		continue;

	    for(i=0; i<nglyph; i++)
	    {
		pci = ppci[i];
		xchar = ppos[i].xchar;

		/* clip the left and right edges */
		if (ppos[i].leftEdge < clip.x1)
		    leftEdge = clip.x1;
		else
		    leftEdge = ppos[i].leftEdge;

		if (ppos[i].rightEdge > clip.x2)
		    rightEdge = clip.x2;
		else
		    rightEdge = ppos[i].rightEdge;

		w = rightEdge - leftEdge;
		if (w <= 0)
		    continue;

		/* clip the top and bottom edges */
		if (ppos[i].topEdge < clip.y1)
		    topEdge = clip.y1;
		else
		    topEdge = ppos[i].topEdge;

		if (ppos[i].bottomEdge > clip.y2)
		    bottomEdge = clip.y2;
		else
		    bottomEdge = ppos[i].bottomEdge;

		h = bottomEdge - topEdge;
		if (h <= 0)
		    continue;

		glyphRow = (topEdge - y) + pci->metrics.ascent;
		widthGlyph = ppos[i].widthGlyph;
		pGlyph = pGlyphBase + pci->byteOffset;
		pGlyph += (glyphRow * widthGlyph);

		pDst = ppos[i].pdstBase - ((y-topEdge) * widthDst);

		glyphCol = (leftEdge - ppos[i].xpos) -
			   (pci->metrics.leftSideBearing);
		xOff = xchar + (leftEdge - ppos[i].xpos);
		while (xOff > 31) {
		    xOff -= 32;
		    pDst++;
		}
		while  (xOff < 0) {
		    xOff += 32;
		    pDst--;
		}

		if ((xOff + w) <= 32) {
		    GLYPH_NARROW(CLIPPED);
		}
		else if (w<=32) {
		    GLYPH_CROSSES(CLIPPED);
		} else {
		    GLYPH_WIDE();
		} 
	    } /* for each glyph */
	} /* while nbox-- */
	DEALLOCATE_LOCAL(ppos);
	break;
      }
      default:
	break;
    }
    QUEUE_SET_MERGE_MODE(MERGE_COPY);
}


/***==================================================================***/

void
apa16PutGlyph(x2,y2,cInfo,pGlyphs)
unsigned	 x2,y2;
CharInfoPtr	 cInfo;
unsigned char	*pGlyphs;
{
    int		 	 w,h,nFirst;

    int			 widthGlyph;
    volatile unsigned long	*pDst;
    int		 	 xOff,xEdge;
    unsigned char		*pGlyph;
    unsigned		 startmask,endmask,tmpSrc;

    TRACE(("apa16PutGlyph(%d,%d,0x%x,0x%d)\n",x2,y2,cInfo,pGlyphs));

    w=		cInfo->metrics.rightSideBearing-cInfo->metrics.leftSideBearing;
    h=		cInfo->metrics.ascent+cInfo->metrics.descent;
    xOff=	x2-w;
    xEdge=	(xOff/32)*32;
    xOff-=	xEdge;
    pDst=	(volatile unsigned long *)SCREEN_ADDR(xEdge,y2-h);
    pGlyph=	pGlyphs+cInfo->byteOffset;
    widthGlyph=	GLYPHWIDTHBYTESPADDED(cInfo);

    SET_MERGE_MODE(MERGE_WHITE);
    if ((xOff + w) <= 32) {		/* glyph all in one longword */
	GLYPH_NARROW(UNCLIPPED);
    }
    else if (w<=32) {			/* glyph crosses longword boundary */
	GLYPH_CROSSES(UNCLIPPED);
    } 
    else {				/* WIDE LOAD */
	GLYPH_WIDE();
    }
    SET_MERGE_MODE(MERGE_COPY);
}
