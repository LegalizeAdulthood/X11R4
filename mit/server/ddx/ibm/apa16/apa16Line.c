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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Line.c,v 3.1 89/11/04 16:03:36 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Line.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Line.c,v 3.1 89/11/04 16:03:36 probe Exp $";
static char sccsid[] = "@(#)apa16line.c	3.1 88/09/22 09:30:53";
#endif

#include "X.h"
#include "Xmd.h"

#include "misc.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"

#include "mistruct.h"

#include "OScompiler.h"

#include "ibmTrace.h"

#include "mfb.h"
#include "maskbits.h"

#include "apa16Hdwr.h"

/* single-pixel lines on a monochrome frame buffer
   written by drewry, with debts to kelleher, mullis, carver, sanborn

   we loop through the clip rectangles on the outside and the line in
the inner loop because we expect there to be few clip rectangles.
*/

/* cohen-sutherland clipping codes
   right and bottom edges of clip rectangles are not
included
*/ 

#define OUTCODES(result, x, y, pbox) \
    if (x < pbox->x1) \
	result |= OUT_LEFT; \
    else if (pbox->x2 <= x) \
	result |= OUT_RIGHT; \
    if (y < pbox->y1) \
	result |= OUT_ABOVE; \
    else if (pbox->y2 <= y) \
	result |= OUT_BELOW; \

/*
    clipping lines presents a few difficulties, most of them
caused by numerical error or boundary conditions.
    when clipping the endpoints, we want to use the floor
operator after division rather than truncating towards 0; truncating
towards 0 gives different values for the clipped endpoint, based
on the direction the line is drawn, because of the signs of
the operands.
    ??? Brian will explain why all the swapping and so on ???
    since we don't want to draw the points on the lower and right
edges of the clip regions, we adjust them in by one if the point
being clipped is the first point in the line.  (if it's the last
point, it won't get drawn anyway.)
    another way would be to sort the points, and always do
everything in the same direction, but then we'd have to keep
track of which was the original first point of the segment, for
the specil-case last endpoint calculations.
    another other way to solve this is to run the bresenham until
the line enters the clipping rectangle.  this is simple, but means
you might run the full line once for each clip rectangle.
    (See ACM Transactions on Graphics, April 1983, article by 
Rob Pike for perhaps a better solution (although there are several
details to fill in.

    the macro mod() is defined because pcc generates atrocious code
for the C % operator; the macro can be turned off and the inline
utility can generate something better.
*/

#define mod(a, b) ((a)%(b))

/* there are NO parentheses here; the user is thus encoureaged to
think very carefully about how this is called.
*/
#define floordiv(dividend, divisor, quotient) \
quotient = dividend/divisor; \
if ( (quotient < 0) && (mod(dividend, divisor)) ) \
    quotient--;


