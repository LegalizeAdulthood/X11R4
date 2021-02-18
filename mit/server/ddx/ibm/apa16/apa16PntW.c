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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16PntW.c,v 3.0 89/11/02 15:19:51 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16PntW.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16PntW.c,v 3.0 89/11/02 15:19:51 probe Exp $";
static char sccsid[] = "@(#)apa16pntw.c	3.1 88/09/22 09:31:05";
#endif

#include "X.h"
#include "Xmd.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "mfb.h"

#include "OScompiler.h"
#include "ibmTrace.h"

#include "apa16Hdwr.h"

static void apa16PaintWindowSolid();

/*
NOTE
   PaintArea32() doesn't need to rotate the tile, since
mfbPositionWIndow() and mfbChangeWindowAttributes() do it.
*/

void
apa16PaintWindow(pWin, pRegion, what)
    WindowPtr	pWin;
    RegionPtr	pRegion;
    int		what;
{
    register mfbPrivWin	*pPrivWin;
    TRACE(("apa16PaintWindow( pWin= 0x%x, pRegion= 0x%x, what = %d )\n",
	                                           pWin,pRegion,what));
    pPrivWin = (mfbPrivWin *)(pWin->devPrivates[mfbWindowPrivateIndex].ptr);
    
    switch (what) {
    case PW_BACKGROUND:
	TRACE(("apa16PaintWindow - background: State: %d\n",pWin->backgroundState));
	switch (pWin->backgroundState) {
	case None:
	    return;
	case ParentRelative:
	    do {
		pWin = pWin->parent;
	    } while (pWin->backgroundState == ParentRelative);
	    (*pWin->drawable.pScreen->PaintWindowBackground)(pWin, pRegion,
							     what);
	    return;
	case BackgroundPixmap:
	    if (pPrivWin->fastBackground)
	    {
		apa16PaintWindow32(pWin, pRegion, what);
		return;
	    }
	    break;
	case BackgroundPixel:
	    apa16PaintWindowSolid(pWin, pRegion, what);
	    return;
    	}
    	break;
    case PW_BORDER:
	if (pWin->borderIsPixel)
	{
	    apa16PaintWindowSolid(pWin, pRegion, what);
	    return;
	}
	else if (pPrivWin->fastBorder)
	{
	    apa16PaintWindow32(pWin, pRegion, what);
	    return;
	}
	break;
    }
    miPaintWindow(pWin, pRegion, what);
}

static void
apa16PaintWindowSolid(pWin, pRegion, what)
    WindowPtr pWin;
    RegionPtr pRegion;
    int what;		
{
    int nbox;		/* number of boxes to fill */
    register BoxPtr pbox;	/* pointer to list of boxes to fill */
    int	     rop;
    unsigned	cmd;
    int		w,h;

    TRACE(("apa16PaintWindowSolid( pWin= 0x%x, pRegion= 0x%x, what= %d )\n",
							pWin,pRegion,what));

    nbox = REGION_NUM_RECTS(pRegion);
    pbox = REGION_RECTS(pRegion);

    if (!nbox)	return;

    if (what == PW_BACKGROUND)
    {
        rop= pWin->background.pixel ? RROP_WHITE: RROP_BLACK;
    } 
    else
    {
        rop = pWin->border.pixel ? RROP_WHITE : RROP_BLACK;
    } 

    APA16_GET_CMD(ROP_RECT_FILL,rop,cmd);
    while (nbox--)
    {
	w = pbox->x2 - pbox->x1;
	h = pbox->y2 - pbox->y1;
	if (w==0)	w=1;
	if (h==0)	h=1;
	FILL_RECT(cmd,pbox->x2,pbox->y2,w,h);
        pbox++;
    }
}
