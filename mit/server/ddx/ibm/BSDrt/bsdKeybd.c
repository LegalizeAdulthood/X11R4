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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/BSDrt/RCS/bsdKeybd.c,v 1.2 89/08/28 10:09:19 hkbirke Exp Locker: hkbirke $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/BSDrt/RCS/bsdKeybd.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/BSDrt/RCS/bsdKeybd.c,v 1.2 89/08/28 10:09:19 hkbirke Exp Locker: hkbirke $" ;
#endif

#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#include <machineio/speakerio.h>

#include "X.h"
#include "Xmd.h"
#define NEED_EVENTS
#define NEED_REPLIES
#include "Xproto.h"
#include "inputstr.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "keysym.h"
#include "cursorstr.h"

#include "OSxqueue.h"

#include "bsdKeymap.h"

#include "ibmKeybd.h"
#include "ibmScreen.h"

#include "ibmTrace.h"

DevicePtr ibmKeybd ;

static	struct spk_blk	BSDBellSetting= { 0,0,0,0 } ;  /* Be vewwwy quiet... */

/***============================================================***/

static void
BSDChangeLEDs(leds)
register unsigned	leds ;
{
register int	fd= BSDEmulatorFD ;

    TRACE(("BSDChangeLEDs(0x%x)\n",leds)) ;
    (void) ioctl(fd,((leds&IBM_LED_NUMLOCK)?QIOCSETNUML:QIOCCLRNUML)) ;
    (void) ioctl(fd,((leds&IBM_LED_CAPSLOCK)?QIOCSETCAPSL:QIOCCLRCAPSL)) ;
    (void) ioctl(fd,((leds&IBM_LED_SCROLLOCK)?QIOCSETSCROLLL:QIOCCLRSCROLLL)) ;
    return ;
}

/***============================================================***/

void
BSDCapsLockFeedback(down)
int	down ;
{
    TRACE(("BSDCapsLockFeedback(%d)\n",down)) ;
    (void) ioctl(BSDEmulatorFD,(down?QIOCSETCAPSL:QIOCCLRCAPSL)) ;
    return ;
}

/***============================================================***/

	/*
	 * Magic code lifted from speaker(4) man page.
	 * I *couldn't* have made this up.
	 */

static void
BSDSetBellPitch(freq)
int	freq ;
{

    TRACE(("BSDSetBellPitch(%d)\n",freq)) ;
    if (freq < 23) {
	BSDBellSetting.freqhigh=0 ;
	BSDBellSetting.freqlow=SPKOLOMIN ;
    } else if (freq < 46) {
	BSDBellSetting.freqhigh=64 ;
	BSDBellSetting.freqlow = (char) ((6000.0 /(float) freq) - 9.31) ;
    } else if (freq < 91) {
	BSDBellSetting.freqhigh=32 ;
	BSDBellSetting.freqlow = (char) ((12000.0 /(float) freq) - 9.37) ;
    } else if (freq < 182) {
	BSDBellSetting.freqhigh=16 ;
	BSDBellSetting.freqlow = (char) ((24000.0 /(float) freq) - 9.48) ;
    } else if (freq < 363) {
	BSDBellSetting.freqhigh=8 ;
	BSDBellSetting.freqlow = (char) ((48000.0 /(float) freq) - 9.71) ;
    } else if (freq < 725) {
	BSDBellSetting.freqhigh=4 ;
	BSDBellSetting.freqlow = (char) ((96000.0 /(float) freq) - 10.18) ;
    } else if (freq < 1433) {
	BSDBellSetting.freqhigh=2 ;
	BSDBellSetting.freqlow = (char) ((192000.0 /(float) freq) - 11.10) ;
    } else if (freq < 12020) {
	BSDBellSetting.freqhigh=1 ;
	BSDBellSetting.freqlow = (char) ((384000.0 /(float) freq) - 12.95) ;
    } else {
	BSDBellSetting.freqhigh=0 ;
	BSDBellSetting.freqlow=SPKOLOMIN ;
    }
    return ;
}

/***============================================================***/

static void
BSDChangeKeyboardControl(pIntDevice,ctrl)
    DeviceIntPtr pIntDevice ;
    KeybdCtrl *ctrl ;
{
    int volume ;
    DevicePtr pDevice = &(pIntDevice->public);

    TRACE(("BSDChangeKeyboardControl(pDev=0x%x,ctrl=0x%x)\n",pDevice,ctrl)) ;

/*
    volume = (ctrl->click==0?-1:((ctrl->click / 14) & 7)) ;
*/
    if (!ctrl->click)	volume= 0 ;
    else		volume= (((double)ctrl->click)*(6.0/100.0))+1 ;

    (void) ioctl(BSDEmulatorFD, QIOCCLICK, &volume) ;

    BSDSetBellPitch(ctrl->bell_pitch) ;
    /* X specifies duration in milliseconds, RT in 1/128th's of a second */
    BSDBellSetting.duration= ((double)ctrl->bell_duration)*(128.0/1000.0) ;

    BSDChangeLEDs(ctrl->leds) ;

    (void) ioctl(BSDEmulatorFD, QIOCAUTOREP, &ctrl->autoRepeat) ;
    return ;
}

