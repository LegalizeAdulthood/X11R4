#include "X.h"
#include "Xmd.h"
#include "Xprotostr.h"

#include "misc.h"
#include "regionstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "mistruct.h"
#include "mfb.h"
#include "maskbits.h"

#include "servermd.h"

#include "OScompiler.h"
#include "ibmTrace.h"
#include "apa16Hdwr.h"

void
apa16SetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted)
    DrawablePtr         pDrawable;
    GCPtr               pGC;
    int                 *psrc;
    register DDXPointPtr ppt;
    int                 *pwidth;
    int                 nspans;
    int                 fSorted;
{
    if (pDrawable->type == DRAWABLE_WINDOW)
	QUEUE_WAIT();
    mfbSetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
}


void
apa16PolyPoint(pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         mode;           /* Origin or Previous */
    int         npt;
    xPoint      *pptInit;
{
    if (pDrawable->type == DRAWABLE_WINDOW)
	QUEUE_WAIT();
    mfbPolyPoint(pDrawable, pGC, mode, npt, pptInit);
}


void
apa16PolyFillRect(pDrawable, pGC, nrectFill, prectInit)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nrectFill;      /* number of rectangles to fill */
    xRectangle  *prectInit;     /* Pointer to first rectangle to fill */
{
    if (pDrawable->type == DRAWABLE_WINDOW)
	QUEUE_WAIT();
    mfbPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
}


void
apa16SaveAreas(pPixmap, prgnSave, xorg, yorg)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnSave; 	/* Region to save (pixmap-relative) */
    int	    	  	xorg;	    	/* X origin of region */
    int	    	  	yorg;	    	/* Y origin of region */
{
    QUEUE_WAIT();
    mfbSaveAreas(pPixmap, prgnSave, xorg, yorg);
}


void
apa16RestoreAreas(pPixmap, prgnRestore, xorg, yorg)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnRestore; 	/* Region to restore (screen-relative)*/
    int	    	  	xorg;	    	/* X origin of window */
    int	    	  	yorg;	    	/* Y origin of window */
{
    QUEUE_WAIT();
    mfbRestoreAreas(pPixmap, prgnRestore, xorg, yorg);
}


void
apa16PushPixels(pGC, pBitMap, pDrawable, dx, dy, xOrg, yOrg)
    GCPtr	pGC;
    PixmapPtr	pBitMap;
    DrawablePtr pDrawable;
    int		dx, dy, xOrg, yOrg;
{
    if (pDrawable->type == DRAWABLE_WINDOW)
	QUEUE_WAIT();
    mfbPushPixels(pGC, pBitMap, pDrawable, dx, dy, xOrg, yOrg);
}


void
apa16PutImage(dst, pGC, depth, x, y, w, h, leftPad, format, pImage)
    DrawablePtr dst;
    GCPtr	pGC;
    int		depth, x, y, w, h;
    int leftPad;
    unsigned int format;
    int 	*pImage;
{
    if (dst->type == DRAWABLE_WINDOW)
	QUEUE_WAIT();
    mfbPutImage(dst, pGC, depth, x, y, w, h, leftPad, format, pImage);
}


void
apa16GetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart)
    DrawablePtr         pDrawable;      /* drawable from which to get bits */
    int                 wMax;           /* largest value of all *pwidths */
    register DDXPointPtr ppt;           /* points to start copying from */
    int                 *pwidth;        /* list of number of bits to copy */
    int                 nspans;         /* number of scanlines to copy */
    unsigned int        *pdstStart;     /* where to put the bits */
{
    if (pDrawable->type == DRAWABLE_WINDOW)
	QUEUE_WAIT();
    mfbGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
}


void
apa16GetImage( pDrawable, sx, sy, w, h, format, planeMask, pdstLine)
    DrawablePtr pDrawable;
    int         sx, sy, w, h;
    unsigned int format;
    unsigned long planeMask;
    pointer     pdstLine;
{
    if (pDrawable->type == DRAWABLE_WINDOW)
	QUEUE_WAIT();
    mfbGetImage( pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
}

void
apa16ZeroPolyArcSS(pDraw, pGC, narcs, parcs)
    DrawablePtr	pDraw;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    if (pDraw->type == DRAWABLE_WINDOW)
	QUEUE_WAIT();
    mfbZeroPolyArcSS(pDraw, pGC, narcs, parcs);
}
