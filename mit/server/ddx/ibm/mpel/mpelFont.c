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

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelFont.c,v 6.5 89/04/29 21:38:15 jeff Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelFont.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelFont.c,v 6.5 89/04/29 21:38:15 jeff Exp $";
#endif

#include "Xproto.h"
#include "Xmd.h"
#include "misc.h"
#include "dixfont.h"
#include "dixfontstr.h"
#include "font.h"
#include "fontstruct.h"
#include "screenint.h"
#include "scrnintstr.h"

#include "ibmTrace.h"

#include "mpelHdwr.h"
#include "mpelFifo.h"
#include "mpelFont.h"

static	mpelATFRegion	*mpelRegion;
static	mpelMpelFont	mpelFonts[MPEL_ATF_NUMFONTS];

static	mpelXFont	*mpelFontBeingMapped;
static	int		 mpelFontHosed;

/***==================================================================***/

static	mpelATFRegion	*mpelSave;

void
mpelSaveFonts()
{
int	i;

    TRACE(("mpelSaveFonts()\n"));
    mpelSave= (mpelATFRegion *)Xalloc(sizeof(mpelATFRegion));
    if (mpelSave==NULL) {
	ErrorF("couldn't allocate save space in mpelSaveFonts\n");
    }
    bcopy(mpelRegion,mpelSave,sizeof(mpelATFRegion));
    for (i=0;i<MPEL_ATF_NUMFONTS;i++) {
	mpelFonts[i].myRegion=   mpelSave;
	mpelFonts[i].myHeader=	&mpelSave->atfHeaders[i];
	mpelFonts[i].myLookup= 	(unsigned short int *) mpelSave->lookup[i];
    }
    mpelRegion= mpelSave;
    return;
}

/***==================================================================***/

void
mpelRestoreFonts()
{
int	i;

    TRACE(("mpelRestoreFonts()\n"));
    mpelRegion= (mpelATFRegion *)MPEL_ATF_REGION;
    MPEL_ATF_LOC_PTR= mpelAddr(MPEL_ATF_REGION);
    if (mpelSave!=NULL) {
	bcopy(mpelSave,mpelRegion,sizeof(mpelATFRegion));
	Xfree(mpelSave);
	mpelSave= NULL;
    }
    else {
	ErrorF("mpelRestoreFonts called with nothing saved\n");
    }
    for (i=0;i<MPEL_ATF_NUMFONTS;i++) {
	mpelFonts[i].myRegion=   mpelRegion;
	mpelFonts[i].myHeader=	&mpelRegion->atfHeaders[i];
	mpelFonts[i].myLookup= 	(unsigned short int *) mpelRegion->lookup[i];
    }
    return;
}

/***==================================================================***/

static void
mpelMFontInit(mFont,w,h)
    mpelMpelFont	*mFont;
    int			w,h;
{
    int			i;

    TRACE(("mpelMFontInit(mFont=0x%x,w=%d,h=%d)\n",mFont,w,h));

    mFont->nextFreePt=	1;
    for (i=0;i<256;i++) {
	mFont->myLookup[i]= i+1;
    }
    mFont->myLookup[255]= 0;
    mFont->xFonts=	  NULL;
    mFont->myHeader->fd.width=	w;
    mFont->myHeader->fd.height=	h;
    mFont->myHeader->fd.baseline= (((h+3)/4)*4)-h;
    mFont->myHeader->fd.bitsperchar= 16*((w+3)/4)*((h+3)/4);
    return;
}

/***==================================================================***/

static void
mpelRegionInit(rgn)
    mpelATFRegion	*rgn;
{
    int			i;

    TRACE(("mpelRegionInit(sRgn=0x%x)\n",rgn));

    rgn->usage.nextFreeBand=	0;
    rgn->usage.nextUse=	0;
    for (i=0;i<MPEL_ATF_NUMBANDS;i++) {
	rgn->bands[i].nextBand=	i+1;
	rgn->bands[i].nextPtrn=	0;
    }
    rgn->bands[MPEL_ATF_MAXBAND].nextBand= MPEL_ATF_NOBAND;
    for (i=0;i<MPEL_ATF_NUMFONTS;i++) {
	mpelMFontInit(&rgn->usage.myFonts[i],0,0);
    }
    return;
}

/***==================================================================***/

