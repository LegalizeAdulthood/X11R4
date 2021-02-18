/***********************************************************
		Copyright IBM Corporation 1988

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
/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Font.c,v 3.1 89/11/04 16:01:57 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Font.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Font.c,v 3.1 89/11/04 16:01:57 probe Exp $";
static char sccsid[] = "@(#)apa16font.c	3.1 88/09/22 09:30:33";
#endif

#include "X.h"
#include "Xproto.h"
#include "Xmd.h"
#include "scrnintstr.h"
#include "dixfontstr.h"
#include "dixfont.h"
#include "fontstruct.h"
#include "font.h"

#include "mfb.h"

#include "OScompiler.h"

#include "apa16Hdwr.h"
#include "apa16Font.h"

#include "ibmTrace.h"

/***====================================================================***/
/***				BAND STUFF				***/
/***====================================================================***/

copy_font_area()
{
    unsigned cmd;

    APA16_GET_CMD(ROP_RECT_COPY,GXcopy,cmd);
    COPY_RECT(cmd,1023,750,
	      1023,1023,1023,1023-768);
}

#define	USELESS_HEIGHT	8
#define	NREGIONS	4

static	apa16Band	afbRegions[NREGIONS] = {
    {
	NULL,				/* afbFont (unallocated) */
	0,		1009,		/* lower scroll area */
	APA16_WIDTH-1,	1023,		/* lower scroll area */
	NULL,		NULL,		/* no next or previous */
    },
    {
	NULL,				/* afbFont (unallocated) */
	0,		768,		/* upper scroll area */
	APA16_WIDTH-1,	783,		/* upper scroll area */
	NULL,		NULL,		/* no next or previous */
    },
    {
	NULL,				/* afbFont (unallocated) */
	0,		FONT_TOP,	/* upper left */
	APA16_WIDTH-1,	FONT_BOTTOM,	/* lower right */
	NULL,		NULL		/* no next or previous band */
	},
    {
	NULL,				/* afbFont (unallocated) */
	MAXCURSORS*2*CURSOR_WIDTH,	CURSOR_AREA_TOP,
	STAGE_X_OFFSET,			CURSOR_AREA_BOTTOM,
	NULL,		NULL		/* no next or previous band */
	}

};

	unsigned 	 afbCounter=	1;

extern	apa16Band	*afbFree();

/***====================================================================***/

afbDebug()
{
    int		region;
    apa16Band	*pAB;

    for (region=0;region<NREGIONS;region++) {
	ErrorF("Region %d\n",region);
	pAB= &afbRegions[region];
	while (pAB) {
	    ErrorF("   Band: 0x%x= [(%d,%d)-->(%d,%d)], extent used %d, %d chars in font\n",
		   pAB,pAB->afbX1,pAB->afbY1,pAB->afbX2,pAB->afbY2,
		   pAB->afbUsed,pAB->afbNChars);
	    ErrorF("         (prv= 0x%x, nxt= 0x%x, fnt= 0x%x)\n",
		   pAB->afbPrev,pAB->afbNext,pAB->afbFont);
	    pAB= pAB->afbNext;
	}
    }
}
 
/***====================================================================***/

void
apa16ReinitializeFonts()
{
    int		 region;
    apa16Band	*pAB;
    unsigned	 cmd;

    TRACE(("apa16ReinitializeFonts()\n"));
    APA16_GET_CMD(ROP_RECT_FILL,RROP_BLACK,cmd);
    for (region=0;region<NREGIONS;region++) {
	for (pAB= &afbRegions[region];pAB!=NULL;pAB=pAB->afbNext) {
	    FILL_RECT(cmd,pAB->afbX2,pAB->afbY2,
		      pAB->afbX2-pAB->afbX1,pAB->afbY2-pAB->afbY1);
	    pAB->afbClearing= TRUE;
	}
    }
}

/***====================================================================***/

void
apa16InvalidateFonts()
{
    int		 region;
    apa16Band	*pAB;

    TRACE(("apa16InvalidateFonts()\n"));
    for (region=0;region<NREGIONS;region++) {
	for (pAB= &afbRegions[region];pAB!=NULL;pAB=pAB->afbNext) {
	    if (pAB->afbFont==NULL) continue;
	    pAB->afbUsed= pAB->afbX1;
	    bzero(pAB->afbInfo,sizeof(struct AFBI)*pAB->afbNChars);
	}
    }
}

/***====================================================================***/

static	apa16Band *
afbFindFreeable(height)
int	height;
{
    unsigned	 oldest=0xffffffff;
    int		 region,rHt;
    apa16Band	*pAB,*pOld=NULL;

    TRACE(("afbFindFreeable(%d)\n",height));
    for (region=0;region<NREGIONS;region++) {
	for (pAB= &afbRegions[region];pAB!=NULL;pAB=pAB->afbNext) {
	    rHt= pAB->afbY2-pAB->afbY1;
	    if (rHt>=height) {
	        if (pAB->afbFont==NULL) {
		    if (height>0)	return(pAB);
		}
		else if ((pOld==NULL)||(pAB->afbLast<oldest)) {
		    pOld= pAB;
		    oldest= pAB->afbLast;
	    	}
	    }
	}
    }
    return(pOld);
}

