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
/* logger.h - logging routines */

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/logger.h,v 1.1 89/09/21 05:47:17 mss Exp $
 *
 *
 * $Log:	logger.h,v $
 * Revision 1.1  89/09/21  05:47:17  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:51:50  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:13:17  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:02:39  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:10:06  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:32:08  mrose
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


#ifndef	_LOGGER_
#define	_LOGGER_

/*  */

typedef struct  ll_struct {
    char   *ll_file;		/* path name to logging file */

    char   *ll_hdr;		/* text to put in opening line */
    char   *ll_dhdr;		/* dynamic header - changes */

    int	    ll_events;		/* interesting events */
#define	LLOG_NONE	0
#define	LLOG_FATAL	0x01	/*   fatal errors */
#define	LLOG_EXCEPTIONS	0x02	/*   exceptional events */
#define	LLOG_NOTICE	0x04	/*   informational notices */
#define	LLOG_PDUS	0x08	/*   PDU printing */
#define	LLOG_TRACE	0x10	/*   program tracing */
#define	LLOG_DEBUG	0x20	/*   full debugging */
#define	LLOG_ALL	0xff
#define	LLOG_MASK \
    "\020\01FATAL\02EXCEPTIONS\03NOTICE\04PDUS\05TRACE\06DEBUG"

    int	    ll_syslog;		/* interesting events to send to syslog */
				/*   takes same values as ll_events */

    int     ll_msize;		/* max size for log, in Kbytes */

    int     ll_stat;		/* assorted switches */
#define	LLOGNIL		0x00
#define	LLOGCLS		0x01	/*   keep log closed, except when writing */
#define	LLOGCRT		0x02	/*   create log if necessary */
#define	LLOGZER		0x04	/*   truncate log when limits reached */
#define	LLOGERR		0x08    /*   log closed due to (soft) error */
#define	LLOGTTY		0x10	/*   also log to stderr */
#define	LLOGHDR		0x20    /*   static header allocated */
#define	LLOGDHR		0x40    /*   dynamic header allocated */

    int     ll_fd;		/* file descriptor */
} LLog;

/*  */

#define	SLOG(lp,event,what,args) \
if (lp -> ll_events & (event)) { \
    (void) ll_log (lp, event, what, "%s", ll_preset args); \
} \
else

#ifndef	LLOG
#define	LLOG(lp,event,args)	SLOG (lp, event, NULLCP, args)
#endif

#ifdef	DEBUG
#define	DLOG(lp,event,args)	SLOG (lp, event, NULLCP, args)
#else
#define	DLOG(lp,event,args)
#endif


#ifdef	DEBUG
#define	PLOG(lp,fnx,pe,text,rw)	\
    if ((lp) -> ll_events & LLOG_PDUS) { \
	vpdu (lp, fnx, pe, text, rw); \
} \
else
#else
#define	PLOG(lp,fnx,pe,text,rw)
#endif


int	ll_open ();
int	ll_log (), _ll_log ();
int	ll_close ();

void	ll_hdinit ();
void	ll_dbinit ();

int	ll_printf ();
int	ll_sync ();

char   *ll_preset ();

int	ll_check ();
#endif