void
mpelInitFonts()
{
    mpelATFLocTable	*loc;
    mpelATFHeader	*fnt;
    unsigned short int *lookup;
    int			 i;
    unsigned long int tmp;

    TRACE(("mpelInitFonts()\n"));

    mpelRegion= (mpelATFRegion *)MPEL_ATF_REGION;
    bzero(mpelRegion,sizeof(mpelATFRegion));

    /* initialize locations table */
    loc= &mpelRegion->atfLocations;
    loc->numEntries=	MPEL_ATF_NUMFONTS;
    for (i=0;i<MPEL_ATF_NUMFONTS;i++) {
	fnt=		&mpelRegion->atfHeaders[i];
	lookup=		(unsigned short int *) mpelRegion->lookup[i];

	loc->entries[i].fid=	100+i;
	tmp= (unsigned long int)mpelAddr(fnt);
	loc->entries[i].addrHigh=	(tmp>>16)&0xffff;
	loc->entries[i].addrLow=	(tmp&0xffff);

	fnt->size=	(unsigned long int)(((char *)(&lookup[256]))-((char *)fnt));
	fnt->class= 		2;
    	fnt->fontId=		100+i;
    	fnt->fontStyle=		0;
    	fnt->fontAttribute=	0;
    	fnt->fd.fontTotalChars=	256;
    	fnt->fd.tblWordSize=	256;
    	fnt->fd.baseline=	0;
    	fnt->fd.capline=	0;
    	fnt->fd.width=		0;
    	fnt->fd.height=		0;
    	fnt->fd.bitsperchar=	0;
	fnt->fd.underlineTop=	0;
	fnt->fd.underlineBase=	0;
	fnt->fd.monoPitch=	0x80;
    	fnt->fd.lookupTblOffset= (unsigned long int)(((CARD16 *)lookup)-((CARD16 *)fnt));

	mpelFonts[i].myRegion=	mpelRegion;
	mpelFonts[i].myHeader=	fnt;
	mpelFonts[i].myLookup=	lookup;
    }

    mpelRegion->usage.myFonts=	&mpelFonts[0];
    mpelRegionInit(mpelRegion);
    return;
}

/***==================================================================***/

static mpelXFont *
mpelGetFont(width,height,nChars)
    int	width;
    int	height;
    int nChars;
{
    mpelMpelFont	*mFont,*freeMFont= NULL;
    mpelATFHeader	*hdr= NULL;
    int			i,found= FALSE;
    int			bitsPerChar;
    mpelXFont		*xFont;

    TRACE(("mpelGetFont(width=%d,height=%d,nChars= %d)\n",width,height,nChars));

    bitsPerChar= ((width+3)/4)*((height+3)/4)*16;

    if (bitsPerChar/16>MPEL_BAND_PTRNSIZE)
	return(NULL);	/* too big */
    
    for (i=0;i<MPEL_ATF_NUMFONTS;i++) {
	mFont= &mpelFonts[i];
	hdr= mFont->myHeader;
	if (hdr->fd.width==width) {
	    found= TRUE;
	    break;
	}
	else if ((!freeMFont)&&(hdr->fd.bitsperchar==0)) {
	    freeMFont= mFont;
	}
    }
    if ((!found)&&(!freeMFont))
	return(NULL);
/* we assume mFont is consistent, so init and free had
 * better take care of initializing it
 */
    if (!found) {
	mFont=	freeMFont;
	mpelMFontInit(mFont,width,height);
	hdr=	mFont->myHeader;
    }

    xFont= (mpelXFont *)Xalloc(sizeof(mpelXFont));
    xFont->myMpelFont=	mFont;
    xFont->mpelId=	hdr->fontId;
    xFont->numMapped=	0;
    xFont->width=	width;
    xFont->height=	height;
    xFont->nChars=	nChars;
    xFont->mpelMap=	(CARD8 *)Xalloc(nChars * sizeof(CARD8));
    bzero(xFont->mpelMap,(nChars*sizeof(CARD8)));
    xFont->currentBand=	MPEL_ATF_NOBAND;
    xFont->lastUse=	mFont->myRegion->usage.nextUse++;
    xFont->nextXFont=	mFont->xFonts;
    mFont->xFonts=	xFont;
    return xFont ;
}

/***==================================================================***/

