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
/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/common/RCS/ibmCmdArgs.h,v 30.0 89/01/23 20:46:56 paul Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/common/RCS/ibmCmdArgs.h,v $ */

#if !defined(lint) && !defined(LOCORE)  && defined(RCS_HDRS)
static char *rcsidibmCmdArgs = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/common/RCS/ibmCmdArgs.h,v 30.0 89/01/23 20:46:56 paul Exp $" ;
#endif

extern char *ibmWhitePixelText ;
extern char *ibmBlackPixelText ;
#if defined(IBM_SPECIAL_MALLOC)
extern char *ibmArenaFile ;
#endif
extern int ibmLockEnabled ;
extern int ibmUsePCKeys ;
extern int ibmUsePCKeys ;
extern int ibmOpenAllScreens ;
extern int ibmNumScreens ;
extern int ibmScreensWanted ;
extern int ibmQuietFlag ;
extern int ibmXWrapScreen ;
extern int ibmYWrapScreen ;
extern int ibmAllowBackingStore ;
#if defined(IBM_OS_HAS_HFT)
extern int ibmDontZap ;
#endif
#if defined(ibm032)
extern int ibmUseHardware ;
#endif
