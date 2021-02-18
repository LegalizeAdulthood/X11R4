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
#ifndef lint
static char sccsid[] = "@(#)AIXinit.c	3.1 88/09/22 09:29:26";
#endif

#if defined(AIXrt)
#include <sys/types.h>
#include <sys/file.h>
#endif

#include <sys/hft.h>

#include "Xmd.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursor.h"

#include "ibmScreen.h"

#include "./OSio.h"

#include "hftQueue.h"
#include "hftUtils.h"

#include "ibmTrace.h"

/***====================================================================***/

int	AIXDefaultDisplay= 0;

void
AIXMachineDependentInit()
{
    TRACE(("AIXMachineDependentInit()\n"));

#if defined(AIXrt)
    hftInitQueue(AIXDefaultDisplay,FALSE);
#endif
    RemoveEnabledDevice(hftQFD);
#ifdef AIXrt
    if (open("/dev/bus",O_RDWR)==-1) {
	ErrorF("Couldn't get /dev/bus.\nExiting\n");
	exit(1);
    }
#endif
    return;
}

/***====================================================================***/

#if !defined(DISABLE_ADDITIONAL_SCREENS)
int	(*ibmAdditionalDfltScreen)();
#endif

void
AIXGetDefaultScreens()
{
unsigned	Device, hftQueryCurrentDevice();
char		*devflag, **fakeArgv= &devflag;
char 		*devstring = "unknown device";

    TRACE(("AIXGetDefaultScreens()\n"));
    if (hftFindAScreen(0)<0) {
	ErrorF("X must be run from the console\n");
	exit(1);
    }

    Device = hftQueryCurrentDevice();
    switch (Device & HFT_DEVID_MASK) {
#ifdef AIXrt
	case HFT_MEGAPEL_ID:
		devflag= "-mpel";
		devstring = "megapel";
		break;
	case HFT_APA16_ID:
		devflag= "-apa16";
		devstring = "6155";
		break;
	case HFT_EGA_ID:
		devflag= "-ega";
		devstring = "ega";
		break;
	case HFT_APA8_ID:
		devstring = "6153";
		devflag= "-gsl";
		break;
	case HFT_APA8C_ID:
		devstring = "6154";
		devflag= "-gsl";
		break;
#else
#ifdef AIX386
        case HFT_VGA_8503_ID:
	case HFT_VGA_8512_ID:
	case HFT_VGA_8513_ID:
	case HFT_VGA_8514_ID:
	case HFT_VGA_8507_ID:
	case HFT_VGA_8604_ID:
                devflag= "-vga" ;
                devstring = "vga" ;
                break ;
        case HFT_8514A_8503_ID:
        case HFT_8514A_8512_ID:
        case HFT_8514A_8513_ID:
        case HFT_8514A_8514_ID:
        case HFT_8514A_8507_ID:
        case HFT_8514A_8604_ID:
                devflag= "-8514" ;
                devstring = "8514" ;
                break ;
#else
ERROR!!! MUST BE RT/PC or PS/2 386
#endif /* AIX386 */
#endif /* AIXrt */
	default:
#ifndef DISABLE_ADDITIONAL_SCREENS
		if (ibmAdditionalDfltScreen) {
		    if ((*ibmAdditionalDfltScreen)(Device,&devstring,&devflag))
			break;
		}
#endif
		devstring= "unknown device";
		devflag= "-gsl";
		break;
	case HFT_ILLEGAL_ID:
		ErrorF("Cannot determine current device for X screen.\n");
		ErrorF("Possible problems: remote execution, no active\n");
		ErrorF("tty, or running in background on old kernel.\n");
		ErrorF("Retry by specifying a screen option on the command\n");
		ErrorF("line (Eg '-mpel') or by running in foreground\n");
		ErrorF("Check out the documentation if all else fails\n\n");

		Error("Goodbye!\n"); 	/* should *NEVER* return */
		_exit(-1);	/* NOTREACHED */
		break;		/* NOTREACHED */
    }

    if (Device & HFT_DEVBUSTED_MASK) {
	ErrorF("Warning: I seem to be running on a broken (or");
	ErrorF(" only semi-functional) %s.\n",devstring);
    }

    ddxProcessArgument(1,fakeArgv,0);

    return;
}

/***==================================================================***/

#if !defined(FORCE_DISPLAY_NUMBER)

#if !defined(MAX_SERVER_NUM)
#define MAX_SERVER_NUM 255
#endif

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/in.h>
#include <errno.h>
#include <sys/wait.h>

#include "Xproto.h"

int AIXTCPSocket= -1;
static char dummy[20];

extern char *display ;

void
AIXFindServerIndex()
{
struct sockaddr_in insock ;
int sockNum, sockFD ;

    if ((sockFD = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
	Error("creating TCP socket (in FindATCPSocket)\n",NULL);
	display= "0";
	return;
    }

    bzero( (char *)&insock, sizeof insock );
    insock.sin_family = AF_INET;
    insock.sin_addr.s_addr = htonl(INADDR_ANY);

    for ( sockNum = 0 ; sockNum < MAX_SERVER_NUM ; sockNum++ ) {
	insock.sin_port = htons( X_TCP_PORT + sockNum ) ;
	if ( bind( sockFD, (struct sockaddr *) &insock, sizeof insock ) ) {
	    if (errno!=EADDRINUSE) {
		display= "0";
		return;
	    }
	}
	else break;
    }
    AIXTCPSocket= sockFD;
    (void) sprintf( display = dummy, "%d", sockNum ) ;
    return;
}
#endif /* FORCE_DISPLAY_NUMBER */