static void
mpelCleanXFont(xFont)
    mpelXFont	*xFont;
{
    mpelMpelFont	*mFont= xFont->myMpelFont;
    mpelATFRegion	*rgn= mFont->myRegion;
    int			 band,i;

    TRACE(("mpelCleanXFont(0x%x)\n",xFont));

    MPELNoOp(2,&MPELNoOpData);
    MPELWaitFifo();
    /* First, free any bands the font is holding */
    band= xFont->currentBand;
    while (band!=MPEL_ATF_NOBAND) {
	rgn->bands[band].nextPtrn=	0;
	if (rgn->bands[band].nextBand==MPEL_ATF_NOBAND) {
	    rgn->bands[band].nextBand= rgn->usage.nextFreeBand;
	    rgn->usage.nextFreeBand= xFont->currentBand;
	    band= MPEL_ATF_NOBAND;
	}
	else band= rgn->bands[band].nextBand;
    }
    xFont->currentBand= MPEL_ATF_NOBAND;

    /* then free any code points */
    for (i=0;i<xFont->nChars;i++) {
	if (xFont->mpelMap[i]) {
	    mFont->myLookup[xFont->mpelMap[i]]= mFont->nextFreePt;
	    mFont->nextFreePt= xFont->mpelMap[i];
	    xFont->mpelMap[i]= 0;
	    xFont->numMapped--;
	}
    }
    if (xFont->numMapped!=0) {
	ErrorF("WSGO!! %d still mapped in CleanXFont\n",xFont->numMapped);
	xFont->numMapped= 0;
    }
    if (xFont==mpelFontBeingMapped)
	mpelFontHosed= TRUE;
    return ;
}

/***==================================================================***/

static void 
mpelDeleteXFont(xFont)
   mpelXFont	*xFont;
{
    mpelXFont		**lastFont;
    mpelMpelFont	 *mFont= xFont->myMpelFont;

    TRACE(("mpelDeleteXFont(0x%x)\n",xFont));

    /* clean up structures first */
    mpelCleanXFont(xFont);

    /* Remove xFont from mFont's list of fonts... */
    lastFont= &mFont->xFonts;
    while ((*lastFont)&&(*lastFont!=xFont)) {
	lastFont= &((*lastFont)->nextXFont);
    }
    if (*lastFont)	*lastFont= xFont->nextXFont;
    else 		ErrorF("WSGO!! mpelDeleteXFont couldn't find xFont\n");

    /* if xFont was the last X font using mFont, initialize mFont */
    if (mFont->xFonts==NULL)
	mpelMFontInit(mFont,0,0);

    if (xFont->mpelMap)
	Xfree(xFont->mpelMap);
    Xfree(xFont);
    return ;
}

/***==================================================================***/

static void
mpelNeedCodePoints(xFont)
    mpelXFont	*xFont;
{
    mpelMpelFont	*mFont= xFont->myMpelFont;
    mpelXFont		*oldestFont,*tmpFont;
    int			oldest;

    TRACE(("mpelNeedCodePoints(0x%x)\n",xFont));
    oldest= xFont->lastUse= ++mFont->myRegion->usage.nextUse;
    oldestFont= xFont;

    tmpFont= mFont->xFonts;
    while (tmpFont) {
	if ((tmpFont->lastUse<oldest)&&(tmpFont->numMapped)) {
	    oldest= tmpFont->lastUse;
	    oldestFont= tmpFont;
	}
	tmpFont= tmpFont->nextXFont;
    }
    mpelCleanXFont(oldestFont);
    if (!mFont->nextFreePt) {
	ErrorF("WSGO!! CleanXFont didn't free points\n");
    }
    return ;
}

/***==================================================================***/

static void
mpelNeedBands(xFont)
    mpelXFont	*xFont;
{
    mpelATFRegion	*rgn= xFont->myMpelFont->myRegion;
    mpelMpelFont	*mFont;
    mpelXFont		*oldestFont,*tmpFont;
    int			 oldest,i;

    TRACE(("mpelNeedBands(0x%x)\n",xFont));
    oldest= xFont->lastUse= ++rgn->usage.nextUse;
    oldestFont= xFont;
    for (i=0;i<MPEL_ATF_NUMFONTS;i++) {
	mFont= &rgn->usage.myFonts[i];
	tmpFont= mFont->xFonts;
	while (tmpFont) {
	    if ((tmpFont->lastUse<oldest)&&
				(tmpFont->currentBand!=MPEL_ATF_NOBAND)) {
		oldest= tmpFont->lastUse;
		oldestFont= tmpFont;
	    }
	    tmpFont= tmpFont->nextXFont;
	}
    }
    mpelCleanXFont(oldestFont);
    if (rgn->usage.nextFreeBand==MPEL_ATF_NOBAND) {
	ErrorF("WSGO!! CleanXFont didn't free bands\n");
    }
    return ;
}

/***==================================================================***/

