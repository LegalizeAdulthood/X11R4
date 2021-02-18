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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16IGBlt.c,v 3.2 89/11/04 16:05:06 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16IGBlt.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16IGBlt.c,v 3.2 89/11/04 16:05:06 probe Exp $";
static char sccsid[] = "@(#)apa16igblt.c	2.1 88/09/19 11:55:57";
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
#include	"maskbits.h"

#include	"OScompiler.h"

#include	"ibmTrace.h"
#include	"apa16Hdwr.h"
#include	"apa16GBlt.h"

/*
    we should eventually special-case fixed-width fonts for ImageText.

    this works for fonts with glyphs <= 32 bits wide.

    the clipping calculations are done for worst-case fonts.
we make no assumptions about the heights, widths, or bearings
of the glyphs.  if we knew that the glyphs are all the same height,
we could clip the tops and bottoms per clipping box, rather
than per character per clipping box.  if we knew that the glyphs'
left and right bearings were wlle-behaved, we could clip a single
character at the start, output until the last unclipped
character, and then clip the last one.  this is all straightforward
to determine based on max-bounds and min-bounds from the font.
    there is some inefficiency introduced in the per-character
clipping to make what's going on clearer.

    (it is possible, for example, for a font to be defined in which the
next-to-last character in a font would be clipped out, but the last
one wouldn't.  the code below deals with this.)

    Image text looks at the bits in the glyph and the fg and bg in the
GC.  it paints a rectangle, as defined in the protocol dcoument,
and the paints the characters.
*/

