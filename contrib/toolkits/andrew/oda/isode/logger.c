/*
Copyright 1988, 1989 Marshall T. Rose

The ISODE is openly available but is NOT in the public domain. You are
allowed and encouraged to take this software and build commercial
products. However, as a condition of use, you are required to "hold
harmless" all contributors.
Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted,
provided that this notice and the reference to this notice appearing
in each software module be retained unaltered, and that the name of
any contributors shall not be used in advertising or publicity pertaining
to distribution of the software without specific written prior permission.
No contributor makes any representations about the suitability of this
software for any purpose. It is provided "as is" without express or
implied warranty.

ALL CONTRIBUTORS DISCLAIM ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OR MERCHANTIBILITY AND FITNESS. IN
NO EVENT SHALL ANY CONTRIBUTOR BE LIABLE FOR
ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFWTARE.

As used above, "contributor" includes, but is not limited to:

	The MITRE Corporation
	The Northrop Corporation
	University College London
	The University of Nottingham
	The Wollongong Group, Inc.
	Marshall T. Rose

In particular, the Northrop Coporation provided the initial
sponsorship for the ISODE and teh Wollongong Group, Inc.,
continued supporting this effort.
*/
/* logger.c - system logging routines */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/logger.c,v 1.2 89/09/26 12:34:38 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/logger.c,v 1.2 89/09/26 12:34:38 mss Exp $
 *
 *
 * $Log:	logger.c,v $
 * Revision 1.2  89/09/26  12:34:38  mss
 * Reverted back to original source + copyright
 * 
 * Revision 1.2  89/09/11  16:13:03  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:02:36  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  15:35:57  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:25:23  mrose
 * Release 5.0
 * 
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <stdio.h>
#include <varargs.h>
#include "general.h"
#include "manifest.h"
#include "logger.h"
#include "tailor.h"

#ifdef	NULL
#undef	NULL
#endif
#include <sys/param.h>
#ifndef	NULL
#define	NULL	0
#endif
#ifdef	BSD42
#include <sys/file.h>
#endif
#ifdef	SYS5
#include <fcntl.h>
#endif
#include <sys/stat.h>

#ifndef	BSD42
#include <time.h>
#else
#ifndef	timerisset
#include <sys/time.h>
#endif
#endif

#ifndef	SYS5
#include <syslog.h>
#endif

/*  */

long	lseek (), time ();

/*  */

int	ll_open (lp)
register LLog *lp;
{
    int	    mask,
	    mode;
    char   *bp,
	    buffer[BUFSIZ];
    struct stat st;

    if (lp -> ll_file == NULLCP
	    || *lp -> ll_file == NULL
	    || ll_check (lp) == NOTOK) {
	(void) ll_close (lp);
	lp -> ll_stat |= LLOGERR;
	return NOTOK;
    }

    lp -> ll_stat &= ~LLOGERR;

    if (lp -> ll_fd != NOTOK)
	return OK;

    if (strcmp (lp -> ll_file, "-") == 0) {
	lp -> ll_stat |= LLOGTTY;
	return OK;
    }

    (void) sprintf (bp = buffer, _isodefile (isodelogs, lp -> ll_file),
		    getpid ());

    mode = O_WRONLY | O_APPEND;
    if (stat (bp, &st) == NOTOK && (lp -> ll_stat & LLOGCRT))
	mode |= O_CREAT;

    mask = umask (~0666);
    lp -> ll_fd = open (bp, mode, 0666);
    (void) umask (mask);

    return (lp -> ll_fd != NOTOK ? OK : NOTOK);
}

/*  */

int	ll_close (lp)
register LLog *lp;
{
    int	    status;
    
    if (lp -> ll_fd == NOTOK)
	return OK;

    status = close (lp -> ll_fd);
    lp -> ll_fd = NOTOK;

    return status;
}

/*  */

#ifndef	lint
int	ll_log (va_alist)
va_dcl
{
    int	    event,
	    result;
    LLog   *lp;
    va_list ap;

    va_start (ap);

    lp = va_arg (ap, LLog *);
    event = va_arg (ap, int);

    result = _ll_log (lp, event, ap);

    va_end (ap);

    return result;
}
#else
/* VARARGS4 */

int	ll_log (lp, event, what, fmt)
LLog   *lp;
int	event;
char   *what,
       *fmt;
{
    return ll_log (lp, event, what, fmt);
}
#endif

/*  */

