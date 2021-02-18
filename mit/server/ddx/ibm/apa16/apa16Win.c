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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Win.c,v 3.2 89/11/04 16:02:24 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Win.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Win.c,v 3.2 89/11/04 16:02:24 probe Exp $";
static char sccsid[] = "@(#)apa16win.c	3.1 88/09/22 09:31:21";
#endif


#include "X.h"
#include "Xmd.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "mfb.h"
#include "mistruct.h"
#include "regionstr.h"

#include "OScompiler.h"
#include "ibmTrace.h"
#include "apa16Hdwr.h"

extern WindowPtr *WindowTable;

extern int *apa16CurrentTileBits;	/* used for invalidating scratch area */

/* UNCLEAN!
   this code calls the bitblt helper code directly.

   mfbCopyWindow copies only the parts of the destination that are
visible in the source.
*/


void 
apa16CopyWindow(pWin, ptOldOrg, prgnSrc)
    WindowPtr pWin;
    DDXPointRec ptOldOrg;
    RegionPtr prgnSrc;
{
    DDXPointPtr pptSrc;
    register DDXPointPtr ppt;
    RegionPtr prgnDst;
    register BoxPtr pbox;
    register int dx, dy;
    register int i, nbox;
    WindowPtr pwinRoot;

    TRACE(("apa16CopyWindow(pWin= 0x%x, ptOldOrg= 0x%x, prgnSrc= 0x%x)\n",
							pWin,ptOldOrg,prgnSrc));

    pwinRoot = WindowTable[pWin->drawable.pScreen->myNum];

    prgnDst = (* pWin->drawable.pScreen->RegionCreate)(NULL, 1);

    dx = ptOldOrg.x - pWin->drawable.x;
    dy = ptOldOrg.y - pWin->drawable.y;
    (* pWin->drawable.pScreen->TranslateRegion)(prgnSrc, -dx, -dy);
    (* pWin->drawable.pScreen->Intersect)(prgnDst, &pWin->borderClip, prgnSrc);

    pbox = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);
    if(!(pptSrc = (DDXPointPtr )ALLOCATE_LOCAL(nbox * sizeof(DDXPointRec))))
	return;
    ppt = pptSrc;

    i = nbox;
    Duff(i, ppt->x = pbox->x1 + dx; ppt->y = pbox->y1 + dy; ppt++; pbox++);

    apa16DoBitblt(pwinRoot, pwinRoot, GXcopy, prgnDst, pptSrc);
    DEALLOCATE_LOCAL(pptSrc);
    (* pWin->drawable.pScreen->RegionDestroy)(prgnDst);
}

/***====================================================================***/
void
apa16PaintWindow32(pWin, pRegion, what)
WindowPtr pWin;
RegionPtr pRegion;
int what;
{
    int		nbox;
    BoxPtr		pBox;
    PixmapPtr	pPix;
    mfbPrivWin 	*pPrivWin;

    TRACE(("apa16PaintWindow32(0x%x,0x%x,0x%x)\n",pWin,pRegion,what));

    pPrivWin = (mfbPrivWin *) (pWin->devPrivates[mfbWindowPrivateIndex].ptr);
    if (what==PW_BACKGROUND)
	pPix= pPrivWin->pRotatedBackground;
    else
	pPix= pPrivWin->pRotatedBorder;
    if (!pPix)
        miPaintWindow(pWin, pRegion, what);

    nbox= REGION_NUM_RECTS(pRegion);
    pBox= REGION_RECTS(pRegion);

    /* XXX */
    /* Should already be handled below XXX */
    apa16CurrentTileBits = NULL;	/* invalidate apa scratch area */
    /* XXX */

    while (nbox--) {
	apa16TileArea32(pWin, 1, pBox, GXcopy, pPix);
	pBox++;
    }
    return;
}
/***====================================================================***/

Bool
apa16ChangeWindowAttributes(pWin, mask)
    WindowPtr pWin;
    int mask;
{
mfbPrivWin	*pPrivWin;
extern int	ibmAllowBackingStore;
    TRACE(("apa16ChangeWindowAttributes( pWin= 0x%x, mask= 0x%x )\n",
								pWin,mask));

    pPrivWin = (mfbPrivWin *)(pWin->devPrivates[mfbWindowPrivateIndex].ptr);

    if (!ibmAllowBackingStore)
	mask&=(~CWBackingStore);

    if ((mask)&&(mfbChangeWindowAttributes(pWin,mask))) {
	return(TRUE);
    }
    return(FALSE);
}

