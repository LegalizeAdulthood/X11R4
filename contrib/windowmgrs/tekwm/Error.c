#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Error.c,v 5.8 89/11/22 15:12:15 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

/*
 * MODIFICATION HISTORY
 *
 * 000 -- M. Gancarz, DEC Ultrix Engineering Group
 */

#ifndef lint
static char *sccsid = "@(#)Error.c	3.8	1/24/86";
#endif

#if defined(vax)
#include <sys/file.h>
#else
/* <fcntl.h> included in Xos.h */
#endif /* vax */

#include <signal.h>
#include "awm.h"

extern Boolean desktop_execd;
extern char execfile[];

/*
 * Default error reporting routine.  Called when a random tekwm error
 * is encountered.
 */
Error(s)
char *s;	/* Error description string */
{
    Entry("Error")

    fprintf(stderr, "tekwm: %s\n", s);
    if (!desktop_execd) {
         if (fork() == 0) {
	     /* don't permit ctl-C or kill to tekwm to kill the -e program */
#ifdef SYSV
	     signal (SIGINT, SIG_IGN);
	     signal (SIGQUIT, SIG_IGN);
	     signal (SIGTERM, SIG_IGN);
	     signal (SIGHUP, SIG_IGN);
#else /* BSD */
	     setpgrp(0, 0);
#endif /* SYSV */
	     execlp("sh", "sh", "-c", execfile, 0);
	    /* the things we have to do to get execfile invoked from $PATH! */
	    /* if we get here, something went wrong! */
	     fprintf(stderr, "tekwm: Can't execute sh -c %s: ", execfile);
	     perror("");
	     _exit(127);
	 }
	 else
	     desktop_execd = TRUE;
    }
    Cleanup();
    exit(1);
}

void Warning(s)
char *s;	/* Error description string */
{
    Entry("Warning")

    fprintf(stderr, "tekwm: warning: %s\n", s);
    Leave_void
}
