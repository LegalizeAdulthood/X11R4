#ifndef APA16_TEXTH
#define	APA16_TEXTH 1
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
/* $Header: apa16Text.h,v 1.1 89/04/19 17:15:40 mckie Exp $ */
/* $Source: /space/X11/server/ddx/ibm/apa16/RCS/apa16Text.h,v $ */
/* "@(#)apa16text.h	3.1 88/09/22 09:31:16" */

#if !defined(lint) && !defined(LOCORE)  && defined(RCS_HDRS)
static char *rcsidapa16Text = "$Header: apa16Text.h,v 1.1 89/04/19 17:15:40 mckie Exp $";
#endif

#define CLIPBYCHAR							\
	{								\
	xCharInfo	*metrics;					\
	int		 width;						\
									\
	if (!afChMapped(aFont,*charsOut))				\
		afMap(aFont,count,charsOut,tmpCinfo, aFont->afFont->pGlyphs);\
									\
	metrics = &tmpCinfo[0]->metrics;				\
	width = metrics->rightSideBearing- metrics->leftSideBearing;	\
									\
	if (afChMapped(aFont,*charsOut))				\
		{							\
		BoxRec cbox;						\
									\
		cbox.y1 = y-metrics->ascent;				\
		cbox.y2 = y+metrics->descent;				\
									\
		/*BEWARE OF FUNNY SYNTAX IN NEXT TWO LINES*/		\
		cbox.x2 = width + /*SIC see next line*/			\
		(cbox.x1 = x+metrics->leftSideBearing);			\
		/* THINK BEFORE CHANGING */				\
									\
		switch( (*pGC->pScreen->RectIn)(pRegion,&cbox))		\
			{						\
			case rgnOUT:					\
				break;					\
			case rgnIN:					\
				COPY_RECT(bltCmd,			\
					cbox.x2, cbox.y2,		\
					afXoff(aFont,*charsOut),	\
					afYoff(aFont,*charsOut),	\
					width,				\
					metrics->ascent+metrics->descent);\
				break;					\
			case rgnPART:					\
				{					\
				RegionPtr pClipRgn;			\
									\
				pClipRgn = (*pGC->pScreen->RegionCreate)\
					(&cbox,pRegion->numRects);	\
				(*pGC->pScreen->Intersect) 		\
					(pClipRgn, pClipRgn, pRegion);	\
									\
				for(pbox = pClipRgn->rects,		\
				    nbox = pClipRgn->numRects;		\
				    nbox>0;				\
				    nbox--, pbox++){			\
				  	COPY_RECT(bltCmd,		\
						pbox->x2, pbox->y2,	\
						afXoff(aFont,*charsOut)+\
							pbox->x2-cbox.x2,\
						afYoff(aFont,*charsOut)+\
							pbox->y2-cbox.y2,\
						pbox->x2-pbox->x1,	\
						pbox->y2-pbox->y1);	\
				}					\
				(*pGC->pScreen->RegionDestroy)(pClipRgn);\
				}					\
				break;					\
				}					\
	}								\
	x+= metrics->characterWidth;					\
	count--;							\
	charsOut++;							\
	tmpCinfo++;							\
	}

#define CLIPBYSTRING							\
	{								\
	xCharInfo	*metrics;					\
	int		 width;						\
									\
	if (!afChMapped(aFont,*charsOut)) {				\
	    afMap(aFont,count,charsOut,tmpCinfo,aFont->afFont->pGlyphs);\
	}								\
	if (afChMapped(aFont,*charsOut)) {				\
	    metrics=	&tmpCinfo[0]->metrics;				\
	    width=	metrics->rightSideBearing-			\
			metrics->leftSideBearing;			\
									\
	    COPY_RECT(bltCmd,						\
		x+metrics->leftSideBearing+width, 			\
		y+metrics->descent,					\
		afXoff(aFont,*charsOut), afYoff(aFont,*charsOut),	\
		width,metrics->ascent+metrics->descent);		\
	    x+= metrics->characterWidth;				\
	}								\
	count--;							\
	charsOut++;							\
	tmpCinfo++;							\
}


#endif /* ndef APA16_TEXTH */