/***====================================================================***/

static	apa16Band *
afbForce(height)
register const int height;
{
    register apa16Band	*pAB= NULL;

    TRACE(("afbForce(%d)\n",height));
    while (!pAB) {
	pAB= afbFindFreeable(height);
	if (pAB)	pAB= afbFree(pAB);
	else		afbFree(afbFindFreeable(0));
    }
    return(pAB);
}

/***====================================================================***/

static	apa16Band *
afbFind(pFont)
apa16Font	*pFont;
{	
    register apa16Band	*pAB = (apa16Band *) 0 ;
    register int		region;
    int		height= pFont->afHeight;
    unsigned	cmd;

    TRACE(("afbFind(0x%x)\n",pFont));
    /* step through regions */
    for (region=0;(region<NREGIONS)&&(!pAB);region++) {
	/* looking for unallocated band
	 * tall enough for pFont
	 */
	for ( pAB= &afbRegions[region];
	     ((pAB)&&((pAB->afbFont!=NULL)||(pAB->afbY2-pAB->afbY1)<height));
	     pAB= pAB->afbNext)		/* Spin */;
    }
    if (!pAB) {				/* if we didn't find one, force some free */
	pAB= afbForce(height);
    }
    pAB->afbFont= pFont;
    /*
     * is the band we found tall enough to split?
     */
    if ((pAB->afbY2-pAB->afbY1)-height>USELESS_HEIGHT) {
	register apa16Band	*pNew;

	pNew= (apa16Band *)Xalloc(sizeof(apa16Band));
	pNew->afbFont=	NULL;
	pNew->afbX1=	pAB->afbX1;
	pNew->afbY1=	pAB->afbY1+height;
	pNew->afbX2=	pAB->afbX2;
	pNew->afbY2=	pAB->afbY2;
	pNew->afbPrev=	pAB;
	pNew->afbNext=	pAB->afbNext;
	pNew->afbInfo=	NULL;
	if (pAB->afbNext) pAB->afbNext->afbPrev = pNew;
	pAB->afbNext=	pNew;
	pAB->afbY2=	pAB->afbY1+height;
    }

    pAB->afbUsed=	pAB->afbX1;
    pAB->afbLast=	afbCounter;
    pAB->afbNChars=	pFont->afNChars;
    pAB->afbInfo= (struct AFBI *)Xalloc(sizeof(struct AFBI)*pAB->afbNChars);
    bzero(pAB->afbInfo,(sizeof(struct AFBI)*pAB->afbNChars));

    APA16_GET_CMD(ROP_RECT_FILL,RROP_BLACK,cmd);
    FILL_RECT(cmd,pAB->afbX2,pAB->afbY2,
	      pAB->afbX2-pAB->afbX1,pAB->afbY2-pAB->afbY1);
    pAB->afbClearing= TRUE;
    return(pAB);
}

/***====================================================================***/

apa16Band	*
afbFree(pAB)
register apa16Band	*pAB;
{
    register apa16Band	*pTmp;

    TRACE(("afbFree(0x%x)\n",pAB));

    if ((!pAB)||(pAB->afbFont==NULL))	/* already free */
	return(pAB);

    pAB->afbFont->afBand= NULL;		/* note that it's free */

    if (pAB->afbInfo) {			/* free info, if allocated */
	Xfree(pAB->afbInfo);
	pAB->afbInfo= NULL;
    }

    /* coalesce with previous band, if possible */
    pTmp= pAB->afbPrev;
    if ((pTmp!=NULL)&&(pTmp->afbFont==NULL)) {
	pTmp->afbY2=	pAB->afbY2;
	pTmp->afbNext=	pAB->afbNext;
	if (pAB->afbNext)
	    pAB->afbNext->afbPrev= pTmp;
	Xfree(pAB);
	pAB= pTmp;
    }

    /* coalesce with following band, if possible */
    pTmp= pAB->afbNext;
    if ((pTmp!=NULL)&&(pTmp->afbFont==NULL)) {
	pAB->afbY2=	pTmp->afbY2;
	pAB->afbNext=	pTmp->afbNext;
	if (pTmp->afbNext)
	    pTmp->afbNext->afbPrev= pAB;
	Xfree(pTmp);
    }
    pAB->afbFont=	NULL;
    pAB->afbUsed=	pAB->afbX1;
    pAB->afbLast=	0;
    pAB->afbNChars=	0;
    return(pAB);
}

/***====================================================================***/

