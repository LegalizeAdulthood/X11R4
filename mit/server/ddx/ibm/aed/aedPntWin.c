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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedPntWin.c,v 1.2 89/11/19 20:40:55 jfc Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedPntWin.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedPntWin.c,v 1.2 89/11/19 20:40:55 jfc Exp $";
#endif

#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "mfb.h"

#include "aedHdwr.h"
#include "ibmTrace.h"

void
aedPaintWindowSolid(pWin, pRegion, what)
    WindowPtr pWin;
    RegionPtr pRegion;
    int what;		
{
    int nbox;
    BoxPtr pbox;
    int i;
    short merge;

    TRACE(("aedPaintWindowSolid(pWin= 0x%x, pRegion= 0x%x, what= %d)\n", pWin, pRegion, what));
/*
    miprintRects(pRegion);
*/
    nbox = REGION_NUM_RECTS(pRegion);
    if ( nbox == 0 )
	return;
    pbox = REGION_RECTS(pRegion);
    if (what == PW_BACKGROUND)
    {
	if ( pWin->background.pixel )
	    merge = mergexlate[GXset];
        else
	    merge = mergexlate[GXclear];
    } 
    else
    {
	if ( pWin->border.pixel )
	    merge = mergexlate[GXset];
        else
	    merge = mergexlate[GXclear];
    }
    vforce();
    clear(2);
    vikint[ORMERGE] = merge;
    vikint[ORCLIPLX] = pbox->x1;
    vikint[ORCLIPLY] = pbox->y1;
    vikint[ORCLIPHX] = pbox->x2-1;
    vikint[ORCLIPHY] = pbox->y2-1;
    pbox++;
    nbox--;
    vikint[vikoff++] = 10;	/* tile order */
    vikint[vikoff++] = 1024;	/* rectangle width */
    vikint[vikoff++] = 800;	/* rectangle height */
    vikint[vikoff++] = 1;	/* tile height */
    vikint[vikoff++] = 1;	/* tile height */
    vikint[vikoff++] = -1;	/* tile (all ones) */

    vforce();

    vikint[VIKCMD] = 2;	/* reprocess order */
    for(i = 0; i < nbox; i++, pbox++)
    {
	vikint[ORCLIPLX] = pbox->x1;
	vikint[ORCLIPLY] = pbox->y1;
	vikint[ORCLIPHX] = pbox->x2-1;
	vikint[ORCLIPHY] = pbox->y2-1;
	command(ORDATA);
    }
    /* reset clipping window */
    clear(2);
}

void
aedPaintWindowTile(pWin, pRegion, what)
    WindowPtr pWin;
    RegionPtr pRegion;
    int what;		
{
    int nbox;
    BoxPtr pbox;
    int i;
    PixmapPtr pTile;
    int tilelen, tileHeight, tileWidth, skip, j;
    unsigned short *bits;
    mfbPrivWin *pPrivWin;

    TRACE(("aedPaintWindowTile(pWin= 0x%x, pRegion= 0x%x, what= %d)\n", pWin, pRegion, what));
    nbox = REGION_NUM_RECTS(pRegion);
    if(nbox == 0)
	return;
    pbox = REGION_RECTS(pRegion);

    pPrivWin = (mfbPrivWin *)(pWin->devPrivates[mfbWindowPrivateIndex].ptr);

    if (what == PW_BACKGROUND)
    {
	pTile = pPrivWin->pRotatedBackground;
	bits = (unsigned short *)(pPrivWin->pRotatedBackground->devPrivate.ptr);
    } 
    else
    {
	pTile = pPrivWin->pRotatedBorder;
	bits = (unsigned short *)(pPrivWin->pRotatedBorder->devPrivate.ptr);
    }

    tileWidth = pTile->drawable.width;
    tileHeight = pTile->drawable.height;
    tilelen = ( tileWidth + 15 ) / 16;

    if ((tileHeight * tilelen ) > 2000) 
    {
	aedBigStippleFillArea(pWin, nbox, pbox, mergexlate[GXcopy], pTile);
	return;
    }

    TRACE(("tileWidth = %d, tileHeight = %d\n", tileWidth, tileHeight));
    vforce();

    clear(5+(tilelen * tileHeight));

    skip = (((tileWidth-1) & 0x1f) < 16);

    vikint[ORMERGE] = mergexlate[GXcopy];
    vikint[ORCLIPLX] = pbox->x1;
    vikint[ORCLIPLY] = pbox->y1;
    vikint[ORCLIPHX] = pbox->x2-1;
    vikint[ORCLIPHY] = pbox->y2-1;
    pbox++;
    nbox--;

    vikint[vikoff++] = 10;	  		/* tile order */
    vikint[vikoff++] = 1024;		/* rectangle width */
    vikint[vikoff++] = 800;	  		/* rectangle height */
    vikint[vikoff++] = tileWidth;		/* tile width */
    vikint[vikoff++] = tileHeight;	 	/* tile height */
    for(j=0; j<tileHeight; j++)
    {
	for (i=0; i<tilelen; i++) 
	    vikint[vikoff++] = *bits++;
	if (skip) 
	    bits++; /* skip the last 16 pad bits */
    }

    vforce();

    vikint[VIKCMD] 	= 2; 		/* reprocess orders */ 
    for(i=0; i<nbox; i++, pbox++)
    {
	vikint[ORCLIPLX] = pbox->x1;
	vikint[ORCLIPLY] = pbox->y1;
	vikint[ORCLIPHX] = pbox->x2-1;
	vikint[ORCLIPHY] = pbox->y2-1;
	command(ORDATA);
    }
    clear(2);

}

void aedPaintWindow(pWin, pRegion, what)
    WindowPtr pWin;
    RegionPtr pRegion;
    int what;		
{
  register mfbPrivWin *pPrivWin;
  TRACE(("aedPaintWindow( pWin= 0x%x, pRegion= 0x%x, what = %d )\n",
	 pWin,pRegion,what));
  pPrivWin = (mfbPrivWin *)(pWin->devPrivates[mfbWindowPrivateIndex].ptr);
  switch (what) 
    {
    case PW_BACKGROUND:
      TRACE(("aedPaintWindow - background: State: %d\n",pWin->backgroundState));
      switch (pWin->backgroundState)
	{
	default:
	case None:
	  return;
	case ParentRelative:
	  do {
	    pWin = pWin->parent;
	  } while (pWin->backgroundState == ParentRelative);
	  (*pWin->drawable.pScreen->PaintWindowBackground)(pWin, pRegion, what);
	  return;
	case BackgroundPixmap:
	  aedPaintWindowTile(pWin, pRegion, what);
	  return;
	case BackgroundPixel:
	  aedPaintWindowSolid(pWin, pRegion, what);
	  return;
	}
    case PW_BORDER:
      if (pWin->borderIsPixel)
	aedPaintWindowSolid(pWin, pRegion, what);
      else
	aedPaintWindowTile(pWin, pRegion, what);
      return;
    default:
      /* ERROR */;
    }
}
