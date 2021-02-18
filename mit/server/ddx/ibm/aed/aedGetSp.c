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

#include "X.h"
#include "Xmd.h"

#include "misc.h"
#include "miscstruct.h"
#include "pixmapstr.h"
#include "pixmap.h"
#include "servermd.h"

#include "ibmTrace.h"

#include "aedHdwr.h"

void mfbGetSpans();

/* GetSpans -- for each span, gets bits from drawable starting at ppt[i]
 * and continuing for pwidth[i] bits
 * Each scanline returned will be server scanline padded, i.e., it will come
 * out to an integral number of words.
 */
void
aedGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart)
    DrawablePtr		pDrawable;	/* drawable from which to get bits */
    int			wMax;		/* largest value of all *pwidths */
    register DDXPointPtr ppt;		/* points to start copying from */
    int			*pwidth;	/* list of number of bits to copy */
    int			nspans;		/* number of scanlines to copy */
    unsigned int	*pdstStart;	/* where to put the bits */
{
    register unsigned int	*pdst;

    TRACE(("aedGetSpans(pDrawable= 0x%x, wMax= %d, ppt= 0x%x, pwidth= 0x%x, nspans= %d)\n", pDrawable, wMax, ppt, pwidth, nspans));

    if (pDrawable->type != DRAWABLE_WINDOW)
      {
	mfbGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
	return;
      }
    pdst = pdstStart;

/*******************************************************************/
    /* XXXXX
     * Copy the bits here.  Each scan line(span) must be padded to a 32 bit
     * boundary at the end.  The bits are stuffed into memory starting at
     * pdst.  The following are true at this point:
     *		ppt points to the table of starting points
     *		pwidth points to the table of widths
     *		pdst points to the buffer that the bits should be copied in to
     * NOTE:  NO clipping, alu functions, etc. must be done, just copy the bits
     */
{
int i, inlen;
vforce();

/* read back the slices */
for(i=0; i<nspans; i++)
	{
	vikint[1] = 4;		/* read command */
	vikint[2] = (short) ppt->x;	/* source x */
	vikint[3] = (short) ppt->y;	/* source y */
	vikint[4] = (short) *pwidth;	/* slice width */
	vikint[5] = 1;		/* slice height */
	vikint[6] = 0;		/* window color */
	ppt++;

	/* send command to Viking */
	command(6);

	inlen = 2* ((*pwidth + 31 ) / 32);
	pwidth++;

	vikwait(); /* wait until viking is finished reading */

	/* copy read-back data into user buffer */
	VREAD(pdst,inlen,0X4002);
	pdst += (inlen/2); /* pdst is a INT pointer */
	}
}
/*******************************************************************/

    clear(2);
    return;
}