static void
mpelGetBand(xFont)
    mpelXFont	*xFont;
{
    mpelMpelFont	*mFont= xFont->myMpelFont;
    mpelATFRegion	*rgn= mFont->myRegion;
    int			 band;

    TRACE(("mpelGetBand(xFont=0x%x)\n",xFont));

    if (rgn->usage.nextFreeBand==MPEL_ATF_NOBAND) 
	mpelNeedBands(xFont);

    band= rgn->usage.nextFreeBand;
    rgn->usage.nextFreeBand= rgn->bands[band].nextBand;
    rgn->bands[band].nextBand= xFont->currentBand;
    rgn->bands[band].nextPtrn= 0;
    xFont->currentBand= band;
    return ;
}

/***==================================================================***/

static unsigned char
mpelMapChar(xFont,ch,ptrn)
    mpelXFont	*xFont;
    unsigned short	 ch;
    unsigned char	*ptrn;
{
    mpelMpelFont	*mFont= xFont->myMpelFont;
    mpelATFBand		*bands= &mFont->myRegion->bands[0];
    int			 newPt=0,band= MPEL_ATF_NOBAND;
    CARD16		*ptrnOut;

    TRACE(("mpelMapChar(xFont=0x%x,ch=%d,ptrn=0x%x)\n",xFont,ch,ptrn));

    /* find a code point */
    if (mFont->nextFreePt==0) {
	mpelNeedCodePoints(xFont);
	if (mpelFontHosed)
	   return(0);
    }
    newPt= mFont->nextFreePt;
    mFont->nextFreePt=	mFont->myLookup[newPt];
    xFont->mpelMap[ch]=	newPt;
    xFont->numMapped++;

    /* find someplace to stick the pattern */
    while (band==MPEL_ATF_NOBAND) {
	if (xFont->currentBand==MPEL_ATF_NOBAND) {
	    mpelGetBand(xFont);
	    if (mpelFontHosed)
		return(0);
	}
	band= xFont->currentBand;
	if (bandSpaceLeft((&bands[band]))<fontWordSize(mFont)) {
	    mpelGetBand(xFont);
	    if (mpelFontHosed)
		return(0);
	    band= xFont->currentBand;
	}
    }

    ptrnOut= bandNextSpace(&bands[band]);
    mpelTransmogrify(fontWidth(xFont),fontHeight(xFont),ptrn,ptrnOut);
    bands[band].nextPtrn+= fontWordSize(mFont);
    mFont->myLookup[newPt]= ((CARD16 *)ptrnOut)-((CARD16 *)mFont->myHeader)-
						sizeof(mpelATFHeader)/2;
    return newPt ;
}


/***==================================================================***/

Bool
mpelRealizeFont( pScr, pFont )
    ScreenPtr	pScr;
    FontPtr	pFont;
{
    int			index = pScr->myNum;
    FontInfoPtr		pfi = pFont->pFI;
    CharInfoPtr		maxb = &pfi->maxbounds;
    CharInfoPtr		minb = &pfi->minbounds;
    mpelXFont		*xFont;
    int			nChars;

    TRACE(("mpelRealizeFont(pScr= 0x%x,pFont= 0x%x)\n",pScr,pFont));

    if ( maxb->metrics.leftSideBearing == minb->metrics.leftSideBearing &&
	      maxb->metrics.leftSideBearing == 0 &&
	      maxb->metrics.rightSideBearing==minb->metrics.rightSideBearing &&
	      maxb->metrics.characterWidth==minb->metrics.characterWidth &&
	      maxb->metrics.ascent == minb->metrics.ascent &&
	      maxb->metrics.descent == minb->metrics.descent ) {
	nChars= pfi->lastCol-pfi->firstCol+1;
	if (pfi->lastRow>0)
	    nChars *= (pfi->lastRow-pfi->firstRow+1);
	xFont=mpelGetFont(	maxb->metrics.characterWidth,
				maxb->metrics.ascent+maxb->metrics.descent,
				nChars);
	if (xFont) {
	    pFont->devPriv[index]= (pointer)xFont;
	    return TRUE ;
	}
    }
    return mfbRealizeFont(pScr,pFont) ;
}

/***==================================================================***/

Bool
mpelUnrealizeFont( pScr, pFont)
    ScreenPtr	pScr;
    FontPtr	pFont;
{
    int index= pScr->myNum;

    TRACE(("mpelUnrealizeFont(pScr=0x%x,pFont=0x%x)\n"));
    if (((unsigned long int)pFont->devPriv[index])>40) {
	mpelDeleteXFont((mpelXFont *)pFont->devPriv[index]);
	pFont->devPriv[index]= NULL;
    }
    return TRUE ;
}