int	_ll_log (lp, event, ap)	/* what, fmt, args ... */
register LLog *lp;
int	event;
va_list	ap;
{
    int	    cc,
	    status;
    long    clock;
    register char *bp;
    char   *what,
	    buffer[BUFSIZ];
    register struct tm *tm;

    if (!(lp -> ll_events & event))
	return OK;

    bp = buffer;

    (void) time (&clock);
    tm = localtime (&clock);
    (void) sprintf (bp, "%2d/%2d %2d:%02d:%02d %s %s ",
		    tm -> tm_mon + 1, tm -> tm_mday,
		    tm -> tm_hour, tm -> tm_min, tm -> tm_sec,
		    lp -> ll_hdr ? lp -> ll_hdr : "",
		    lp -> ll_dhdr ? lp -> ll_dhdr : "");
    bp += strlen (bp);

    what = va_arg (ap, char *);

    _asprintf (bp, what, ap);

#ifndef	SYS5
    if (lp -> ll_syslog & event) {
	int	priority;

	switch (event) {
	    case LLOG_FATAL:
	        priority = LOG_ERR;
		break;

	    case LLOG_EXCEPTIONS:
		priority = LOG_WARNING;
		break;

	    case LLOG_NOTICE:
	        priority = LOG_INFO;
		break;

	    case LLOG_PDUS:
	    case LLOG_TRACE:
	    case LLOG_DEBUG:
	        priority = LOG_DEBUG;
		break;

	    default:
		priority = LOG_NOTICE;
		break;
	}

	(void) syslog (priority, "%s", buffer + 13);

	if (lp -> ll_stat & LLOGCLS)
	    (void) closelog ();
    }
#endif

    if (!(lp -> ll_stat & LLOGTTY)
	    && lp -> ll_fd == NOTOK
	    && strcmp (lp -> ll_file, "-") == 0)
	lp -> ll_stat |= LLOGTTY;

    if (lp -> ll_stat & LLOGTTY) {
	(void) fflush (stdout);

	if (lp -> ll_fd != NOTOK)
	    (void) fprintf (stderr, "LOGGING: ");
	(void) fputs (bp, stderr);
	(void) fputc ('\n', stderr);
	(void) fflush (stderr);
    }
    bp += strlen (bp);

    if (lp -> ll_fd == NOTOK) {
	if ((lp -> ll_stat & (LLOGERR | LLOGTTY)) == (LLOGERR | LLOGTTY))
	    return OK;
	if (ll_open (lp) == NOTOK)
	    return NOTOK;
    }
    else
	if (ll_check (lp) == NOTOK)
	    return NOTOK;

    *bp++ = '\n', *bp = NULL;
    cc = bp - buffer;

    if ((status = write (lp -> ll_fd, buffer, cc)) != cc) {
	if (status == NOTOK) {
	    (void) ll_close (lp);
error: ;
	    lp -> ll_stat |= LLOGERR;
	    return NOTOK;
	}

	status = NOTOK;
    }
    else
	status = OK;

    if ((lp -> ll_stat & LLOGCLS) && ll_close (lp) == NOTOK)
	goto error;

    return status;
}

/*  */

void	ll_hdinit (lp, prefix)
register LLog *lp;
char   *prefix;
{
    register char  *cp,
		   *up;
    char    buffer[BUFSIZ],
	    user[10];

    if (prefix == NULLCP)
	cp = "unknown";
    else {
	if ((cp = rindex (prefix, '/')))
	    cp++;
	if (cp == NULL || *cp == NULL)
	    cp = prefix;
    }

    if ((up = getenv ("USER")) == NULLCP
	    && (up = getenv ("LOGNAME")) == NULLCP) {
	(void) sprintf (user, "#%d", getuid ());
	up = user;
    }
    (void) sprintf (buffer, "%-8.8s %05d (%-8.8s)",
		    cp, getpid () % 100000, up);

    if (lp -> ll_stat & LLOGHDR)
	free (lp -> ll_hdr);
    lp -> ll_stat &= ~LLOGHDR;

    if ((lp -> ll_hdr = malloc ((unsigned) (strlen (buffer) + 1))) == NULLCP)
	return;

    (void) strcpy (lp -> ll_hdr, buffer);
    lp -> ll_stat |= LLOGHDR;
}

/*  */

