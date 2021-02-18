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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedWindow.c,v 1.3 89/11/29 16:25:36 jfc Exp Locker: jfc $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedWindow.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedWindow.c,v 1.3 89/11/29 16:25:36 jfc Exp Locker: jfc $";
#endif

#include "X.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "mfb.h"
#include "mistruct.h"
#include "regionstr.h"

#include "aedProcs.h"
#include "aedHdwr.h"
#include "ibmTrace.h"

extern WindowRec WindowTable[];

Bool aedCreateWindow(pWin)
register WindowPtr pWin;
{
    register mfbPrivWin *pPrivWin;
    int ret;
    TRACE(("aedCreateWindow(pWin= 0x%x)\n", pWin));

    ret = mfbCreateWindow(pWin);

    return ret;
}

/*
   aedCopyWindow copies only the parts of the destination that are
visible in the source.
*/


void 
aedCopyWindow(pWin, ptOldOrg, prgnSrc)
    WindowPtr pWin;
    DDXPointRec ptOldOrg;
    RegionPtr prgnSrc;
{
    RegionPtr prgnDst;
    register BoxPtr pbox;
    register int dx, dy;
    register int i, nbox;
    BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew1, pboxNew2;
				/* temporaries for shuffling rectangles */

    TRACE(("aedCopyWindow(pWin= 0x%x, ptOldOrg= 0x%x, prgnSrc= 0x%x)\n", pWin, ptOldOrg, prgnSrc));

    prgnDst = (* pWin->drawable.pScreen->RegionCreate)
      (NULL, REGION_NUM_RECTS(&pWin->borderClip));

    dx = ptOldOrg.x - pWin->drawable.x;
    dy = ptOldOrg.y - pWin->drawable.y;
    (* pWin->drawable.pScreen->TranslateRegion)(prgnSrc, -dx, -dy);
    (* pWin->drawable.pScreen->Intersect)(prgnDst, &pWin->borderClip, prgnSrc);


    pbox = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    pboxNew1 = 0;
    pboxNew2 = 0;
    if ( (pbox->y1 + dy) < pbox->y1) 
    {
        /* walk source bottom to top */

	if (nbox > 1)
	{
	    /* keep ordering in each band, reverse order of bands */
	    pboxNew1 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    if(!pboxNew1)
	    {
	        DEALLOCATE_LOCAL(pboxNew1);
	        return;
	    }
	    pboxBase = pboxNext = pbox+nbox-1;
	    while (pboxBase >= pbox)
	    {
	        while ((pboxNext >= pbox) && 
		       (pboxBase->y1 == pboxNext->y1))
		    pboxNext--;
	        pboxTmp = pboxNext+1;
	        while (pboxTmp <= pboxBase)
	        {
		    *pboxNew1++ = *pboxTmp++;
	        }
	        pboxBase = pboxNext;
	    }
	    pboxNew1 -= nbox;
	    pbox = pboxNew1;
        }
    }

    if ( (pbox->x1 + dx) < pbox->x1)
    {
	/* walk source right to left */

	if (nbox > 1)
	{
	    /* reverse order of rects in each band */
	    pboxNew2 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    pboxBase = pboxNext = pbox;
	    if(!pboxNew2)
	    {
	        DEALLOCATE_LOCAL(pboxNew2);
	        return;
	    }
	    while (pboxBase < pbox+nbox)
	    {
	        while ((pboxNext < pbox+nbox) &&
		       (pboxNext->y1 == pboxBase->y1))
		    pboxNext++;
	        pboxTmp = pboxNext;
	        while (pboxTmp != pboxBase)
	        {
		    *pboxNew2++ = *--pboxTmp;
	        }
	        pboxBase = pboxNext;
	    }
	    pboxNew2 -= nbox;
	    pbox = pboxNew2;
	}
    }

/*
    ErrorF("prgnSrc = \n");
    miprintRects(prgnSrc);
    ErrorF("prgnDst = \n");
    miprintRects(prgnDst);
*/

/*
    pbox = prgnDst->rects;
    nbox = prgnDst->numRects;
*/

    vforce();
    vikint[1] = 3;			/* copy area command */
    vikint[8] = mergexlate[GXcopy];	/* merge mode */
    vikint[9] = vikint[10] = 0;		/* area colors */
    for (i=0; i<nbox; i++, pbox++)
    {
	vikint[2] = pbox->x1 + dx;	/* src x */
	vikint[3] = pbox->y1 + dy;	/* src y */
	vikint[4] = pbox->x1;		/* dest x */
	vikint[5] = pbox->y1;		/* dest y */
	vikint[6] = pbox->x2 - pbox->x1;/* area width */
	vikint[7] = pbox->y2 - pbox->y1;/* area height */
	vikwait();
	command(10);
    }
    clear(2);

    /* free up stuff */
    if (pboxNew1)
    {
	DEALLOCATE_LOCAL(pboxNew1);
    }
    if (pboxNew2)
    {
	DEALLOCATE_LOCAL(pboxNew2);
    }

    (* pWin->drawable.pScreen->RegionDestroy)(prgnDst);
}

/* swap in correct PaintWindow* routine.  If we can use a fast output
routine (i.e. the pixmap is paddable to 32 bits), also pre-rotate a copy
of it in devPrivate.
*/