/***============================================================***/

static void
BSDBell(loud, pIntDevice)
    int loud ;
    DeviceIntPtr pIntDevice ;
{
int BSDSpeakerFD ;
DevicePtr pDevice = &(pIntDevice->public);

    TRACE( ( "BSDBell(loud=%d,pDev=0x%x)\n", loud, pDevice ) ) ;

    BSDSpeakerFD = open( "/dev/speaker", O_WRONLY ) ;
    if ( BSDSpeakerFD < 0 )
	return ;
    /* RT speaker volume is between 0 (off) and 3 (loud) */
    if ( loud != 0 ) {
	BSDBellSetting.volume = ( loud /34 ) + 1 ;
	(void) write(BSDSpeakerFD,&BSDBellSetting,sizeof(BSDBellSetting)) ;
    }
    (void) close( BSDSpeakerFD ) ;
    return ;
}

/***============================================================***/

Bool
LegalModifier( key )
BYTE key ;
{
    TRACE( ( "LegalModifier(key=0x%x)\n", key ) ) ;

    if ((key==BSD_CONTROL)||(key==BSD_LEFT_SHIFT)||(key==BSD_RIGHT_SHIFT)||
	(key==BSD_LOCK)||(key==BSD_ALT_L)||(key==BSD_ALT_R)||(key==BSD_ACTION)||
	(key==BSD_NUM_LOCK)) {
	return TRUE ;
    }
    return FALSE ;
}

/***============================================================***/

extern int ibmUsePCKeys ;

void
BSDGetKbdMappings( pKeySyms, pModMap )
KeySymsPtr pKeySyms ;
CARD8 *pModMap ;
{
    register int i ;

    TRACE( ( "BSDGetKbdMappings(pKeySyms=0x%x,pModMap=0x%x)\n",
	    pKeySyms, pModMap ) ) ;

    for (i = MAP_LENGTH ; i-- ; )
	pModMap[i] = NoSymbol ;	/* make sure it is restored */

    if ( ibmUsePCKeys ) {
	pModMap[ BSD_CONTROL ] = LockMask ;
	BSDmap[(BSD_CONTROL-BSD_MIN_KEY)*BSD_GLYPHS_PER_KEY]=  XK_Caps_Lock ;
	pModMap[ BSD_LOCK ] = ControlMask ;
	BSDmap[(BSD_LOCK-BSD_MIN_KEY)*BSD_GLYPHS_PER_KEY]= XK_Control_L ;
    }
    else {
	pModMap[ BSD_LOCK ] = LockMask ;
	BSDmap[ (BSD_LOCK-BSD_MIN_KEY)*BSD_GLYPHS_PER_KEY ] = XK_Caps_Lock ;
	pModMap[ BSD_CONTROL ] = ControlMask ;
	BSDmap[ (BSD_CONTROL-BSD_MIN_KEY)*BSD_GLYPHS_PER_KEY ] = XK_Control_L ;
    }
    pModMap[ BSD_LEFT_SHIFT ] = ShiftMask ;
    pModMap[ BSD_RIGHT_SHIFT ] = ShiftMask ;
    pModMap[ BSD_ALT_L ] = Mod1Mask ;
    pModMap[ BSD_ALT_R ] = Mod1Mask ;
    pModMap[ BSD_NUM_LOCK ] = LockMask ;

    pKeySyms->minKeyCode =	BSD_MIN_KEY ;
    pKeySyms->maxKeyCode =	BSD_MAX_KEY ;
    pKeySyms->mapWidth =	BSD_GLYPHS_PER_KEY ;
    pKeySyms->map =		BSDmap ;
    return ;
}

/***============================================================***/

int
BSDKeybdProc( pIntDev, onoff, argc, argv )
    DeviceIntPtr 	 pIntDev ;
    int 	 onoff ;
    int		 argc ;
    char	*argv[] ;
{
    KeySymsRec	keySyms ;
    CARD8 	modMap[MAP_LENGTH] ;
    DevicePtr	pDev = &(pIntDev->public);

    TRACE( ( "BSDKeybdProc(pDev=0x%x,onoff=0x%x)\n", pDev, onoff ) ) ;

    switch (onoff)
    {
	case DEVICE_INIT: 
	    ibmKeybd = pDev ;
	    ibmLockState= 0 ;
	    pDev->devicePrivate = (pointer) &ibmQueue ;
	    BSDGetKbdMappings( &keySyms, modMap ) ;
	    InitKeyboardDeviceStruct(
			ibmKeybd, &keySyms, modMap, BSDBell,
			BSDChangeKeyboardControl) ;
	    break ;
	case DEVICE_ON: 
	    pDev->on = TRUE ;
	    TRACE( ("Calling AddEnabledDevice") );
	    AddEnabledDevice(BSDEmulatorFD) ;
	    TRACE( ("Returned from AddEnabled") );
	    break ;
	case DEVICE_OFF: 
	    pDev->on = FALSE ;
	    break ;
	case DEVICE_CLOSE:
	    break ;
    }
    return Success ;
}