void	ll_dbinit (lp, prefix)
register LLog *lp;
char   *prefix;
{
    register char  *cp;
    char    buffer[BUFSIZ];

    ll_hdinit (lp, prefix);

    if (prefix) {
	if ((cp = rindex (prefix, '/')))
	    cp++;
	if (cp == NULL || *cp == NULL)
	    cp = prefix;

	(void) sprintf (buffer, "./%s.log", cp);

	if ((lp -> ll_file = malloc ((unsigned) (strlen (buffer) + 1)))
	        == NULLCP)
	    return;

	(void) strcpy (lp -> ll_file, buffer);
    }

    lp -> ll_events |= LLOG_ALL;
    lp -> ll_stat |= LLOGTTY;
}

/*  */

#ifndef	lint
int	ll_printf (va_alist)
va_dcl
{
    int	    result;
    LLog    *lp;
    va_list ap;

    va_start (ap);

    lp = va_arg (ap, LLog *);

    result = _ll_printf (lp, ap);

    va_end (ap);

    return result;
}
#else
/* VARARGS2 */

int	ll_printf (lp, fmt)
LLog   *lp;
char   *fmt;
{
    return ll_printf (lp, fmt);
}
#endif

/*  */

#ifndef	lint
static
#endif
int  _ll_printf (lp, ap)		/* fmt, args ... */
register LLog *lp;
va_list	ap;
{
    int	    cc,
	    status;
    register char   *bp;
    char     buffer[BUFSIZ];
    char    *fmt;
    va_list fp = ap;

    fmt = va_arg (fp, char *);
    if (strcmp (fmt, "%s") != 0) {
	bp = buffer;
	_asprintf (bp, NULLCP, ap);
    }
    else {
	bp = NULL;
	fmt = va_arg (fp, char *);
    }

    if (!(lp -> ll_stat & LLOGTTY)
	    && lp -> ll_fd == NOTOK
	    && strcmp (lp -> ll_file, "-") == 0)
	lp -> ll_stat |= LLOGTTY;

    if (lp -> ll_stat & LLOGTTY) {
	(void) fflush (stdout);

	if (bp)
	    (void) fputs (bp, stderr);
	else
	    (void) fputs (fmt, stderr);
	(void) fflush (stderr);
    }
    if (bp)
	bp += strlen (bp);

    if (lp -> ll_fd == NOTOK) {
	if ((lp -> ll_stat & (LLOGERR | LLOGTTY)) == (LLOGERR | LLOGTTY))
	    return OK;
	if (ll_open (lp) == NOTOK)
	    return NOTOK;
    }
    else
	if (ll_check (lp) == NOTOK)
	    return NOTOK;

    if (bp)
	cc = bp - buffer;
    else
	cc = strlen (fmt);

    if ((status = write (lp -> ll_fd, bp ? buffer : fmt, cc)) != cc) {
	if (status == NOTOK) {
	    (void) ll_close (lp);
	    lp -> ll_stat |= LLOGERR;
	    return NOTOK;
	}

	status = NOTOK;
    }
    else
	status = OK;

    return status;
}

/*  */

int	ll_sync (lp)
register LLog *lp;
{
    if (lp -> ll_stat & LLOGCLS)
	return ll_close (lp);

    return OK;
}

/*  */

#ifndef	lint
char   *ll_preset (va_alist)
va_dcl
{
    va_list ap;
    static char buffer[BUFSIZ];

    va_start (ap);

    _asprintf (buffer, NULLCP, ap);

    va_end (ap);

    return buffer;
}
#else
/* VARARGS1 */

char   *ll_preset (fmt)
char   *fmt;
{
    return ll_preset (fmt);
}
#endif

/*  */

int	ll_check (lp)
register LLog *lp;
{
#ifndef	BSD42
    int	    fd;
#endif
    long    size;
    struct stat st;

    if ((size = lp -> ll_msize) <= 0)
	return OK;

    if (lp -> ll_fd == NOTOK
	    || (fstat (lp -> ll_fd, &st) != NOTOK
		    && st.st_size < (size <<= 10)))
	return OK;

    if (!(lp -> ll_stat & LLOGZER)) {
	(void) ll_close (lp);

#ifndef	BSD42
error: ;
#endif
	lp -> ll_stat |= LLOGERR;
	return NOTOK;
    }

#ifdef	BSD42
#ifdef	SUNOS4
    (void) ftruncate (lp -> ll_fd, (off_t) 0);
#else
    (void) ftruncate (lp -> ll_fd, 0);
#endif
    (void) lseek (lp -> ll_fd, 0L, 0);
    return OK;
#else
    (void) ll_close (lp);
    if ((fd = open (lp -> ll_file, O_WRONLY | O_APPEND | O_TRUNC)) == NOTOK)
	goto error;
    (void) close (fd);
    return ll_open (lp);
#endif
}
