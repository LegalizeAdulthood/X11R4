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

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/BSDrt/RCS/bsdInitEmul.c,v 9.2 88/11/10 21:34:03 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/BSDrt/RCS/bsdInitEmul.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/BSDrt/RCS/bsdInitEmul.c,v 9.2 88/11/10 21:34:03 paul Exp $" ;
#endif

#if !defined(ibm032) || !defined(BSDrt)
	******** ERROR ********
#endif

#include <sys/types.h>
#include <sys/file.h>
#include <sys/signal.h>
#include <machinecons/screen_conf.h>
#include <machinecons/buf_emul.h>

#include "X.h"
#include "misc.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"
#include "OSxqueue.h"

#include "ibmTrace.h"

extern void ibmInfoMsg() ;

XIoAddr *BSDXaddr ;
int BSDEmulatorFD = -1 ;
int BSDdevbusFD = -1 ;

static void
ibm6152_os2_setup()
{
#ifdef SIGGRANT	/* only if signal is defined */
#ifndef SIGRETRACT
#define SIGRETRACT SIGRELEASE /* Blame Bob Relyea ! */
#endif

extern void BSDGrantHandler() ;
extern void BSDRetractHandler() ;

(void) signal( SIGRETRACT, BSDRetractHandler ) ;
(void) signal( SIGGRANT, BSDGrantHandler ) ;
#endif
return ;
}

static void
rtVS6152()
{
unsigned long int infoword ;

if ( ioctl( ibmScreenFD(0), BUFDISPINFO, &infoword ) >= 0 ) {
	if ( BUF_IS_RTPC(infoword) ) {
		ibmInfoMsg( "Machine Type Is IBM RT\n" ) ;
		if ( ( BSDdevbusFD = open( "/dev/bus", O_RDWR ) ) < 0 ) {
			ErrorF( "open of /dev/bus failed!\nExiting.\n" ) ;
			exit( 1 ) ;
		}
	}
	else {
		ibmInfoMsg( "Machine Type Is IBM 6152\n" ) ;
		ibm6152_os2_setup() ;
	}
}
else {
	ErrorF( "ioctl of emulator FD failed!\nExiting.\n" ) ;
	exit( 1 ) ;
}

return ;
}

/*
 * -- This Entry Point is called from common code !
 */
void
BSDMachineDependentInit()
{
/* Do anything which depends upon whether we're on an rt or a 6152 */
rtVS6152() ;

/* Maximize BSD resource allocations */
setResourceLimits() ; /* in bsdLimits.c */

return ;
}

void
BSDInitEmulator()
{
static int been_here = 0 ;
int emulator = E_XINPUT ;
struct x_screen_size bounds ;
int scr ;
int miny, maxy ;

    TRACE( ( "BSDInitEmulator()\n" ) ) ;

    if ( ibmNumScreens <= 0 ) {
	ErrorF(
	  "BSDInitEmulator: No open screens in BSDInitEmulator.\nExiting.\n" ) ;
	exit( 1 ) ;
    }
    if ( !been_here ) {
	been_here = 1 ;
	/* Make the first screen the input source !! */
	BSDEmulatorFD = ibmScreenFD( 0 ) ;
	if ( BSDEmulatorFD < 0 ) {
	    ErrorF( "BSDInitEmulator: ScreenFD 0 not opened!\nExiting.\n" ) ;
	    exit( 1 ) ;
	}
	if ( ioctl( BSDEmulatorFD, EISETD, &emulator ) < 0 ) {
	    ErrorF( "BSDInitEmulator: Failed to set emulator!\nExiting.\n" ) ;
	    exit( 1 ) ;
	}

#ifdef SCRSETNINP
	/* Shut Off kernel output emulators for all but the first screen !! */
	for ( scr = 1 ; scr < ibmNumScreens ; scr++ ) {
		/* This ioctl is "Screen-Set-No-Input" */
		if ( ioctl( ibmScreenFD( scr ), SCRSETNINP ) < 0 )
			ErrorF(
"BSDInitEmulator: Failed to remove screen emulator! Possible old kernel.\n" ) ;
	}
#endif

	/* Turn off kernel's cursor for the Emulator screen */
	if ( ioctl( BSDEmulatorFD, QIOCHIDECUR ) < 0 )
	    ErrorF( "BSDInitEmulator: Failed to hide cursor!\n" ) ;

	/* Ask OS for X style information */
	if ( ioctl( BSDEmulatorFD, QIOCADDR, &BSDXaddr ) < 0 ) {
	    ErrorF(
		"BSDInitEmulator: Failed to get X event queue!\nExiting.\n" ) ;
	    exit( 1 ) ;
	}

	BSDXaddr->mbox.bottom =
	BSDXaddr->mbox.top =
	BSDXaddr->mbox.left =
	BSDXaddr->mbox.right = 10000 ;

	/* Initialize OS-INdependent X-Queue */
	ibmQueue = (XEventQueue *) &BSDXaddr->ibuff ;

	miny = ibmScreenMinY( 0 ) ;
	maxy = ibmScreenMaxY( 0 ) ;
	for ( scr = 1 ; scr < ibmNumScreens ; scr++ ) {
	    if ( ibmScreenMinY( scr ) < miny )
		miny = ibmScreenMinY( scr ) ;
	    if ( ibmScreenMaxY( scr ) > maxy )
		maxy = ibmScreenMaxY( scr ) ;
	}

	/*
	 * If WrapScreen is defined, let the cursor leave the bounds of all
	 * the screens, so we can wrap it.  50 is an arbitrary choice.
	 */
	bounds.x_x_min = ibmScreenMinX( 0 ) - ( ibmXWrapScreen ? 1 : 0 ) ;
	bounds.x_x_max = ibmScreenMaxX( ibmNumScreens - 1 )
		       + ( ibmXWrapScreen ? 1 : 0 ) ;
	bounds.x_y_min = miny - ( ibmYWrapScreen ? 1 : 0 ) ;
	bounds.x_y_max = maxy + ( ibmYWrapScreen ? 1 : 0 ) ;
	if ( ioctl( BSDEmulatorFD, QIOCSETSIZE, &bounds ) == -1 ) {
	    ErrorF( "BSDInitEmulator: couldn't set bounds:\nExiting ...\n" ) ;
	    exit( 1 ) ;
	}
    }
    return ;
}
