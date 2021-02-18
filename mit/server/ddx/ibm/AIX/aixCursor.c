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
/* $Header: /andrew/X11/r3src/server/ddx/ibm/AIX/RCS/aixCursor.c,v 20.0 88/12/04 15:21:12 paul Exp $ */
/* $Source: /andrew/X11/r3src/server/ddx/ibm/AIX/RCS/aixCursor.c,v $ */

#ifndef lint
static char sccsid[] = "@(#)aixCursor.c	2.1 88/09/19 11:52:30";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "input.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "miscstruct.h"

#include "OSio.h"

#include "ibmScreen.h"
#include "ibmIO.h"

#include "aixCursor.h"

#include "ibmTrace.h"


/***============================================================***/

void
AIXConstrainCursor( pScr, pBox )
register ScreenPtr      pScr ;
register BoxPtr         pBox ;
{

    TRACE( ( "AIXConstrainCursor(pScr=0x%x,pBox=0x%x)\n" ) ) ;
    /* XXX -- not implemented yet */
    return ;
}


/***============================================================***/

int
AIXSetCursorPosition( pScr, x, y, generateEvent )
    register ScreenPtr 	pScr;
    register int	x,y;
    Bool	generateEvent;
{
xEvent		motion;
DevicePtr	mouse;

    TRACE(("AIXSetCursorPosition( pScr= 0x%x, x= %d, y= %d )\n",pScr,x,y));
    if (pScr->myNum!=ibmCurrentScreen) {
	(*ibmHideCursor(ibmCurrentScreen))(ibmScreen(ibmCurrentScreen));
	ibmCurrentScreen= pScr->myNum;
    }

    if (generateEvent) {
	ProcessInputEvents();
	motion.u.keyButtonPointer.rootX=	x;
	motion.u.keyButtonPointer.rootY=	y;
	motion.u.keyButtonPointer.time=	lastEventTime;
	motion.u.u.type=			MotionNotify;
	mouse= LookupPointerDevice();
	(*mouse->processInputProc)(&motion,mouse);
    }
    AIXCurrentX= x;
    AIXCurrentY= y;
    (*(ibmCursorShow(pScr->myNum)))(x,y);
    return TRUE;
}

/***============================================================***/

void
AIXCursorLimits( pScr, pCurs, pHotBox, pTopLeftBox )
ScreenPtr	pScr;
CursorPtr	pCurs;
BoxPtr		pHotBox;
BoxPtr		pTopLeftBox;
{
    TRACE(("AIXCursorLimits( pScr= 0x%x, pCurs= 0x%x, pHotBox= 0x%x, pTopLeftBox= 0x%x)\n", pScr, pCurs, pHotBox, pTopLeftBox));

    pTopLeftBox->x1= max( pHotBox->x1, 0 );
    pTopLeftBox->y1= max( pHotBox->y1, 0 );
    pTopLeftBox->x2= min( pHotBox->x2, pScr->width );
    pTopLeftBox->y2= min( pHotBox->y2, pScr->height );
    return ;
}
