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
/* $Header: /andrew/X11/r3src/server/ddx/ibm/AIX/RCS/aixKeybd.c,v 20.0 88/12/04 15:21:51 paul Exp $ */
/* $Source: /andrew/X11/r3src/server/ddx/ibm/AIX/RCS/aixKeybd.c,v $ */

#ifndef lint
/* @(#)aixKeybd.c	3.6 88/10/03 10:01:20 */
static char *rcsid = "$Header: /andrew/X11/r3src/server/ddx/ibm/AIX/RCS/aixKeybd.c,v 20.0 88/12/04 15:21:51 paul Exp $";
#endif

#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/hft.h>

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "keysym.h"
#include "cursorstr.h"

#include "aixModKeys.h"
#include "aixKeymap.h"

#include "ibmIO.h"
#include "ibmKeybd.h"
#include "ibmScreen.h"

#include "hftUtils.h"

#include "ibmTrace.h"

/***============================================================***/

static void
AIXChangeKeyboardControl(pDevice,ctrl)
    DevicePtr pDevice;
    KeybdCtrl *ctrl;
{
    int volume;

    TRACE(("AIXChangeKeyboardControl(pDev=0x%x,ctrl=0x%x)\n",pDevice,ctrl));

    ibmKeyClick =   ctrl->click;
    hftSetKeyClick(ctrl->click);

    /* X specifies duration in milliseconds, RT in 1/128th's of a second */
    ibmBellPitch=	ctrl->bell_pitch;
    ibmBellDuration= 	((double)ctrl->bell_duration)*(128.0/1000.0);

    hftSetLEDS(0x7,(ibmLEDState=ctrl->leds));

    if (ibmKeyRepeat= ctrl->autoRepeat)
	hftSetTypematicDelay( 400 );
    else
	hftSetTypematicDelay( 600 );

    return;
}

/***============================================================***/

static void
AIXBell(loud, pDevice)
    int loud;
    DevicePtr pDevice;
{

    TRACE(("AIXBell(loud= %d, pDev= 0x%x)\n",loud,pDevice));
    hftSound(loud,ibmBellDuration,ibmBellPitch);
    return;
}

/***============================================================***/

Bool 
LegalModifier (key)
BYTE    key;
{

    TRACE(("LegalModifier(key= 0x%x)\n",key));
    if ( (key==Aix_Control_L) || (key==Aix_Shift_L) || (key==Aix_Shift_R) ||
	(key==Aix_Caps_Lock) || (key==Aix_Alt_L) || (key==Aix_Alt_R) )
      return TRUE ;
#ifdef notdef
#if defined(i386) 
    else if (key==Aix_Control_R)
      return TRUE;
#endif
#endif
    else
      return FALSE;
}

/***============================================================***/

extern int ibmUsePCKeys;
int kbdType ;

AIXGetKbdMappings(pKeySyms, pModMap)
KeySymsPtr pKeySyms;
CARD8 *pModMap;
{
    register int i;
    TRACE(("AIXGetKbdMappings( pKeySyms= 0x%x, pModMap= 0x%x )\n",
							pKeySyms,pModMap));
    for (i = 0; i < MAP_LENGTH; i++)
       pModMap[i] = NoSymbol;

    if (ibmUsePCKeys) {
	pModMap[ Aix_Control_L ] = LockMask;
	pModMap[ Aix_Caps_Lock ] = ControlMask;
    }
    else {
	pModMap[ Aix_Caps_Lock ] = LockMask;
	pModMap[ Aix_Control_L ] = ControlMask;
    }

	/*
	 * 12/27/88 (ef) -- XXX!  Someday we should set this based on
	 *	a keyboard id, not on the operating system/processor.
	 *	(assuming the hft provides different keyboard id's for
	 *	the RT and 386 keyboards)
	 */
#ifdef notdef
#if defined(i386) 
    pModMap[ Aix_Control_R ] =	ControlMask;
#endif
#endif
    pModMap[ Aix_Shift_L ] =	ShiftMask;
    pModMap[ Aix_Shift_R ] =	ShiftMask;
    pModMap[ Aix_Num_Lock ] =	NumLockMask;

    /*
     * Theses define's should be removed once they are in 
     * /usr/include/sys/hft.h.
     */
#define HF101KBD 0
#define HF102KBD 1

    switch ( kbdType = hftGetKeyboardID() )
    {
    case HF101KBD :
       pModMap[ Aix_Alt_L ] = Mod1Mask;
       pModMap[ Aix_Alt_R ] = Mod1Mask;
	break;
    case HF102KBD :
       pModMap[ Aix_Alt_L ] = Mod1Mask;
       pModMap[ Aix_Alt_R ] = Mod2Mask;
	break;
    case HFT_ILLEGAL_KEYBOARD:
    default :
       pModMap[ Aix_Alt_L ] = Mod1Mask;
       pModMap[ Aix_Alt_R ] = Mod1Mask;
	break;
    }

    pKeySyms->minKeyCode=       AIX_MIN_KEY;
    pKeySyms->maxKeyCode=       AIX_MAX_KEY;
    pKeySyms->mapWidth=         AIX_GLYPHS_PER_KEY;
    pKeySyms->map=              aixmap;
}

/***============================================================***/

int
AIXKeybdProc(pDev, onoff, argc, argv)
    DevicePtr 	 pDev;
    int 	 onoff;
    int		 argc;
    char	*argv[];
{
    KeySymsRec		keySyms;
    CARD8 		modMap[MAP_LENGTH];

    TRACE(("AIXKeybdProc( pDev= 0x%x, onoff= 0x%x )\n",pDev,onoff));

    switch (onoff)
    {
	case DEVICE_INIT: 
	    ibmKeybd = pDev;
	    AIXGetKbdMappings( &keySyms, modMap );
	    InitKeyboardDeviceStruct(
			ibmKeybd, &keySyms, modMap, AIXBell,
			AIXChangeKeyboardControl);
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
