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

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/common/RCS/ibmKeybd.c,v 9.2 88/11/10 21:56:17 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/common/RCS/ibmKeybd.c,v $ */
#ifndef lint
static char *rcsid  = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/common/RCS/ibmKeybd.c,v 9.2 88/11/10 21:56:17 paul Exp $" ;
#endif

#include "X.h"
#include "input.h"

#include "ibmTrace.h"

DevicePtr ibmKeybd ;
int ibmBellPitch ;
int ibmBellDuration ;
int ibmLEDState ;
int ibmKeyClick ;
int ibmKeyRepeat ;

int ibmUsePCKeys =	0 ;
int ibmLockState =	0 ;
int ibmCurLockKey =	0 ;
int ibmLockEnabled =	TRUE ;

void
ibmChangeKeyboardControl( pDevice, pCtrl )
	DevicePtr	*pDevice ;
	KeybdCtrl	*pCtrl ;
{
    TRACE( ( "ibmChangeKeyboardControl(0x%x,0x%x)\n", pDevice, pCtrl ) ) ;
    ibmKeyClick =	pCtrl->click ;
    ibmKeyRepeat =	pCtrl->autoRepeat ;
    ibmBellPitch =	pCtrl->bell_pitch ;
    ibmBellDuration =	pCtrl->bell_duration ;
    ibmLEDState =	pCtrl->leds ;

    return ;
}