Bool
aedChangeWindowAttributes(pWin, mask)
    WindowPtr pWin;
    int mask;
{
    register int index;
    register mfbPrivWin *pPrivWin;
    extern int	ibmAllowBackingStore;
    extern	void	aedSaveAreas();
    extern	void	aedRestoreAreas();


    TRACE(("aedChangeWindowAttributes(pWin = 0x%x, mask = 0x%x)\n",pWin,mask));
    pPrivWin = (mfbPrivWin *)(pWin->devPrivates[mfbWindowPrivateIndex].ptr);

    while(mask)
    {
	index = lowbit(mask);
	mask &= ~index;
	switch(index)
	{
	  case CWBackingStore:
#ifdef AEDBACKINGSTORE
	      if (!ibmAllowBackingStore) {
		   break;
	      }
	      if (pWin->backingStore != NotUseful)
	      {
		  miInitBackingStore(pWin, aedSaveAreas, aedRestoreAreas, (void (*)()) 0);
	      }
	      else
	      {
		  miFreeBackingStore(pWin);
	      }
	      /*
	       * XXX: The changing of the backing-store status of a window
	       * is serious enough to warrant a validation, since otherwise
	       * the backing-store stuff won't work.
	       */
	      pWin->drawable.serialNumber = NEXT_SERIAL_NUMBER;
#endif
	    break;

	  case CWBackPixmap:
	      if (pWin->backgroundState == None)
		{
		  pPrivWin->fastBackground = FALSE;
		}
	      else if (pWin->backgroundState == ParentRelative)
		{
		  pPrivWin->fastBackground = FALSE;
		}
	      else 
		{
		  pPrivWin->fastBackground = TRUE;
		  pPrivWin->oldRotate.x = pWin->drawable.x;
		  pPrivWin->oldRotate.y = pWin->drawable.y;
		  if (pPrivWin->pRotatedBackground)
			  mfbDestroyPixmap(pPrivWin->pRotatedBackground);
		  pPrivWin->pRotatedBackground =
			mfbCopyPixmap(pWin->background.pixmap);
		  if(pPrivWin->pRotatedBackground->drawable.width == 32)
		    mfbXRotatePixmap(pPrivWin->pRotatedBackground,
				     pWin->drawable.x);
		  mfbYRotatePixmap(pPrivWin->pRotatedBackground,
				   pWin->drawable.y);
		}
	      break;

	  case CWBackPixel:
	      pPrivWin->fastBackground = FALSE;
	      break;

	  case CWBorderPixmap:
	      switch(pWin->backgroundState)
	      {
		case None:
		  pPrivWin->fastBorder = FALSE;
		  break;
		case ParentRelative:
		  pPrivWin->fastBorder = FALSE;
		  break;
		default:
		  pPrivWin->fastBorder = TRUE;
		  pPrivWin->oldRotate.x = pWin->drawable.x;
		  pPrivWin->oldRotate.y = pWin->drawable.y;
		  if (pPrivWin->pRotatedBorder)
			  mfbDestroyPixmap(pPrivWin->pRotatedBorder);
		  pPrivWin->pRotatedBorder =
			mfbCopyPixmap(pWin->border.pixmap);
		  if(pPrivWin->pRotatedBorder->drawable.width == 32)
		    mfbXRotatePixmap(pPrivWin->pRotatedBorder,
				     pWin->drawable.x);
		  mfbYRotatePixmap(pPrivWin->pRotatedBorder,
				   pWin->drawable.y);
		  break;
	      }
	      break;
	    case CWBorderPixel:
	      pPrivWin->fastBorder = FALSE;
	      break;
	}
    }
    pWin->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    return TRUE;
}

Bool aedPositionWindow(pWin, x, y)
register WindowPtr pWin;
int x, y;
{
    register mfbPrivWin *pPrivWin;

    pPrivWin = (mfbPrivWin *)(pWin->devPrivates[mfbWindowPrivateIndex].ptr);
    if (pWin->backgroundState == BackgroundPixmap && pPrivWin->fastBackground)
      {
	if(pPrivWin->pRotatedBackground->drawable.width == 32)
	  mfbXRotatePixmap(pPrivWin->pRotatedBackground,
			   pWin->drawable.x - pPrivWin->oldRotate.x);
	mfbYRotatePixmap(pPrivWin->pRotatedBackground,
		      pWin->drawable.y - pPrivWin->oldRotate.y);
    }

    if (!pWin->borderIsPixel && pPrivWin->fastBorder)
    {
      if(pPrivWin->pRotatedBorder->drawable.width == 32)
	mfbXRotatePixmap(pPrivWin->pRotatedBorder,
			 pWin->drawable.x - pPrivWin->oldRotate.x);
	mfbYRotatePixmap(pPrivWin->pRotatedBorder,
		      pWin->drawable.y - pPrivWin->oldRotate.y);
    }
    if ((pWin->backgroundState == BackgroundPixmap && pPrivWin->fastBackground) ||
	(!pWin->borderIsPixel && pPrivWin->fastBorder))
    {
	pPrivWin->oldRotate.x = pWin->drawable.x;
	pPrivWin->oldRotate.y = pWin->drawable.y;
    }

    /* This is the "wrong" fix to the right problem, but it doesn't really
     * cost very much.  When the window is moved, we need to invalidate any
     * RotatedPixmap that exists in any GC currently validated against this
     * window.
     */
    pWin->drawable.serialNumber = NEXT_SERIAL_NUMBER;

    return TRUE;
}
