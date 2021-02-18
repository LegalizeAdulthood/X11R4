#ifndef APA16_FONTH
#define	APA16_FONTH 1
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

/* $Header: apa16Font.h,v 1.1 89/04/19 17:15:34 mckie Exp $ */
/* $Source: /space/X11/server/ddx/ibm/apa16/RCS/apa16Font.h,v $ */
/* "@(#)apa16font.h	3.1 88/09/22 09:30:35" */

#if !defined(lint) && !defined(LOCORE)  && defined(RCS_HDRS)
static char *rcsidapa16Font = "$Header: apa16Font.h,v 1.1 89/04/19 17:15:34 mckie Exp $";
#endif


#define	AF_NCHARS	256

typedef	struct	A16BAND	{
	struct A16FONT	*afbFont;	/* apa16 font associated w/band */
	int		 afbX1,afbY1;	/* upper left of band */
	int		 afbX2,afbY2;	/* lower right of band */
	struct	A16BAND	*afbPrev;	/* Prev band (for coalesce) */
	struct	A16BAND *afbNext;	/* Next band (for coalesce) */
	unsigned	 afbLast;	/* Last use of band */
	unsigned 	 afbUsed;	/* extent of band used */
	unsigned 	 afbClearing;	/* A clear font area may be pending */
	int		 afbNChars;	/* maximum number of chars in font */
	struct AFBI {
		unsigned short	offsetCh;	/* offset of character 'n' */
		unsigned short	widthCh;	/* width of character 'n' */
	} *afbInfo;
} apa16Band;

#define	afbTouch(pAB)		{if (pAB) \
				    (pAB)->afbLast= afbCounter;}
#define	afbWidth(pAB)		((pAB)?(pAB)->afbX2-(pAB)->afbX1:0)
#define	afbHeight(pAB)		((pAB)?(pAB)->afbY2-(pAB)->afbY1:0)
#define	afbXoffCh(pAB,ndx)	((pAB)&&(pAB->afbInfo)?(pAB)->afbInfo[ndx].offsetCh:-100)
#define	afbYoffCh(pAB,ndx)	((pAB)?(pAB)->afbY2:-100)
#define	afbTouchCh(pAB,ndx)	{if ((pAB)&&(pAB->afbInfo)) \
				    (pAB)->afbInfo[ndx].usageCh= afbCounter;}
#define	afbMapped(pAB,ndx)	((pAB)&&(pAB->afbInfo)&&((pAB)->afbInfo[ndx].widthCh!=0))

extern	unsigned	afbCounter;

typedef	struct	A16FONT {
	FontPtr		 afFont;	/* X font associated with apa16 font */
	unsigned	 afHeight;	/* height of band needed by font */
	unsigned	 afNChars;	/* number of characters in font */
	apa16Band	*afBand;	/* band assigned to font */
} apa16Font;

#define	afXoff(pa,ch)		(afbXoffCh((pa)->afBand,ch))
#define	afYoff(pa,ch)		(afbYoffCh((pa)->afBand,ch))
#define	afChMapped(pa,ch)	(afbMapped((pa)->afBand,ch))
#define	afTouch(pa)		{afbTouch((pa)->afBand);}

extern	Bool		 afRealize();
extern	Bool		 afUnrealize();
extern	int		 afMap();
#endif /* ndef APA16_FONTH */