/* single-pixel solid lines */
void 
apa16LineSS(pDrawable, pGC, mode, npt, pptInit)
DrawablePtr pDrawable;
GC *pGC;
int mode;		/* Origin or Previous */
int npt;		/* number of points */
DDXPointPtr pptInit;
{
    int nrect;
    register BoxPtr pbox;
    int nline;			/* npt - 1 */
    int nptTmp;
    register DDXPointPtr ppt;	/* pointer to list of translated points */

    DDXPointRec pt1;
    DDXPointRec pt2;
    DDXPointRec pt1Orig;		/* spare copy of points */

    register int spos;		/* inness/outness of starting point */
    register int fpos;		/* inness/outness of final point */

    int *addrl;			/* address of longword with first point */
    int xorg, yorg;		/* origin of window */

    int dx, dy;

    int clipDone;		/* flag for clipping loop */
    int swapped;		/* swapping of endpoints when clipping */

    register int cliptmp;	/* for endpoint clipping */
    register int clipquot;	/* quotient from floordiv() */
    int	lastx= -1,lasty= -1;
    unsigned cmd;

    RegionPtr cclip;

    TRACE(("apa16LineSS(pDrawable= 0x%x, pGC= 0x%x, mode= 0x%x, npt= %d, pptInit= 0x%x)\n",pDrawable,pGC,mode,npt,pptInit));

    if((((mfbPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)->rop)==RROP_NOP)
	return;

    if (pDrawable->type==DRAWABLE_PIXMAP) {
	mfbLineSS(pDrawable,pGC,mode,npt,pptInit);
	return;
    }

    APA16_GET_CMD(ROP_VECTOR,
		  (((mfbPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)->rop),
		  cmd);

    cclip = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip;
    pbox = REGION_RECTS(cclip);
    nrect = REGION_NUM_RECTS(cclip);

    xorg = pDrawable->x;
    yorg = pDrawable->y;

    addrl = (int *)(((PixmapPtr)
		     (pDrawable->pScreen->devPrivate))->devPrivate.ptr);

    /* translate the point list */
    ppt = pptInit;
    nptTmp = npt;
    if (mode == CoordModeOrigin)
    {
	while(nptTmp--)
	{
	    ppt->x += xorg;
	    ppt->y += yorg;
	    ppt++;
	}
    }
    else
    {
	ppt->x += xorg;
	ppt->y += yorg;
	nptTmp--;
	while(nptTmp--)
	{
	    ppt++;
	    ppt->x += (ppt-1)->x;
	    ppt->y += (ppt-1)->y;
	}
    }

    while(nrect--)
    {
	nline = npt;
	ppt = pptInit;

	while(--nline)
	{
	    pt1 = *ppt++;
	    pt2 = *ppt;
	    pt1Orig = pt1;

	    dx = pt2.x - pt1.x;
	    dy = pt2.y - pt1.y;

	    /* calculate clipping */
	    swapped = 0;
	    clipDone = 0;
	    do
	    {
		DDXPointRec ptTemp;
		int temp;

	        spos = 0;
	        fpos = 0;

	        OUTCODES(spos, pt1.x, pt1.y, pbox)
	        OUTCODES(fpos, pt2.x, pt2.y, pbox)
        
	        if (spos & fpos)	/* line entirely out */
		    clipDone = -1;
	        else if ((spos | fpos) == 0) /* line entirely in */
	        {
		    if (swapped)
		    {
		        ptTemp = pt1;
		        pt1 = pt2;
		        pt2 = ptTemp;
		    }
		    clipDone = 1;
	        }
	        else	/* line is clipped */
	        {
		    /* clip start point */
		    if (spos == 0)
		    {
		        ptTemp = pt1;
		        pt1 = pt2;
		        pt2 = ptTemp;

		        temp = spos;
		        spos = fpos;
		        fpos = temp;
			swapped = !swapped;
		    }

		    if (spos & OUT_BELOW)
		    {
			cliptmp = ((pbox->y2-1) - pt1Orig.y) * dx;
			floordiv(cliptmp, dy, clipquot);
		        pt1.x = pt1Orig.x + clipquot;
		        pt1.y = pbox->y2-1;
		    }
		    else if (spos & OUT_ABOVE)
		    {
			cliptmp = (pbox->y1 - pt1Orig.y) * dx;
			floordiv(cliptmp, dy, clipquot);
		        pt1.x = pt1Orig.x + clipquot;
		        pt1.y = pbox->y1;
		    }
		    else if (spos & OUT_RIGHT)
		    {
			cliptmp = ((pbox->x2-1) - pt1Orig.x) * dy;
			floordiv(cliptmp, dx, clipquot);
		        pt1.y = pt1Orig.y + clipquot;
		        pt1.x = pbox->x2-1;
		    }
		    else if (spos & OUT_LEFT)
		    {
			cliptmp = (pbox->x1 - pt1Orig.x) * dy;
			floordiv(cliptmp, dx, clipquot);
		        pt1.y = pt1Orig.y + clipquot;
		        pt1.x = pbox->x1;
		    }
	        }
	    } while (!clipDone);

	    if (clipDone == -1)
		continue;

	    if ((pt1.x==lastx)&&(pt1.y==lasty)) {
		POLY_VECTOR(cmd,pt2.x,pt2.y);
	    }
	    else {
		DRAW_VECTOR(cmd,pt1.x,pt1.y,pt2.x,pt2.y);
	    }
	    lastx= pt2.x;
	    lasty= pt2.y;
	} /* while (nline--) */
	pbox++;
    } /* while (nbox--) */

    /* paint the last point if it's not the same as the first
       and hasn't been clipped
       after the main loop is done, ppt points to the last point in
       the list
    */

    if ((ppt->x != pptInit->x) ||
	(ppt->y != pptInit->y))
    {
        RegionPtr cclip;
      
	cclip = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->
	  pCompositeClip;
	pbox = REGION_RECTS(cclip);
	nrect = REGION_NUM_RECTS(cclip);

	pt1 = *ppt;

	QUEUE_WAIT();
	while (nrect--)
	{
	    if ((pt1.x >= pbox->x1) &&
		(pt1.y >= pbox->y1) &&
		(pt1.x <  pbox->x2) &&
		(pt1.y <  pbox->y2))
	    {
		addrl += (pt1.y * (APA16_WIDTH>>5)) + (pt1.x >> 5);
		switch( ((mfbPrivGC *)
			 (pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop)
		{
		    case RROP_BLACK:
		        *addrl &= rmask[pt1.x & 0x1f];
			break;
		    case RROP_WHITE:
		        *addrl |= mask[pt1.x & 0x1f];
			break;
		    case RROP_INVERT:
		        *addrl ^= mask[pt1.x & 0x1f];
			break;
		}
		break;
	    }
	    else
		pbox++;
	}
      
      }
    return ;
}

	/*
	 * apa16PolySegment
	 */

void 
apa16PolySegment(pDrawable, pGC, nseg, pSegIn)
DrawablePtr	 pDrawable;
GC		*pGC;
int		 nseg;
xSegment	*pSegIn;
{
    int nrect;
    register BoxPtr pbox;
    int nline;			/* npt - 1 */
    register xSegment *pSeg;	/* pointer to list of translated points */

    DDXPointRec pt1;
    DDXPointRec pt2;
    DDXPointRec pt1Orig;		/* spare copy of points */

    register int spos;		/* inness/outness of starting point */
    register int fpos;		/* inness/outness of final point */

    int xorg, yorg;		/* origin of window */

    int dx, dy;

    int clipDone;		/* flag for clipping loop */
    int swapped;		/* swapping of endpoints when clipping */

    register int cliptmp;	/* for endpoint clipping */
    register int clipquot;	/* quotient from floordiv() */
    int	lastx= -1,lasty= -1;
    unsigned cmd;
    RegionPtr cclip;

    TRACE(("apa16PolySegment(pDrawable= 0x%x, pGC= 0x%x, nseg= %d, pSegIn= 0x%x)\n",pDrawable,pGC,nseg,pSegIn));

    if((((mfbPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)->rop)==RROP_NOP)
	return;

    if (pDrawable->type==DRAWABLE_PIXMAP) {
	miPolySegment(pDrawable,pGC,nseg,pSegIn);
	return;
    }

    APA16_GET_CMD(ROP_VECTOR,
		  (((mfbPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)->rop),
		  cmd);
    cclip = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip;
    pbox = REGION_RECTS(cclip);
    nrect = REGION_NUM_RECTS(cclip);

    xorg = pDrawable->x;
    yorg = pDrawable->y;

    while(nrect--)
    {
	nline = nseg;
	pSeg = pSegIn;

	while(nline--)
	{
	    pt1.x=	pSeg->x1+xorg;	pt1.y= pSeg->y1+yorg;
	    pt2.x=	pSeg->x2+xorg;	pt2.y=	pSeg->y2+yorg;
	    pt1Orig = pt1;
	    pSeg++;

	    dx = pt2.x - pt1.x;
	    dy = pt2.y - pt1.y;

	    /* calculate clipping */
	    swapped = 0;
	    clipDone = 0;
	    do
	    {
		DDXPointRec ptTemp;
		int temp;

	        spos = 0;
	        fpos = 0;

	        OUTCODES(spos, pt1.x, pt1.y, pbox)
	        OUTCODES(fpos, pt2.x, pt2.y, pbox)
        
	        if (spos & fpos)	/* line entirely out */
		    clipDone = -1;
	        else if ((spos | fpos) == 0) /* line entirely in */
	        {
		    if (swapped)
		    {
		        ptTemp = pt1;
		        pt1 = pt2;
		        pt2 = ptTemp;
		    }
		    clipDone = 1;
	        }
	        else	/* line is clipped */
	        {
		    /* clip start point */
		    if (spos == 0)
		    {
		        ptTemp = pt1;
		        pt1 = pt2;
		        pt2 = ptTemp;

		        temp = spos;
		        spos = fpos;
		        fpos = temp;
			swapped = !swapped;
		    }

		    if (spos & OUT_BELOW)
		    {
			cliptmp = ((pbox->y2-1) - pt1Orig.y) * dx;
			floordiv(cliptmp, dy, clipquot);
		        pt1.x = pt1Orig.x + clipquot;
		        pt1.y = pbox->y2-1;
		    }
		    else if (spos & OUT_ABOVE)
		    {
			cliptmp = (pbox->y1 - pt1Orig.y) * dx;
			floordiv(cliptmp, dy, clipquot);
		        pt1.x = pt1Orig.x + clipquot;
		        pt1.y = pbox->y1;
		    }
		    else if (spos & OUT_RIGHT)
		    {
			cliptmp = ((pbox->x2-1) - pt1Orig.x) * dy;
			floordiv(cliptmp, dx, clipquot);
		        pt1.y = pt1Orig.y + clipquot;
		        pt1.x = pbox->x2-1;
		    }
		    else if (spos & OUT_LEFT)
		    {
			cliptmp = (pbox->x1 - pt1Orig.x) * dy;
			floordiv(cliptmp, dx, clipquot);
		        pt1.y = pt1Orig.y + clipquot;
		        pt1.x = pbox->x1;
		    }
	        }
	    } while (!clipDone);

	    if (clipDone == -1)
		continue;

	    if ((pt1.x==lastx)&&(pt1.y==lasty)) {
		POLY_VECTOR(cmd,pt2.x,pt2.y);
	    }
	    else {
		DRAW_VECTOR(cmd,pt1.x,pt1.y,pt2.x,pt2.y);
	    }
	    lastx= pt2.x;
	    lasty= pt2.y;
	} /* while (nline--) */
	pbox++;
    } /* while (nbox--) */
}

void
apa16DashLine( pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int mode;		/* Origin or Previous */
    int npt;		/* number of points */
    DDXPointPtr pptInit;
{
    int nseg;			/* number of dashed segments */
    miDashPtr pdash;		/* list of dashes */
    miDashPtr pdashInit;

    int nboxInit;
    int nbox;
    BoxPtr pboxInit;
    BoxPtr pbox;
    int nptTmp;
    DDXPointPtr ppt;		/* pointer to list of translated points */

    DDXPointRec pt1;
    DDXPointRec pt2;

    unsigned int oc1;		/* outcode of point 1 */
    unsigned int oc2;		/* outcode of point 2 */

    int xorg, yorg;		/* origin of window */

				/* these are all per original line */
    int adx;			/* abs values of dx and dy */
    int ady;
    int signdx;			/* sign of dx and dy */
    int signdy;
    int axis;			/* major axis */

    int clipDone;		/* flag for clipping loop */
    DDXPointRec pt1Orig;	/* unclipped start point */
    int clip1, clip2;		/* clippedness of the endpoints */

    int clipdx, clipdy;		/* difference between clipped and
				   unclipped start point */
    unsigned cmd;
    unsigned fg_cmd;
    unsigned bg_cmd;
    int	lastx= -1,lasty= -1;
    RegionPtr cclip;

    if ( pGC->alu == GXnoop )
	return;

    if (pDrawable->type != DRAWABLE_WINDOW)
    {
	mfbLineSD( pDrawable, pGC, mode, npt, pptInit ) ;
	return ;
    }

    if (pGC->lineStyle == LineOnOffDash)
    {
	if ((((mfbPrivGC *)pGC->devPrivates[mfbGCPrivateIndex].ptr)->rop)==
	    RROP_NOP) 
	  return;
	APA16_GET_CMD( ROP_VECTOR, 
		      ((mfbPrivGC *)
		       (pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop, 
		      cmd );
    }
    else
      {
	APA16_GET_CMD( ROP_VECTOR, 
		      ((mfbPrivGC *)
		       (pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop, 
		      fg_cmd );
	APA16_GET_CMD( ROP_VECTOR, 
		      apa16ReduceRop(pGC->alu, pGC->bgPixel), 
		      bg_cmd ) ;
	/* Original call
	APA16_GET_CMD( ROP_VECTOR, 
		      mfbReduceRop(pGC->alu, pGC->bgPixel), 
		      bg_cmd ) ;
		      */
      }

    cclip = ((mfbPrivGC *)
	     (pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip;
    pboxInit = REGION_RECTS(cclip);
    nboxInit = REGION_NUM_RECTS(cclip);

    if ( !nboxInit )
	return ;

    xorg = pDrawable->x;
    yorg = pDrawable->y;

    /* translate the point list */
    ppt = pptInit;
    nptTmp = npt;
    if (mode == CoordModeOrigin)
    {
	while(nptTmp--)
	{
	    ppt->x += xorg;
	    (ppt++)->y += yorg;
	}
    }
    else
    {
	ppt->x += xorg;
	ppt->y += yorg;
	nptTmp--;
	while(nptTmp--)
	{
	    ppt++;
	    ppt->x += (ppt-1)->x;
	    ppt->y += (ppt-1)->y;
	}
    }
    pdash = miDashLine(npt, pptInit, 
		       pGC->numInDashList, pGC->dash, pGC->dashOffset,
		       &nseg);
    pdashInit = pdash;

    for (; --nseg >= 0; pdash++)
    {
	if (pdash->newLine)
	{
#if HCVERSION > 1
	    pt1Orig = *pptInit;
	    pt1 = *pptInit++;
#else
	    pt1Orig = pt1 = *pptInit++;
#endif
	    pt2 = *pptInit;
	    adx = pt2.x - pt1.x;
	    ady = pt2.y - pt1.y;
	    signdx = sign(adx);
	    signdy = sign(ady);
	    adx = ABS(adx);
	    ady = ABS(ady);
	    axis = (adx > ady) ? X_AXIS : Y_AXIS;
	}
	if (pGC->lineStyle == LineOnOffDash) {
	    if (pdash->which == ODD_DASH)
		continue;
	}
	else if (pGC->lineStyle == LineDoubleDash) {
	    /* use a different color for odd dashes */
	    cmd = (pdash->which == EVEN_DASH) ? fg_cmd : bg_cmd ;
	}

	nbox = nboxInit;
	pbox = pboxInit;
	for ( ; nbox--; pbox++)
	{
	    BoxRec box;

	    clipDone = 0;
	    pt1 = pdash->pt;
	    pt2 = (pdash+1)->pt;
	    box.x1 = pbox->x1;
	    box.y1 = pbox->y1;
	    box.x2 = pbox->x2-1;
	    box.y2 = pbox->y2-1;
	    clip1 = 0;
	    clip2 = 0;

	    oc1 = 0;
	    oc2 = 0;
	    OUTCODES(oc1, pt1.x, pt1.y, pbox);
	    OUTCODES(oc2, pt2.x, pt2.y, pbox);

	    if (oc1 & oc2)
		clipDone = -1;
	    else if ((oc1 | oc2) == 0)
		clipDone = 1;
	    else /* have to clip */
		clipDone = mfbClipLine(pbox, box,
				       &pt1Orig, &pt1, &pt2, 
				       adx, ady, signdx, signdy, axis,
				       &clip1, &clip2);

	    if (clipDone != -1) {
		if ((pt1.x==lastx)&&(pt1.y==lasty)) {
		    POLY_VECTOR(cmd,pt2.x,pt2.y);
	        }
	        else {
		    DRAW_VECTOR(cmd,pt1.x,pt1.y,pt2.x,pt2.y);
	        }
	        lastx= pt2.x;
	        lasty= pt2.y;
		/* if segment is unclipped, skip remaining rectangles 
		if (!(clip1 || clip2))
			break;
			*/
	} /* for ( ; nbox--; pbox++) */
      }
    } /* for */

    Xfree(pdashInit);
    return ;
}
