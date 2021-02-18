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
#ifndef OS_IOH
#define OS_IOH 1

/***********************************************************
		Copyright IBM Corporation 1988

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
/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/BSDrt/RCS/OSio.h,v 9.9 89/04/29 20:43:03 jeff Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/BSDrt/RCS/OSio.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidOSio = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/BSDrt/RCS/OSio.h,v 9.9 89/04/29 20:43:03 jeff Exp $";
#endif

#if !defined(IBM_OS_HAS_X_QUEUE)
#define	IBM_OS_HAS_X_QUEUE 1
#endif

extern void NoopDDA() ;

#define	OS_BlockHandler				NoopDDA
#define OS_WakeupHandler			NoopDDA

#define	OS_MouseProc				BSDMouseProc
#define	OS_KeybdProc				BSDKeybdProc

#define	OS_CapsLockFeedback( dir )		BSDCapsLockFeedback( dir )

#define	OS_PreScreenInit			BSDMachineDependentInit
#define	OS_PostScreenInit			BSDInitEmulator
#ifdef SIGGRANT
#define	OS_ScreenStateChange( evt )		BSDScreenStateChange( evt )
#else /* SIGGRANT */
#define	OS_ScreenStateChange(evt)
#endif /* SIGGRANT */

#define	OS_GetDefaultScreens			NoopDDA
#define	OS_InitInput				NoopDDA
#define	OS_AddAndRegisterOtherDevices		NoopDDA
#define	OS_SaveState				NoopDDA
#define	OS_RestoreState				NoopDDA

#define OS_GiveUp				NoopDDA
#define OS_Abort				NoopDDA

#endif /* ndef OS_IOH */
