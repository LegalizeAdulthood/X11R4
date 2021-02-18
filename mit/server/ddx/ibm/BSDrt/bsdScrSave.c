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

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/BSDrt/RCS/bsdScrSave.c,v 1.2 89/02/08 14:52:35 paul Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/BSDrt/RCS/bsdScrSave.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/BSDrt/RCS/bsdScrSave.c,v 1.2 89/02/08 14:52:35 paul Exp $";
#endif

#if !defined(ibm032) || !defined(BSDrt)
	******** ERROR ********
#endif

#include <sys/signal.h>
#include <machinecons/screen_conf.h>
#include <machinecons/buf_emul.h>
#include <machinecons/xio.h>

#include "X.h"
#include "misc.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"

#include "ibmTrace.h"

#ifdef SIGGRANT	/* only if signal is defined */

#ifndef SIGRETRACT
#define SIGRETRACT SIGRELEASE /* Blame Bob Relyea ! */
#endif

void
BSDScreenStateChange( pE )
register XEvent *pE ;
{
register ibmPerScreenInfo *screenInfo = ibmScreens[ pE->xe_key ] ;

/* NOTE: This function uses the fact that sigblock() returns the current
 *	signal mask AND ignores attempts to block SIGKILL !!
 */

/* Up for lose screen -- Down for regain */
if ( pE->xe_direction == XE_KBTUP ) {
	(* screenInfo->ibm_SaveFunc)( pE->xe_key ) ;
	if ( ioctl( screenInfo->ibm_ScreenFD, BUFSAVEDONE ) < 0 )
		ErrorF( "BSDScreenStateChange: ioctl Failed\n" ) ;
	/* Should Unblock SIGRETRACT here !! */
	(void) sigsetmask( sigblock( SIGKILL ) & ~ sigmask( SIGRETRACT ) ) ;
}
else if ( pE->xe_direction == XE_KBTDOWN ) {
	(* screenInfo->ibm_RestoreFunc)( pE->xe_key ) ;
	if ( ioctl( screenInfo->ibm_ScreenFD, BUFRESTOREDONE ) < 0 )
		ErrorF( "BSDScreenStateChange: ioctl Failed\n" ) ;
	/* Should Unblock SIGGRANT here !! */
	(void) sigsetmask( sigblock( SIGKILL ) & ~ sigmask( SIGGRANT ) ) ;
}
else
	ErrorF( "BSDScreenStateChange: Bad Screen Event\n" ) ;

return ;
}

static int
findSignaledScreen( desiredState )
register int desiredState ;
{
register int i ;
unsigned long int screenStatus ;

for ( i = ibmNumScreens ; i-- ; )
	if ( ioctl( ibmScreenFD(i), BUFGETSAVE, &screenStatus ) < 0 )
		ErrorF( "FindSignaledScreen: ioctl BUFGETSAVE FAILED\n" ) ;
	else if ( screenStatus == desiredState )
		return i ;

return -1 ;
}

/* BSDRetractHandler( sig ) -- Should Do :
 -	ioctl() to determine which screen
 -	then ioctl() to push a "Screen" event onto the shared memory queue
 -	All "real" stuff is done when the event is taken off the queue !
 -	NOTE: After receiving the signal, further signals MUST be "held"
 -		until the screen has actually been saved !
 */
void
BSDRetractHandler( sig, code, cp )
register int sig ;
int code ;
register struct sigcontext *cp ;
{
register int whichScreen ;
XEvent dummyEvent ;

if ( ( whichScreen = findSignaledScreen( BUF_NEED_SAVE ) ) < 0 )
	ErrorF( "BSDRetractHandler : no screen needs to be saved!\n" ) ;
else {
	dummyEvent.xe_key = whichScreen ;
	dummyEvent.xe_x = 0 ;
	dummyEvent.xe_y = 0 ;
	dummyEvent.xe_time = 0 ;
	dummyEvent.xe_type = 0 ;
	/* Up for lose screen -- Down for regain */
	dummyEvent.xe_direction = XE_KBTUP ;
	dummyEvent.xe_device = XE_CONSOLE ; /* console */
	if ( ioctl( ibmScreenFD(0), QIOCMUSTADD, &dummyEvent ) < 0 )
		ErrorF( "BSDRetractHandler : ioctl QIOCMUSTADD FAILED\n" ) ;
	cp->sc_mask |= sigmask( SIGRETRACT ) ;
}

#if defined(DEBUG)
(void) printf( "BSDRetractHandler(%d) Called!!\n", sig ) ;
(void) printf( "BSDRetractHandler: final Screen #%d\n", whichScreen ) ;
#endif

return ;
}

/* BSDGrantHandler()
 - Should Do :
 -	ioctl() to determine which screen
 -	then ioctl() to push a "Screen" event onto the shared memory queue
 -	All "real" stuff is done when the event is taken off the queue !
 */
void
BSDGrantHandler( sig, code, cp )
register int sig ;
int code ;
register struct sigcontext *cp ;
{
register int whichScreen ;
XEvent dummyEvent ;

if ( ( whichScreen = findSignaledScreen( BUF_NEED_RESTORE ) ) < 0 )
	ErrorF( "BSDGrantHandler : no screen needs to be restored!\n" ) ;
else {
	dummyEvent.xe_key = whichScreen ;
	dummyEvent.xe_x = 0 ;
	dummyEvent.xe_y = 0 ;
	dummyEvent.xe_time = 0 ;
	dummyEvent.xe_type = 0 ;
	/* Up for lose screen -- Down for regain */
	dummyEvent.xe_direction = XE_KBTDOWN ;
	dummyEvent.xe_device = XE_CONSOLE ; /* console */
	if ( ioctl( ibmScreenFD(0), QIOCMUSTADD, &dummyEvent ) < 0 )
		ErrorF( "BSDGrantHandler : ioctl QIOCMUSTADD FAILED\n" ) ;
	cp->sc_mask |= sigmask( SIGGRANT ) ;
}
#if defined(DEBUG)
(void) printf( "BSDGrantHandler( %d ) Called!!\n", sig ) ;
(void) printf( "BSDGrantHandler: final Screen #%d\n", whichScreen ) ;
#endif

return ;
}
#endif /* def SIGGRANT */
