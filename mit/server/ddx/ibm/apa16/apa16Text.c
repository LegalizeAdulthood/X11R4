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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Text.c,v 3.1 89/11/04 16:03:08 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Text.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Text.c,v 3.1 89/11/04 16:03:08 probe Exp $";
static char sccsid[] = "@(#)apa16text.c	3.1 88/09/22 09:31:14";
#endif

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"fontstruct.h"
#include	"font.h"
#include	"dixfontstr.h"
#include	"dixfont.h"
#include	"gc.h"
#include	"gcstruct.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"pixmap.h"
#include	"windowstr.h"
#include	"window.h"
#include	"region.h"
#include	"regionstr.h"

#include	"mfb.h"

#include	"ibmTrace.h"

#include	"OScompiler.h"
#include	"apa16Hdwr.h"
#include	"apa16Font.h"
#include	"apa16Text.h"

/***==================================================================***/

#if defined(DEBUG) || defined(TRACE_X)
static int	debugKnt;
#endif 

void
apa16ImageText( pDraw, pGC, x, y, count, chars, fontEncoding )
DrawablePtr 	 pDraw;
GCPtr		 pGC;
int		 x, y;
int		 count;
char		*chars;
FontEncoding	 fontEncoding;
{
CharInfoPtr		*charinfo,*tmpCinfo;
unsigned int	 	 n;
FontPtr 		 font = pGC->font;
ExtentInfoRec	 	 info;
int 		 	 nbox;
BoxPtr 		 	 pbox;
RegionPtr 		 pRegion;
BoxRec		 	 backbox;
BoxRec 		 	 bbox;
apa16Font 		*aFont;
unsigned 		 rectCmd,bltCmd;
unsigned short		*charsOut,*freeCharsOut;

TRACE(("apa16ImageText( 0x%x, 0x%x, (%d,%d), %d, 0x%x, 0x%x)\n", 
			pDraw, pGC, x, y, count, chars, fontEncoding));

	/*
	 * if not touching any planes, or mode is no, no effect, return.
	 */
#if defined(DEBUG) || defined(TRACE_X)
    if (strncmp(chars,"dEbuG",5)==0)  {
	if (debugKnt++>5) 
	    copy_font_area();
    }
#endif

    if ( !( pGC->planemask & 0x1 ) || ( pGC->alu == GXnoop ) )
	return ;
	
	/*
	 * if priv<40, font was not realized by apa16RealizeFont,
	 * if not a window, or merge mode not copy, the hardware can't 
	 * help us.
	 * (I guess we *could* write an glyph blt that uses blt and
	 *  the character as stored on the adapter....)
	 */

    if ( ( ( (CARD32) font->devPriv[pDraw->pScreen->myNum] ) < 40 )
	  || ( pDraw->type != DRAWABLE_WINDOW ) ) {
	miImageText( pDraw, pGC, x, y, count, chars, fontEncoding ) ;
	return;
    }

    if (pGC->fgPixel==0) {
	APA16_GET_CMD(ROP_RECT_FILL,RROP_WHITE,rectCmd);
	APA16_GET_CMD(ROP_RECT_COPY,GXandInverted,bltCmd);
    }
    else {
	APA16_GET_CMD(ROP_RECT_FILL,RROP_BLACK,rectCmd);
	APA16_GET_CMD(ROP_RECT_COPY,GXor,bltCmd);
    }

    x += pDraw->x;
    y += pDraw->y;

    if ( !( charinfo = (CharInfoPtr *)
		ALLOCATE_LOCAL( count * sizeof (CharInfoPtr) ) ) )
	return;
    if ( !( charsOut= (unsigned short *)
		ALLOCATE_LOCAL( count * sizeof(unsigned short) ) ) ) {
	DEALLOCATE_LOCAL(charinfo);
	return;
    }

    GetGlyphsCharsAndExtents(	font,count,chars,fontEncoding,
				&n,charinfo,charsOut, &info );

TRACE(("apa16ImageText: GetGlyphsCharsAndExtents returns n=%d\n", n));

    if ((count= n)==0)
   	return;

    backbox.x1=	x;
    backbox.x2= backbox.x1+info.overallWidth;
    backbox.y1=	y-font->pFI->fontAscent;
    backbox.y2= backbox.y1+	pGC->font->pFI->fontAscent+
				pGC->font->pFI->fontDescent;

    bbox.x1 = x + info.overallLeft;
    bbox.x2 = x + info.overallRight;
    bbox.y1 = y - info.overallAscent;
    bbox.y2 = y + info.overallDescent;


    pRegion = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))
      ->pCompositeClip;

    switch ((*pGC->pScreen->RectIn)(pRegion,&backbox)) {
	case rgnIN:
		if ((backbox.x1<backbox.x2)&&(backbox.y1<backbox.y2)) {
		    FILL_RECT(rectCmd,	backbox.x2, backbox.y2,
				backbox.x2-backbox.x1,backbox.y2-backbox.y1);
		}
		TRACE(("apa16ImageText: rgnIN\n"));
		break;
	case rgnOUT:
		TRACE(("apa16ImageText: rgnOUT\n"));
		break;
	case rgnPART:
		TRACE(("apa16ImageText: rgnPART\n"));

		nbox = REGION_NUM_RECTS(pRegion);
		pbox = REGION_RECTS(pRegion);

		for ( ; nbox-- ; pbox++ ) {
		    int	left,right,top,bottom;
		    left=	MAX(pbox->x1,bbox.x1);
		    right=	MIN(pbox->x2,bbox.x2);
		    top=	MAX(pbox->y1,bbox.y1);
		    bottom=	MIN(pbox->y2,bbox.y2);
		    if ((left<right)&&(top<bottom)) {
			FILL_RECT(rectCmd,	right,	bottom,
					right-left, bottom-top);
		    }
		}
		break;
    }
    aFont= (apa16Font *)font->devPriv[pDraw->pScreen->myNum];

    TRACE(("apa16ImageText: aFont = %x\n",aFont));

    afTouch(aFont);
    tmpCinfo= 		charinfo;
    freeCharsOut=	charsOut;
    switch ((*pGC->pScreen->RectIn)(pRegion,&bbox)) {
	case rgnIN:
		TRACE(("apa16ImageText: rgnIN\n"));
   
		while (count>0) {
		    xCharInfo	*metrics;
		    int		 width;	

		    if (!afChMapped(aFont,*charsOut)) {
			TRACE(("apa16ImageText: rgnIN:afChMapped failed\n"));
			afMap(aFont,count,charsOut,tmpCinfo,
						aFont->afFont->pGlyphs);
		    }
		    metrics=	&tmpCinfo[0]->metrics;
		    if (afChMapped(aFont,*charsOut)) {
			width=		metrics->rightSideBearing-
					metrics->leftSideBearing;

			COPY_RECT(bltCmd,
				  x+metrics->rightSideBearing,
				  y+metrics->descent,
				  afXoff(aFont,*charsOut), 
				  afYoff(aFont,*charsOut),
				  width,metrics->ascent+metrics->descent);
		    }
		    x+= metrics->characterWidth;
		    count--;
		    charsOut++;
		    tmpCinfo++;
		}
		break;
	case rgnOUT:
		TRACE(("apa16ImageText: rgnOUT\n"));
		break;
	case rgnPART:
		TRACE(("apa16ImageText: rgnPART\n"));

		while(count>0) {
		    xCharInfo	*metrics;
		    int		 width;

		    if (!afChMapped(aFont,*charsOut)) {
			TRACE(("apa16ImageText: rgnPART:afChMapped failed\n"));
			afMap(aFont,count,charsOut,tmpCinfo, 
						   aFont->afFont->pGlyphs);
		    }
		    metrics = &tmpCinfo[0]->metrics;
		    width = metrics->rightSideBearing- 
		      metrics->leftSideBearing;

		    if (afChMapped(aFont,*charsOut)) {
			BoxRec cbox;

			cbox.y1 = y-metrics->ascent;
			cbox.y2 = y+metrics->descent;

			cbox.x2 = width +(cbox.x1=x+metrics->leftSideBearing);

			switch( (*pGC->pScreen->RectIn)(pRegion,&cbox)) {
			    case rgnOUT:
				break;
			    case rgnIN:
				COPY_RECT(bltCmd,cbox.x2, cbox.y2,
					afXoff(aFont,*charsOut),
					afYoff(aFont,*charsOut),
					width,
					metrics->ascent+metrics->descent);
				break;
			    case rgnPART:
				{
				RegionPtr pClipRgn;

				pClipRgn = (*pGC->pScreen->RegionCreate)
				  (&cbox,REGION_NUM_RECTS(pRegion));
				(*pGC->pScreen->Intersect) 
				  (pClipRgn, pClipRgn, pRegion);

				for(pbox = REGION_RECTS(pClipRgn),
				    nbox = REGION_NUM_RECTS(pClipRgn);
				    nbox--;
				    pbox++){
				  	COPY_RECT(bltCmd,
						pbox->x2, pbox->y2,
						afXoff(aFont,*charsOut)+
							pbox->x2-cbox.x2,
						afYoff(aFont,*charsOut)+
							pbox->y2-cbox.y2,
						pbox->x2-pbox->x1,
						pbox->y2-pbox->y1);
				}
				(*pGC->pScreen->RegionDestroy)(pClipRgn);
				}
				break;
				}
		    }
		    x+= metrics->characterWidth;
		    count--;
		    charsOut++;
		    tmpCinfo++;
		}
		break;
    }
    DEALLOCATE_LOCAL(charinfo);
    DEALLOCATE_LOCAL(freeCharsOut);
    afTouch(aFont);
    return ;
}

