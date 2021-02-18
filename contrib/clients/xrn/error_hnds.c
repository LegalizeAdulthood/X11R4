
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/error_hnds.c,v 1.9 89/11/17 12:51:18 ricks Rel $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988, 1989, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * error_handlers.c: routines for error/signal handling
 *
 */

#include "copyright.h"
#include <X11/Xos.h>
#include <stdio.h>
#include "config.h"
#include "utils.h"
#include <signal.h>
#include <X11/Intrinsic.h>
#include "dialogs.h"
#include "xmisc.h"
#include "xthelper.h"
#include "internals.h"
#include "xrn.h"
#include "mesg.h"
#include "error_hnds.h"

/*
 * error handlers - major purpose is to close down 'rn' cleanly
 */

extern void exit();

static int
xrnXIOError(display)
Display *display;
{
    perror("XIO Error");
    XRNState &= ~XRN_X_UP;
    ehErrorExitXRN("XIO Error");
}


/*ARGSUSED*/
static int
xrnXError(display, event)
Display *display;
XErrorEvent *event;
{
    char buffer[1024];

#ifdef ERRORBUG
	if (event->request_code == 4) 		/* destroy window errors */
	return;					/* Bug workaround */
#endif

    XGetErrorText(display, event->error_code, buffer, sizeof(buffer));
    (void) fprintf(stderr, "X Error: %s\n", buffer);
    (void) fprintf(stderr, "    serial number: %d\n", event->serial);
    (void) fprintf(stderr, "    error code:  %d\n", (int) event->error_code);
    (void) fprintf(stderr, "    request code:  %d\n", (int) event->request_code);
    (void) fprintf(stderr, "    minor code:  %d\n", (int) event->minor_code);
    (void) fprintf(stderr, "    resource id: %d\n", (int) event->resourceid);
    XRNState &= ~XRN_X_UP;
    ehErrorExitXRN("X Error");
}

#ifndef XTERRORBUG
static int
xrnXtError(errorCode)
int errorCode;
{
    (void) fprintf(stderr, "X Toolkit Error\n");
    XRNState &= ~XRN_X_UP;
    ehErrorExitXRN("X Toolkit Error");
}
#endif /* XTERROR */

void
ehInstallErrorHandlers()
{
#ifndef XTERRORBUG
    XtErrorHandler(xrnXtError);
#endif /* XTERROR */
    XSetErrorHandler(xrnXError);
    XSetIOErrorHandler(xrnXIOError);
    return;
}


static int
sig_catcher(signo)
int signo;
{
    char buffer[80];
    
    /* allow HUP's and INT's to do a clean exit */
    if ((signo == SIGHUP) || (signo == SIGINT)) {
	ehCleanExitXRN();
    }

    (void) sprintf(buffer, "Caught signal (%d), cleaned up .newsrc and removed temp files", signo);

    signal(signo, SIG_DFL);
    ehSignalExitXRN(buffer);
    kill(getpid(), signo);
}


void
ehInstallSignalHandlers()
{
/* #define DUMPCORE */
#ifndef DUMPCORE
    int i;

    for (i = 1; i <= SIGTERM; i++) {
	switch(i) {
#ifdef SIGSTOP
	    case SIGSTOP:
#endif
#ifdef SIGTSTP
	    case SIGTSTP:
#endif
#ifdef SIGCONT
	    case SIGCONT:
#endif
	    break;

	    case SIGPIPE:
	    (void) signal(i, SIG_IGN);
	    break;

	    default:
	    (void) signal(i, sig_catcher);
	    break;
	}
    }
#endif /* DUMPCORE */
    return;
}


static int die = 0;

static void
myAbort()
{
    die = 1;
    return;
}    


static void
myExit()
{
    exit(-1);
}


static void
deathNotifier(message)
char *message;
{
    XEvent ev;

    static struct DialogArg args[] = {
	{"abort", myAbort, (caddr_t) -1},
	{"exit", myExit, (caddr_t) -1},
    };

    if ((XRNState & XRN_X_UP) != XRN_X_UP) {
	(void) fprintf(stderr, "%s\n", message);
	return;
    }

    /* XXX unmap icon */
    XtMapWidget(topLevel);
    PopUpDialog(CreateDialog(topLevel, message, DIALOG_NOTEXT, args, XtNumber(args)));

    while (!die) {
	XtNextEvent(&ev);
	XtDispatchEvent(&ev);
    }

    return;
}


#define XRN_NORMAL_EXIT_BUT_NO_UPDATE 2
#define XRN_NORMAL_EXIT 1
#define XRN_ERROR_EXIT 0
#define XRN_SIGNAL_EXIT -1
      
static void
exitXRN(status, message)
int status;
char *message;   /* message for deathNotifier in ERROR exit */
{
    static int beenHere = 0;

    /*
     * immediate exit, exitXRN was called as a result of something in
     * itself
     */ 
    if (beenHere == 1) {
	exit(-1);
    }
    
    beenHere = 1;

    if ((XRNState & XRN_NEWS_UP) == XRN_NEWS_UP) {
	/* XXX is this really needed?  does free files... */
	releaseNewsgroupResources();

#ifdef PREFETCH  
	/* unlink the first article of the prefetched group */
	freePrefetchedGroupArticle();
#endif /* PREFETCH */
	if (status != XRN_NORMAL_EXIT_BUT_NO_UPDATE) {
	    if (!updatenewsrc()) {
		message = "error during newsrc update";
	    }
	}
    }

    /* clean up the lock */
    removeLock();

    return;
}


void
ehNoUpdateExitXRN()
{
    exitXRN(XRN_NORMAL_EXIT_BUT_NO_UPDATE, NIL(char));
    exit(0);
}


void
ehCleanExitXRN()
{
    exitXRN(XRN_NORMAL_EXIT, NIL(char));
    exit(0);
}


void
ehErrorExitXRN(message)
char *message;
{
    exitXRN(XRN_ERROR_EXIT, message);
    deathNotifier(message);
    exit(-1);
}


void
ehSignalExitXRN(message)
{
    exitXRN(XRN_SIGNAL_EXIT, message);
    deathNotifier(message);
    return;
}    