void
apa16ImageGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci, pGlyphBase)
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    pointer 	pGlyphBase;	/* start of array of glyphs */
{
    ExtentInfoRec info;	/* used by QueryGlyphExtents() */
    BoxRec bbox;	/* string's bounding box */
    xRectangle backrect;/* backing rectangle to paint.
			   in the general case, NOT necessarily
			   the same as the string's bounding box
			*/

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
    pointer pGlyph;	/* pointer to current row of glyph */

    int startmask;	/* used for putting down glyph */
    int endmask;
    int nFirst;		/* bits of glyph in current longword */
    int tmpSrc;		/* for getting bits from glyph */
    int rrop;
    CARD16	cmd;
    CARD16	new_mode;

    TRACE(("apa16ImageGlyphBlt(pDrawable= 0x%x, pGC= 0x%x, x= %d, y= %d, nglyph= %d, ppci= %d, pGlyphBase= 0x%x )\n",pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase));

    rrop= (((mfbPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)->rop);
    if ((!(pGC->planemask&0x1))||(rrop==RROP_NOP))
	return;

    if (pDrawable->type != DRAWABLE_WINDOW) {
	if (rrop==RROP_WHITE)
	    mfbImageGlyphBltWhite(pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase);
	else if (rrop==RROP_BLACK)
	    mfbImageGlyphBltBlack(pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase);
	else
	    miImageGlyphBlt(pDrawable,pGC,x,y,nglyph,ppci,pGlyphBase);
	return;
    }
    xorg = pDrawable->x;
    yorg = pDrawable->y;
    pDstBase = (unsigned int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
    widthDst = (int)
		 (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;

    if (pGC->fgPixel==0) {
	rrop= 		RROP_BLACK;
	new_mode= 	MERGE_BLACK;
	APA16_GET_CMD(ROP_RECT_FILL,RROP_WHITE,cmd);
    }
    else {
	rrop= 		RROP_WHITE;
	new_mode= 	MERGE_WHITE;
	APA16_GET_CMD(ROP_RECT_FILL,RROP_BLACK,cmd);
    }


    QueryGlyphExtents(pGC->font, ppci, nglyph, &info);

    x += xorg;
    y += yorg;

    backrect.x = x;
    backrect.y = y - pGC->font->pFI->fontAscent;
    backrect.width = info.overallWidth;
    backrect.height = pGC->font->pFI->fontAscent + 
		      pGC->font->pFI->fontDescent;
    if ((backrect.width==0)||(backrect.height==0))
	return;

    bbox.x1 = x + info.overallLeft;
    bbox.x2 = x + info.overallRight;
    bbox.y1 = y - info.overallAscent;
    bbox.y2 = y + info.overallDescent;

    switch ((*pGC->pScreen->RectIn)(
			((mfbPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)
				    ->pCompositeClip,&bbox))
    {
      case rgnOUT:
	break;
      case rgnIN:
	FILL_RECT(cmd,backrect.x+backrect.width,backrect.y+backrect.height,
				backrect.width,backrect.height);
        pDstBase = pDstBase + (widthDst * y) + (x >> 5);
        xchar = x & 0x1f;

        SET_MERGE_MODE(new_mode);

        while(nglyph--)
        {
	    pci = *ppci;
	    pGlyph = pGlyphBase + pci->byteOffset;
	    w = pci->metrics.rightSideBearing - pci->metrics.leftSideBearing;
	    h = pci->metrics.ascent + pci->metrics.descent;
	    widthGlyph = GLYPHWIDTHBYTESPADDED( pci );

	    /* start at top scanline of glyph */
	    pDst = pDstBase - (pci->metrics.ascent * widthDst);

	    /* find correct word in scanline and x offset within it
	       for left edge of glyph
	    */
	    xOff = xchar + pci->metrics.leftSideBearing;
	    while (xOff > 31) {
	        pDst++;
	        xOff -= 32;
	    }
	    while (xOff < 0) {
	        xOff += 32;
	        pDst--;
	    }

	    if ((xOff + w) <= 32) { /* glyph all in one longword */
		GLYPH_NARROW(UNCLIPPED);
	    }
	    else if (w<=32) { /* glyph crosses longword boundary */
		GLYPH_CROSSES(UNCLIPPED);
	    } else {
		GLYPH_WIDE();
	    }

	    /* update character origin */
	    x += pci->metrics.characterWidth;
	    xchar += pci->metrics.characterWidth;
	    if (xchar > 31) {
	        xchar -= 32;
	        pDstBase++;
	    }
	    else if (xchar < 0) {
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
	RegionPtr	bboxClip;

	if(!(ppos = (TEXTPOS *)ALLOCATE_LOCAL(nglyph * sizeof(TEXTPOS))))
	    return;

	bboxClip= (*pGC->pScreen->RegionCreate)(&bbox,1);
	(*pGC->pScreen->Intersect)(bboxClip,bboxClip,
		    ((mfbPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)->
				   pCompositeClip);
	
	pbox= REGION_RECTS(bboxClip);
	nbox= REGION_NUM_RECTS(bboxClip);
	while (nbox--) {
	    register w= pbox->x2-pbox->x1;
	    register h= pbox->y2-pbox->y1;

	    if (w==0) w=1;
	    if (h==0) h=1;
	    FILL_RECT(cmd,pbox->x2,pbox->y2,w,h);
	    pbox++;
	}
	(*pGC->pScreen->RegionDestroy)(bboxClip);

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
	    ppos[i].widthGlyph = GLYPHWIDTHBYTESPADDED( pci );

	    xpos += pci->metrics.characterWidth;
	    xchar += pci->metrics.characterWidth;
	    while (xchar > 31) {
		xchar -= 32;
		pDstBase++;
	    }
	    while (xchar < 0) {
		xchar += 32;
		pDstBase--;
	    }
	}

	pbox = REGION_RECTS(((mfbPrivGC *)
			    pGC->devPrivates[mfbGCPrivateIndex].ptr)->
			    pCompositeClip);
	nbox = REGION_NUM_RECTS(((mfbPrivGC *)
			    pGC->devPrivates[mfbGCPrivateIndex].ptr)->
			    pCompositeClip);

	/* HACK ALERT
	   since we continue out of the loop below so often, it
	   is easier to increment pbox at the  top than at the end.
	   don't try this at home.
	*/
	pbox--;

        SET_MERGE_MODE(new_mode);

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
		while (xOff < 0) {
		    xOff += 32;
		    pDst--;
		}

		if ((xOff + w) <= 32)
		{
		    GLYPH_NARROW(CLIPPED);
		}
		else if (w<=32)
		{
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
