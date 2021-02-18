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
/* manifest.h - manifest constants */

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/manifest.h,v 1.2 89/10/04 14:08:21 ajp Exp $
 *
 *
 * $Log:	manifest.h,v $
 * Revision 1.2  89/10/04  14:08:21  ajp
 * Added ifndef hpux for unsigned typedefs.  They are already
 * defined in sys/types.h on the hp machine.
 * 
 * Revision 1.1  89/09/21  05:47:39  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:52:15  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:13:28  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:02:43  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:10:08  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:32:10  mrose
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


#ifndef	_MANIFEST_
#define	_MANIFEST_

#ifndef	_CONFIG_
#include "config.h"		/* system-specific configuration */
#endif


/* target-dependent defines:

	SYS5NLY -	target has SYS5 types only, no BSD types

	BSDSIGS	-	target supports BSD signals

 */

#ifdef	BSD42
#undef	SYS5NLY
#define	BSDSIGS
#endif 


#ifdef	ROS
#undef	SYS5NLY
#define	BSDSIGS

#ifndef	BSD42
#define	BSD42
#endif

#endif


#ifdef	SYS5
#define	SYS5NLY
#undef	BSDSIGS

#ifdef	OSX
#undef	SYS5NLY
#endif

#if	defined(WIN) || defined (WINTLI)
#undef	SYS5NLY
#endif

#ifdef	sun
#undef	SYS5NLY
#endif

#ifdef	sgi
#undef	SYS5NLY
#endif

#ifdef	HPUX
#undef	SYS5NLY
#define	BSDSIGS
#endif

#ifdef	AIX
#undef	SYS5NLY
#define	BSDSIGS
#define	SIGEMT	SIGUSR1


int   (*_signal ()) ();

#define	signal	_signal
#endif

#endif


#ifdef	NSIG

#ifndef	sigmask
#define	sigmask(s)	(1 << ((s) - 1))
#endif

#ifdef	SIGPOLL
#define	_SIGIO	SIGPOLL
#else
#ifdef	SIGUSR1
#define	_SIGIO	SIGUSR1
#else
#define	_SIGIO	SIGEMT
#endif
#endif

typedef	int	SBV;
#define	sigioblock()	sigblock (sigmask (_SIGIO))
#define	sigiomask(s)	sigsetmask (s)
#define	siginblock()	sigblock (sigmask (SIGINT))
#define	siginmask(s)	sigsetmask (s)

#endif

/*    TYPES */

#ifndef	NOTOK
#define	NOTOK		(-1)
#define	OK		0
#define	DONE		1
#endif


#ifndef	NULLCP
typedef char *CP;
#define	NULLCP		((char *) 0)
#define	NULLVP		((char **) 0)
#endif


#ifndef	makedev
#include <sys/types.h>
#if	defined(WIN) || defined(WINTLI)
#include "sys/inet.h"
#ifndef	NFDBITS
typedef struct fd_set { int fds_bits[1]; } fd_set;
#endif
#endif

#ifdef	SYS5NLY
#ifndef hpux
typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;
#endif
#endif
#endif


#ifndef FD_SET
#define	FD_SETSIZE	    (sizeof (fd_set) * 8)

#define FD_SET(f,s)	    ((s)->fds_bits[0] |= (1 << (f)))
#define FD_CLR(f,s)	    ((s)->fds_bits[0] &= ~(1 << (f)))
#define FD_ISSET(f,s)	    ((s)->fds_bits[0] & (1 << (f)))
#define FD_ZERO(s)	    ((s)->fds_bits[0] = 0)
#endif

#define	NULLFD		((fd_set *) 0)


#undef	IP
typedef int	*IP;
#define	NULLIP		((IP) 0)


typedef	int	(*IFP) ();
#define	NULLIFP		((IFP) 0)


typedef void   (*VFP) ();
#define	NULLVFP		((VFP) 0)


#if	!defined(SVR3) && !defined(SUNOS4)
#define	SFD	int
#define	SFP	IFP
#else
#define	SFD	void
#define	SFP	VFP
#endif


struct udvec {			/* looks like a BSD iovec... */
    caddr_t uv_base;
    int	    uv_len;

    int	    uv_inline;
};


struct qbuf {
    struct qbuf *qb_forw;	/* doubly-linked list */
    struct qbuf *qb_back;	/*   .. */

    int	    qb_len;		/* length of data */
    char   *qb_data;		/* current pointer into data */
    char    qb_base[1];		/* extensible... */
};

#define	QBFREE(qb) \
{ \
    register struct qbuf *QB, \
			 *QP; \
 \
    for (QB = (qb) -> qb_forw; QB != (qb); QB = QP) { \
	QP = QB -> qb_forw; \
	remque (QB); \
	free ((char *) QB); \
    } \
}


#ifndef	min
#define	min(a, b)	((a) < (b) ? (a) : (b))
#define	max(a, b)	((a) > (b) ? (a) : (b))
#endif


#ifdef SYS5

#if	!defined(WINTLI) && !defined(WIN)
#ifndef	sun
#define	getdtablesize()	_NFILE
#endif
#else
#define	getdtablesize()	(_NFILE - 1)
#endif

#ifdef	RT
#define	ntohs(x)	(x)
#define	htons(x)	(x)
#define	ntohl(x)	(x)
#define	htonl(x)	(x)
#endif

#endif

#endif
