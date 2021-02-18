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

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmFntUtil.c,v 9.1 88/10/31 23:09:45 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmFntUtil.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/common/RCS/ibmFntUtil.c,v 9.1 88/10/31 23:09:45 paul Exp $";
#endif

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "dixfontstr.h"
#include "fontstruct.h"

#include "OScompiler.h"

void
GetGlyphsAndChars(font, count, chars, fontEncoding, 
				glyphcount, glyphs, indices)
    FontPtr font;
    unsigned long count;
    register unsigned char *chars;
    FontEncoding fontEncoding;
    unsigned long *glyphcount;	/* RETURN */
    CharInfoPtr glyphs[];	/* RETURN */
    CARD16	indices[];	/* RETURN */
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
		    indices[i] = c;
		    glyphs[i] = ci;
		}
		n = count;
	    } else {
		for (i=0; i < count; i++) {
    
		    c = (*chars++) - firstCol;
		    if (c < numCols) {
			ci = &pCI[c];
			if (ci->exists) {
			    indices[n] = c;
			    glyphs[n++] = ci; 
			    continue;
			}
		    }
    
		    if (cDef < numCols) {
			ci = &pCI[cDef];
			if (ci->exists) {
			    indices[n] = cDef;
			    glyphs[n++] = ci;
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
		    indices[i] = c;
		    glyphs[i] = ci;
		}
		n = count;
	    } else {
		for (i=0; i < count; i++) {
    
		    c = *chars++ << 8;
		    c = (c | *chars++) - firstCol;
		    if (c < numCols) {
			ci = &pCI[c];
			if (ci->exists) {
			    indices[n] = c;
			    glyphs[n++] = ci; 
			    continue;
			}
		    }
    
		    if (cDef < numCols) {
			ci = &pCI[cDef];
			if (ci->exists)  {
			    indices[n] = cDef;
			    glyphs[n++] = ci;
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
			indices[n] = c;
			glyphs[n++] = ci; 
			continue;
		    }
		}

		row = (chDefault >> 8)-firstRow;
		col = (chDefault & 0xff)-firstCol;
		if ((row < numRows) && (col < numCols)) {
		    c = row*numCols + col;
		    ci = &pCI[c];
		    if (ci->exists) {
			indices[n] = c;
			glyphs[n++] = ci;
		    }
		}
	    }
	    break;
    }
    *glyphcount = n;
    return ;
}

/***====================================================================***/

#define	CM_UNCHECKED	0
#define	CM_VARIABLE	1
#define	CM_CONSTANT	2

#define	CHECK_EXTENTS(f,ei,i,ci) \
	if (ei==CM_VARIABLE) {\
	    (i)->overallAscent= MAX(	(i)->overallAscent,\
					(ci)->metrics.ascent);\
	    (i)->overallDescent= MAX(	(i)->overallDescent,\
					(ci)->metrics.descent);\
	    (i)->overallLeft=	MIN(	(i)->overallLeft,\
			info->overallWidth+(ci)->metrics.leftSideBearing);\
	    (i)->overallRight=	MAX(	(i)->overallRight,\
			info->overallWidth+(ci)->metrics.rightSideBearing);\
	    (i)->overallWidth+= (ci)->metrics.characterWidth;\
	}\
	else if (ei==CM_UNCHECKED) {\
	    (i)->overallAscent= (ci)->metrics.ascent;\
	    (i)->overallDescent= (ci)->metrics.descent;\
	    (i)->overallLeft=	(ci)->metrics.leftSideBearing;\
	    (i)->overallRight=	(ci)->metrics.rightSideBearing;\
	    (i)->overallWidth=	(ci)->metrics.characterWidth;\
	    if ((f)->pFI->constantMetrics)	ei= CM_CONSTANT;\
	    else				ei= CM_VARIABLE;\
	}

void
GetGlyphsCharsAndExtents(font, count, chars, fontEncoding, 
				glyphcount, glyphs, indices, info)
    FontPtr font;
    unsigned long count;
    register unsigned char *chars;
    FontEncoding fontEncoding;
    unsigned long *glyphcount;	/* RETURN */
    CharInfoPtr glyphs[];	/* RETURN */
    CARD16	indices[];	/* RETURN */
    ExtentInfoRec	*info;	/* RETURN */
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
	     int		extentType= CM_UNCHECKED;

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
		    indices[i] = c;
		    glyphs[i] = ci;
		    CHECK_EXTENTS(font,extentType,info,ci);
		}
		n = count;
	    } else {
		for (i=0; i < count; i++) {
    
		    c = (*chars++) - firstCol;
		    if (c < numCols) {
			ci = &pCI[c];
			if (ci->exists) {
			    indices[n] = c;
			    glyphs[n++] = ci; 
			    CHECK_EXTENTS(font,extentType,info,ci);
			    continue;
			}
		    }
    
		    if (cDef < numCols) {
			ci = &pCI[cDef];
			if (ci->exists) {
			    indices[n] = cDef;
			    glyphs[n++] = ci;
			    CHECK_EXTENTS(font,extentType,info,ci);
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
		    indices[i] = c;
		    glyphs[i] = ci;
		    CHECK_EXTENTS(font,extentType,info,ci);
		}
		n = count;
	    } else {
		for (i=0; i < count; i++) {
    
		    c = *chars++ << 8;
		    c = (c | *chars++) - firstCol;
		    if (c < numCols) {
			ci = &pCI[c];
			if (ci->exists) {
			    indices[n] = c;
			    glyphs[n++] = ci; 
			    CHECK_EXTENTS(font,extentType,info,ci);
			    continue;
			}
		    }
    
		    if (cDef < numCols) {
			ci = &pCI[cDef];
			if (ci->exists)  {
			    indices[n] = cDef;
			    glyphs[n++] = ci;
			    CHECK_EXTENTS(font,extentType,info,ci);
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
			indices[n] = c;
			glyphs[n++] = ci; 
			CHECK_EXTENTS(font,extentType,info,ci);
			continue;
		    }
		}

		row = (chDefault >> 8)-firstRow;
		col = (chDefault & 0xff)-firstCol;
		if ((row < numRows) && (col < numCols)) {
		    c = row*numCols + col;
		    ci = &pCI[c];
		    if (ci->exists) {
			indices[n] = c;
			glyphs[n++] = ci;
			CHECK_EXTENTS(font,extentType,info,ci);
		    }
		}
	    }
	    break;
    }

    info->drawDirection= font->pFI->drawDirection;
    info->fontAscent= font->pFI->fontAscent;
    info->fontDescent= font->pFI->fontDescent;
    if (extentType==CM_UNCHECKED) {
	info->overallAscent= info->overallDescent= 0;
	info->overallLeft= info->overallRight= 0;
	info->overallWidth= 0;
    }
    else if (extentType==CM_CONSTANT) {
	info->overallRight += info->overallWidth * (count -1);
	info->overallWidth *= count;
    }
    *glyphcount = n;
    return ;
}
