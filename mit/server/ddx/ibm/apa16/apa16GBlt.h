#ifndef APA16_GBLTH
#define	APA16_GBLTH 1
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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16GBlt.h,v 3.0 89/11/02 15:19:03 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16GBlt.h,v $ */

#if !defined(lint) && !defined(LOCORE)  && defined(RCS_HDRS)
static char *rcsidapa16GBlt = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16GBlt.h,v 3.0 89/11/02 15:19:03 probe Exp $";
#endif

/* "@(#)apa16gblt.h	3.1 88/09/22 09:30:37" */
#define	UNCLIPPED	getleftbits((int *)pGlyph,w,tmpSrc)
#define	CLIPPED		getshiftedleftbits((int *)pGlyph,glyphCol,w,tmpSrc)

	        /* glyph all in one longword */
#define	GLYPH_NARROW(cl)	\
	        maskpartialbits(xOff, w, startmask);\
	        while (h--) {\
		    cl;\
		    *pDst = (SCRRIGHT(tmpSrc, xOff) & startmask);\
		    pGlyph += widthGlyph;\
		    pDst += APA16_WIDTH>>5;\
	        }

	        /* glyph crosses longword boundary */
#define	GLYPH_CROSSES(cl)	\
		mask32bits(xOff, w, startmask, endmask);\
	        nFirst = 32 - xOff;\
	        while (h--) {\
		    cl;\
		    *pDst = (SCRRIGHT(tmpSrc, xOff) & startmask);\
		    *(pDst+1) = (SCRLEFT(tmpSrc, nFirst) & endmask);\
		    pGlyph += widthGlyph;\
		    pDst += APA16_WIDTH>>5;\
	        }

#define	GLYPH_WIDE() {\
		int	nlw,row,left,right;\
		unsigned *tmpGlyph,tmpBits;\
		maskbits(xOff,w,startmask,endmask,nlw);\
		mask32bits(xOff,32,right,left);\
	        nFirst = 32 - xOff;\
	        while (h--) {\
		    getleftbits((tmpGlyph=(unsigned *)pGlyph), 32, tmpSrc);\
		    *pDst = (SCRRIGHT(tmpSrc, xOff) & startmask);\
		    for (row=1;row<=nlw;row++) {\
			tmpBits= (SCRLEFT(tmpSrc,nFirst)&left);\
			tmpGlyph++;\
			getleftbits(tmpGlyph,32,tmpSrc);\
			tmpBits|= SCRRIGHT(tmpSrc,xOff)&right;\
			*(pDst+row)= tmpBits;\
		    }\
		    getleftbits((tmpGlyph+1),32,tmpBits);\
		    tmpSrc= (SCRLEFT(tmpSrc,nFirst)&left)|\
					(SCRRIGHT(tmpBits,xOff)&right);\
		    *(pDst+nlw+1) = (tmpSrc & endmask);\
		    pGlyph += widthGlyph;\
		    pDst += APA16_WIDTH>>5;\
	        }\
		}

#endif /* ndef APA16_GBLTH */