int
afbGetRect(pAB,ch,width)
register apa16Band	*pAB;
int		 ch;
int		 width;
{
    unsigned	cmd;

    TRACE(("afbGetRect(0x%x,%d,%d)\n",pAB,ch,width));

    APA16_GET_CMD(ROP_RECT_FILL,RROP_BLACK,cmd);

    while (pAB->afbUsed+width>pAB->afbX2) {
	if (pAB->afbLast==afbCounter) {
	    return(0);
	}
	pAB->afbUsed= pAB->afbX1;
	if (pAB->afbInfo)
	    bzero(pAB->afbInfo,pAB->afbNChars*sizeof(struct AFBI));

	FILL_RECT(cmd,pAB->afbX2,pAB->afbY2,
		  pAB->afbX2-pAB->afbX1,pAB->afbY2-pAB->afbY1);
	pAB->afbClearing= TRUE;
    }
    pAB->afbUsed+=			width;
    pAB->afbInfo[ch].widthCh=		width;
    pAB->afbInfo[ch].offsetCh=		pAB->afbUsed;
    pAB->afbLast=			afbCounter;
    return(1);
}

/***====================================================================***/
/*** 			    END OF BAND STUFF 				***/
/***====================================================================***/

#define	RASTER_WIDTH(m)		((m)->rightSideBearing-(m)->leftSideBearing)
#define	RASTER_HEIGHT(m)	((m)->ascent+(m)->descent)

/***====================================================================***/

Bool
afRealizeFont( pScr, pFont )
ScreenPtr	pScr;
FontPtr		pFont;
{
    int		index=	pScr->myNum;
    FontInfoPtr	pfi=	pFont->pFI;
    xCharInfo	*maxb=	&pfi->maxbounds.metrics;
    xCharInfo	*minb=	&pfi->minbounds.metrics;
    register apa16Font	*pAF;

    TRACE(("afRealizeFont(0x%x,0x%x)\n",pScr,pFont));

    if (   maxb->rightSideBearing - minb->leftSideBearing > 32 /* too wide */
	|| RASTER_HEIGHT(maxb)> 24	/* too tall */
	|| minb->characterWidth<0	/* too weird */
	|| pfi->drawDirection != FontLeftToRight ) { /* too weird */
	return(mfbRealizeFont( pScr, pFont ));
    }
    /* cache this puppy!! */

    TRACE(("using apa16 cache for font on screen %d\n",index));
    pAF= (apa16Font *)Xalloc(sizeof(apa16Font));
    pFont->devPriv[index]= (pointer)pAF;
    pAF->afFont=	pFont;
    pAF->afHeight=	RASTER_HEIGHT(maxb);
    pAF->afNChars=	pfi->lastCol-pfi->firstCol+1;
    if (pfi->lastRow>0)
	pAF->afNChars*=((pfi->lastRow-pfi->firstRow)+1);
    pAF->afBand=	NULL;
    return (TRUE);
}

/***====================================================================***/

Bool
afUnrealizeFont( pScr, pFont )
ScreenPtr	pScr;
FontPtr		pFont;
{
    register apa16Font	*pAF;

    TRACE(("afUnrealizeFont(0x%x,0x%x)\n",pScr,pFont));
    if (((int)pFont->devPriv[pScr->myNum])<40) {
	return(mfbUnrealizeFont( pScr, pFont ));
    }
    TRACE(("blanking font from apa16 cache on screen %d\n",pScr->myNum));
    pAF= (apa16Font *)pFont->devPriv[pScr->myNum];
    if (pAF) {
	if (pAF->afBand)
	    afbFree(pAF->afBand);
	Xfree(pAF);
    }
    pFont->devPriv[pScr->myNum]= NULL;
    return(TRUE);
}

/***====================================================================***/

int
afMap(pAF,nChars,pStr,cInfo,pGlyphs)
register apa16Font	*pAF;
int		 nChars;
unsigned short	*pStr;
CharInfoPtr	*cInfo;
unsigned char	*pGlyphs;
{
    register int		ch;
    register apa16Band	*pAB;
    int		 	i;

    TRACE(("afMap(0x%x,%d,0x%x)\n",pAF,nChars,pStr));
    if (!pAF->afBand) {
	pAF->afBand=	afbFind(pAF);
    }
    afbCounter++;
    pAB= pAF->afBand;
    for (i=0;i<nChars;i++) {
	ch= pStr[i];
	if (!afbMapped(pAB,ch)) {
	    int	width = RASTER_WIDTH((&cInfo[i]->metrics));
	    if ((width>0)&&(width<=afbWidth(pAB))) {
		if (!afbGetRect(pAB,ch,width))
		    return i ;
		if (pAB->afbClearing) {
		    pAB->afbClearing= FALSE;
		    QUEUE_WAIT();
		}
		apa16PutGlyph( afbXoffCh( pAB, ch ), afbYoffCh( pAB, ch ),
			      cInfo[i], pGlyphs ) ;
	    }
	}
    }
    return nChars ;
}