/***==================================================================***/

int
apa16PolyText( pDraw, pGC, x, y, count, chars, fontEncoding )
DrawablePtr 	 pDraw;
GCPtr		 pGC;
int		 x, y;
int		 count;
char		*chars;
FontEncoding	 fontEncoding;
{
CharInfoPtr	*charinfo,*tmpCinfo;
unsigned int	 n;
FontPtr 	 font = pGC->font;
ExtentInfoRec	 info;
int 		 nbox,xOffset;
BoxPtr 		 pbox;
RegionPtr 	 pRegion;
BoxRec 		 bbox;
unsigned short	*charsOut,*freeCharsOut;
int		 intersect;
apa16Font 	*aFont;
unsigned 	 bltCmd;

TRACE(("apa16PolyText( 0x%x, 0x%x, (%d,%d), %d, 0x%x, 0x%x)\n", pDraw, pGC, x, y, count, chars,fontEncoding));

	/*
	 * if priv<40, font was not realized by apa16RealizeFont,
	 * if not a window, or merge mode not copy, the hardware can't 
	 * help us.
	 * (I guess we *could* write an glyph blt that uses blt and
	 *  the character as stored on the adapter....)
	 */

#if defined(DEBUG) || defined(TRACE_X)
    if (strncmp(chars,"dEbuG",5)==0)  {
	if (debugKnt++>5) 
	    copy_font_area();
    }
#endif

    if (( pGC->alu == GXnoop )||((pGC->planemask&0x1)==0)) {
	return(x);
    }
    if ( ( ( (CARD32) font->devPriv[pDraw->pScreen->myNum] ) < 40 ) ||
	  ( pDraw->type != DRAWABLE_WINDOW ) ) {
	return(miPolyText( pDraw, pGC, x, y, count, chars, fontEncoding ));
    }

    n= ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop;	/* n is just a handy temp */
    if (n==RROP_WHITE)	{ 
	APA16_GET_CMD(ROP_RECT_COPY,GXor,bltCmd); 
    }
    else if (n==RROP_BLACK) { 
	APA16_GET_CMD(ROP_RECT_COPY,GXandInverted,bltCmd); 
    }
    else if (n==RROP_INVERT) {
	APA16_GET_CMD(ROP_RECT_COPY,GXxor,bltCmd); 
    }
    else
        return miPolyText( pDraw, pGC, x, y, count, chars, fontEncoding ) ;

    x += (xOffset=pDraw->x);
    y += pDraw->y;

    if ( !( charinfo = (CharInfoPtr *)
		ALLOCATE_LOCAL( count * sizeof (CharInfoPtr) ) ) )
	return x-xOffset;
    if ( !( charsOut = (unsigned short *)
		ALLOCATE_LOCAL( count * sizeof (unsigned short) ) ) ) {
	DEALLOCATE_LOCAL(charinfo);
	return x-xOffset;
    }

    GetGlyphsCharsAndExtents(	font, count, chars, fontEncoding, 
				&n, charinfo, charsOut, &info);
    if ((count= n)==0)
   	return x-xOffset;

    bbox.x1 = x + info.overallLeft;
    bbox.x2 = x + info.overallRight;
    bbox.y1 = y - info.overallAscent;
    bbox.y2 = y + info.overallDescent;

    pRegion = ( (mfbPrivGC *) (pGC->devPrivates[mfbGCPrivateIndex].ptr) )->pCompositeClip;
    intersect= (*pGC->pScreen->RectIn)(pRegion,&bbox);

    aFont= (apa16Font *)font->devPriv[pDraw->pScreen->myNum];
    tmpCinfo= 		charinfo;
    freeCharsOut=	charsOut;
    switch (intersect) {
	case rgnIN:
		while (count>0)  {
		    xCharInfo	*metrics;
		    int		 width;	

		    if (!afChMapped(aFont,*charsOut)) {
			afMap(aFont,count,charsOut,tmpCinfo,
						aFont->afFont->pGlyphs);
		    }
		    metrics=	&tmpCinfo[0]->metrics;
		    if (afChMapped(aFont,*charsOut)) {
			width=		metrics->rightSideBearing-
					metrics->leftSideBearing;

			COPY_RECT(bltCmd,
				x+metrics->leftSideBearing+width,
				y+metrics->descent,
				afXoff(aFont,*charsOut), afYoff(aFont,*charsOut),
				width,metrics->ascent+metrics->descent);
		    }
		    x+= metrics->characterWidth;
		    count--;
		    charsOut++;
		    tmpCinfo++;
		}
		break;
	case rgnOUT:
		x += info.overallWidth;
		break;
	case rgnPART:
		while (count>0) {
		    xCharInfo	*metrics;
		    int		 width;

		    if (!afChMapped(aFont,*charsOut))
			afMap(aFont,count,charsOut,tmpCinfo, 
						   aFont->afFont->pGlyphs);
		    metrics = &tmpCinfo[0]->metrics;
		    width = metrics->rightSideBearing- metrics->leftSideBearing;

		    if (afChMapped(aFont,*charsOut)) {
			BoxRec cbox;

			cbox.y1 = y-metrics->ascent;
			cbox.y2 = y+metrics->descent;

			cbox.x2 = width +(cbox.x1=x+metrics->leftSideBearing);

			switch( (*pGC->pScreen->RectIn)(pRegion,&cbox)) {
			    case rgnOUT:
				break;
			    case rgnIN:
				COPY_RECT(bltCmd,cbox.x2, cbox.y2,
					afXoff(aFont,*charsOut),
					afYoff(aFont,*charsOut),
					width,
					metrics->ascent+metrics->descent);
				break;
			    case rgnPART:
				{
				RegionPtr pClipRgn;

				pClipRgn = (*pGC->pScreen->RegionCreate)
						    (&cbox,REGION_NUM_RECTS(pRegion));
				(*pGC->pScreen->Intersect) 
					(pClipRgn, pClipRgn, pRegion);

				for(pbox = REGION_RECTS(pClipRgn),
					nbox = REGION_NUM_RECTS(pClipRgn);
				    nbox>0;
				    nbox--, pbox++){
				  	COPY_RECT(bltCmd,
						pbox->x2, pbox->y2,
						afXoff(aFont,*charsOut)+
							pbox->x2-cbox.x2,
						afYoff(aFont,*charsOut)+
							pbox->y2-cbox.y2,
						pbox->x2-pbox->x1,
						pbox->y2-pbox->y1);
				}
				(*pGC->pScreen->RegionDestroy)(pClipRgn);
				}
				break;
				}
		    }
		    x+= metrics->characterWidth;
		    count--;
		    charsOut++;
		    tmpCinfo++;
		}
		break;
    }
    afTouch(aFont);
    DEALLOCATE_LOCAL(charinfo);
    DEALLOCATE_LOCAL(freeCharsOut);
    return (x-xOffset);
}

