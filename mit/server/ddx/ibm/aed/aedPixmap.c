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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedPixmap.c,v 1.2 89/11/19 20:36:12 jfc Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedPixmap.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedPixmap.c,v 1.2 89/11/19 20:36:12 jfc Exp $";
#endif

#include "X.h"
#include "pixmap.h"
#include "pixmapstr.h"
#include "mfb.h"
#include "maskbits.h"

/* Rotates pixmap pPix by w pixels to the right on the screen. Assumes that
 * words are 32 bits wide, and that the least significant bit appears on the
 * left.
 */
aedXRotatePixmap(pPix, rw)
    PixmapPtr	pPix;
    register int rw;
{
    register long	*pw, *pwFinal, *pwNew;
    register unsigned long	t;
    int			size;

    if ( (pPix == NullPixmap) || ( rw == 0 ) )
        return;

    pw = (long *)pPix->devPrivate.ptr;
    rw %= pPix->drawable.width;
    if (rw < 0)
	rw += pPix->drawable.width;
    if ( rw == 0 )
	return;
    if(pPix->drawable.width == 32)
    {
        pwFinal = pw + pPix->drawable.height;
	while(pw < pwFinal)
	{
	    t = *pw;
	    *pw++ = SCRRIGHT(t, rw) | 
		    (SCRLEFT(t, (32-rw)) & endtab[rw]);
	}
    }
    else
    {
	ErrorF("X internal error: trying to rotate odd-sized pixmap (AED).\n");
#if 0
	pwNew = (long *) Xalloc( pPix->drawable.height * pPix->devKind);

	/* o.k., divide pw (the pixmap) in two vertically at (w - rw)
	 * pick up the part on the left and make it the right of the new
	 * pixmap.  then pick up the part on the right and make it the left
	 * of the new pixmap.
	 * now hook in the new part and throw away the old. All done.
	 */
	size = PixmapWidthInPadUnits(pPix->drawable.width, 1) /* >> 2*/;
        mfbQuickBlt(pw, pwNew, 0, 0, rw, 0, pPix->drawable.width - rw,
		    pPix->drawable.height, size, size);
        mfbQuickBlt(pw, pwNew, pPix->drawable.width - rw, 0, 0, 0, rw,
		    pPix->drawable.height, size, size);
	pPix->devPrivate.ptr = (pointer) pwNew;
	Xfree((char *) pw);
#endif
    }

}


void
aedCopyRotatePixmap(psrcPix, ppdstPix, xrot, yrot)
    register PixmapPtr psrcPix, *ppdstPix;
    int	xrot, yrot;
{
    register PixmapPtr pdstPix;

    if ((pdstPix = *ppdstPix) &&
	(pdstPix->devKind == psrcPix->devKind) &&
	(pdstPix->drawable.height == psrcPix->drawable.height))
    {
	bcopy((char *)psrcPix->devPrivate.ptr,
	      (char *)pdstPix->devPrivate.ptr,
	      psrcPix->drawable.height * psrcPix->devKind);
	pdstPix->drawable.width = psrcPix->drawable.width;
	pdstPix->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    }
    else
    {
	if (pdstPix)
	    mfbDestroyPixmap(pdstPix);
	*ppdstPix = pdstPix = mfbCopyPixmap(psrcPix);
	if (!pdstPix)
	    return;
    }
    mfbPadPixmap(pdstPix);
    if (xrot)
	aedXRotatePixmap(pdstPix, xrot);
    if (yrot)
	mfbYRotatePixmap(pdstPix, yrot);
}
