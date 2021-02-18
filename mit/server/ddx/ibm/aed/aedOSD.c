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

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/aed/RCS/aedOSD.c,v 1.1 89/05/01 14:36:59 jeff Exp Locker: jeff $";
#endif

#include <fcntl.h>
#include <sys/ioctl.h>

#if defined(BSDrt)
#include <machinecons/screen_conf.h>
#endif

#include "X.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "OScompiler.h"

#include "ibmScreen.h"

#include "aedHdwr.h"

#include "ibmTrace.h"

/***==================================================================***/

extern void ibmInfoMsg() ;

int
aedProbe()
{
register int	fd;
#ifdef BSDrt
struct	screen_control	scr;
#endif

    TRACE(("aedProbe()\n"));

    if ( ( fd = open( AED_SCREEN_DEVICE, O_RDWR | O_NDELAY | O_EXCL ) ) < 0 ) {
	return -1 ;
    }

#ifdef BSDrt
    scr.device= CONS_AED;
    if ( ioctl( fd, SCRIOCGETF, &scr ) < 0 ) {
	ErrorF( "WSGO! ioctl SCRIOCGETF failed in aedProbe()\n" ) ;
	(void) close( fd ) ;
	return -1 ;
    }
    if ( !( scr.switches & CONSDEV_PRESENT ) ) {
	(void) close( fd ) ;
	return -1 ;
    }
#else
	******** ERROR ******** AIX/rt ??
#endif

    ibmInfoMsg( "Found an AED\n" ) ;
    return fd ;
}

/***==================================================================***/

int
aedScreenInitHW( index )
    register int index;
{
    register int aedFD ;
    static int been_here = 0 ;

    TRACE(("aedScreenInitHW( index= %d )\n", index )) ;

    if ( ( aedFD = ibmScreenFD(index) ) < 0 ) {
	ErrorF( "aed file descriptor is invalid\n");
	return 0;
    }

#if defined(BSDrt)

    if ( !been_here ) {
	int out_emulator = E_AED;
	int u_codeFD ;
	int len ;
	volatile unsigned short int tmpi ;

	if ( ( u_codeFD = open( MICROCODE, O_RDONLY ) ) < 0 ) {
		ErrorF( "aedScreenInit: unable to open micro-code file: %s\n",
			MICROCODE ) ;
		return 0 ;
	}

#ifdef ATRIO
	{
	    int page = VIKADDR;
	    /* Set 128k window to point to aed display buffer */
	    if ( ioctl( aedFD, BUFSETWIND, &page ) < 0 ) {
			ErrorF( "aedScreenInit: aedioctl, set window" ) ;
			(void) close(fd);
			return 0;
	    }
	}
#endif /* RTAT */

	if ( ioctl( aedFD, EOSETD, &out_emulator ) < 0 ) {
	    ErrorF("aedScreenInit: error setting output emulator");
	    return 0;
	}

	/* ok, get going already ---- reset viking */
	tmpi = VIKLOAD( *( (volatile unsigned short int *) VIKROOT ) ) ;
	do {
		/* read from micro-code file */
		len = read( u_codeFD, vikint, 4096 ) ;
		/* load microcode into the adapter */
		write( aedFD, vikint, 4096 ) ;
	   } while ( len == 4096 ) ;
	(void) close( u_codeFD ) ;
	/* reset viking again */
	tmpi = VIKLOAD( *( (volatile unsigned short int *) VIKROOT ) ) ;

	/* reset command */
	VIKSTORE( *((volatile unsigned short int *) VIKROOT + 0x4002) , 0);
	/* set to go */
	VIKSTORE(*semaphore , 0xffff);
	tmpi = VIKLOAD(*semaphore);		/* start running viking */
	vikwait();
	out_emulator = E_XOUTPUT;
	if ( ioctl( aedFD, EOSETD, &out_emulator ) < 0 )
	{
	    ErrorF( "aedScreenInit: error setting output emulator" ) ;
	    return 0;
	}
    }
#else
	******** ERROR ******** AIX/rt ??
#endif

	return been_here = 1;
}