/***====================================================================***/

int
apa16PolyText8( pDraw, pGC, x, y, count, chars )
DrawablePtr 	 pDraw;
GCPtr		 pGC;
int		 x, y;
int		 count;
char		*chars;
{
    TRACE(("apa16PolyText8( 0x%x, 0x%x, (%d,%d), %d, 0x%x )\n",
						pDraw,pGC,x,y,count,chars));
    return(apa16PolyText( pDraw, pGC, x, y, count, chars,  Linear8Bit ));
}

/***====================================================================***/

int
apa16PolyText16( pDraw, pGC, x, y, count, chars )
DrawablePtr 	 pDraw;
GCPtr		 pGC;
int		 x, y;
int		 count;
char		*chars;
{
    TRACE(("apa16PolyText16( 0x%x, 0x%x, (%d,%d), %d, 0x%x )\n",
						pDraw,pGC,x,y,count,chars));
    if (pGC->font->pFI->lastRow==0)
	return(apa16PolyText( pDraw, pGC, x, y, count, chars,  Linear16Bit ));
    else
	return(apa16PolyText( pDraw, pGC, x, y, count, chars,  TwoD16Bit ));

}

/***====================================================================***/

void
apa16ImageText8( pDraw, pGC, x, y, count, chars )
DrawablePtr 	 pDraw;
GCPtr		 pGC;
int		 x, y;
int		 count;
char		*chars;
{
    TRACE(("apa16ImageText8( 0x%x, 0x%x, (%d,%d), %d, 0x%x )\n",
						pDraw,pGC,x,y,count,chars));
    apa16ImageText( pDraw, pGC, x, y, count, chars,  Linear8Bit );
    return;
}

/***====================================================================***/

void
apa16ImageText16( pDraw, pGC, x, y, count, chars )
DrawablePtr 	 pDraw;
GCPtr		 pGC;
int		 x, y;
int		 count;
char		*chars;
{
    TRACE(("apa16ImageText16( 0x%x, 0x%x, (%d,%d), %d, 0x%x )\n",
						pDraw,pGC,x,y,count,chars));
    if (pGC->font->pFI->lastRow==0)
	apa16ImageText( pDraw, pGC, x, y, count, chars,  Linear16Bit );
    else
	apa16ImageText( pDraw, pGC, x, y, count, chars,  TwoD16Bit );
    return;
}
