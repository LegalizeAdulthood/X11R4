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
/* $Header: /andrew/X11/r3src/server/ddx/ibm/AIX/RCS/aixMouse.c,v 20.0 88/12/04 15:22:12 paul Exp $ */
/* $Source: /andrew/X11/r3src/server/ddx/ibm/AIX/RCS/aixMouse.c,v $ */

#ifndef lint
static char sccsid[] = "@(#)aixMouse.c	3.2 88/09/26 21:40:02";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/hft.h>

#include "X.h"
#include "Xproto.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "input.h"
#include "cursorstr.h"

#include "ibmIO.h"
#include "ibmMouse.h"
#include "ibmScreen.h"

#include "ibmTrace.h"

#include "hftQueue.h"

extern	char	*getenv();

int	AIXMouseChordDelay= 20;

/***================================================================***/

static int
AIXGetMotionEvents(buff, start, stop)
    CARD32 start, stop;
    xTimecoord *buff;
{
    TRACE(("rtGetMotionEvents( buff= 0x%x, start= %d, stop= %d )\n",
							buff,start,stop));
    return 0;
}

/***================================================================***/

static unsigned	always0= 0;

int
AIXMouseProc(pDev, onoff)
    DevicePtr	pDev;
    int onoff;
{
    BYTE map[4];

    TRACE(("AIXMouseProc( pDev= 0x%x, onoff= 0x%x )\n",pDev, onoff ));

    switch (onoff)
    {
    case DEVICE_INIT: 
	    ibmPtr = pDev;
	    map[1] = 1;
	    map[2] = 2;
	    map[3] = 3;
#ifdef AIX386
	    /*
	     * On the AIX PS/2, the default HFT horizontal (11) and 
	     * vertical (22) mouse threshold is too jerky.
	     */  
	    hftSetLocatorThresholds(2,2);
#endif /* AIX386 */
	    InitPointerDeviceStruct(
		ibmPtr, map, 3, AIXGetMotionEvents, ibmChangePointerControl );
	    SetInputCheck( &hftPending, &always0 );
	    break;
    case DEVICE_ON:
	    pDev->on = TRUE;
	    break;
    case DEVICE_OFF:
	    pDev->on = FALSE;
	    break;
    case DEVICE_CLOSE:
	    break;
    }
    return Success;
}