/***==================================================================***/

static int
mpelMFontUsage(mFont)
    mpelMpelFont	*mFont;
{
    mpelATFRegion	*rgn= mFont->myRegion;
    mpelXFont		*xFont;
    int			i,
			nXFonts=0,
			nBands=0,
			nChars= 0,
			bandSpaceUsed=0,
			usage=0;

    if (!mFont->myHeader->fd.bitsperchar) {
	ErrorF("inactive\n");
	return 0 ;
    }
    xFont= mFont->xFonts;
    while (xFont) {
	nXFonts++;
	i= xFont->currentBand;
	nChars+= xFont->numMapped;
	while (i!=MPEL_ATF_NOBAND) {
	     nBands++;
	     bandSpaceUsed+= rgn->bands[i].nextPtrn;
	     i= rgn->bands[i].nextBand;
	     if (nBands>MPEL_ATF_NUMBANDS) {
		ErrorF("WSGO! Too many bands\n");
		return MPEL_ATF_NUMBANDS ;
	     }
	}
	xFont= xFont->nextXFont;
    }

    if (nBands) 
    	usage= (100*bandSpaceUsed)/(nBands*MPEL_BAND_PTRNSIZE);

    ErrorF("[%d bits], %d X fonts, %d chars, %d bands (%%%d usage)\n",
					mFont->myHeader->fd.bitsperchar,
					nXFonts,nChars,nBands,usage);
    return nBands ;
}

/***==================================================================***/

#if defined(DEBUG) && !defined(NDEBUG)
static void
mpelATFUsage( rgn )
    mpelATFRegion	*rgn;
{
    int		i,
		count=0,
		bandKnt=0;

    ErrorF("Megapel ATF Region usage (0x%x):\n",rgn);
    ErrorF("nextFreeBand:	%d\n",rgn->usage.nextFreeBand);
    ErrorF("nextUse:		%d\n",rgn->usage.nextUse);
    ErrorF("Annotation fonts:\n");
    for (i=0;i<MPEL_ATF_NUMFONTS;i++) {
	ErrorF("font %d: ",i);
	bandKnt+= mpelMFontUsage(&rgn->usage.myFonts[i]);
    }

    for (count=0,i=rgn->usage.nextFreeBand;i!=MPEL_ATF_NOBAND;
						i=rgn->bands[i].nextBand) {
	count++;
    }
    ErrorF("free Bands: %d\n",count);
    bandKnt+= count;
    if (bandKnt<MPEL_ATF_NUMBANDS) {
	ErrorF("Warning!! Missing bands (%d should be %d)\n",bandKnt,
							MPEL_ATF_NUMBANDS);
    }
    if (bandKnt>MPEL_ATF_NUMBANDS) {
	ErrorF("Warning!! Too many bands (%d should be %d)\n",bandKnt,
							MPEL_ATF_NUMBANDS);
    }
    return ;
}
#endif

/***==================================================================***/

void
mpelRemap(xFont,nCh,inStr,outStr,ppci,pglyphBase)
    mpelXFont		*xFont;
    int			 nCh;
    unsigned 	short	*inStr;
    unsigned	char	*outStr;
    CharInfoPtr 	*ppci;		/* array of character info */
    unsigned	char	*pglyphBase;	/* start of array of glyphs */
{
    int	i;

    TRACE(("mpelRemap(xFont=0x%x,nCh=%d,inStr=%s,outStr=%s)\n",xFont,nCh,
							inStr,outStr));

#ifdef DEBUG
    if ( !strncmp( inStr, "dUmp", 4 ) )
	mpelATFUsage( xFont->myMpelFont->myRegion ) ;
#endif /* DEBUG */

    mpelFontBeingMapped= xFont;
    mpelFontHosed= FALSE;

    for (i=0;i<nCh;i++) {
	if (mpelFontHosed) {
	    i=0;
	    mpelFontHosed= FALSE;
	}
	if (xFont->mpelMap[inStr[i]]!=0) {
	    outStr[i]= xFont->mpelMap[inStr[i]];
	}
	else {
    	    outStr[i]=
		mpelMapChar(xFont,inStr[i],pglyphBase+ppci[i]->byteOffset);
	}
	xFont->lastUse= ++xFont->myMpelFont->myRegion->usage.nextUse;
    }
    return ;
}
