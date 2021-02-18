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
/* general.h - general compatibility */

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/general.h,v 1.1 89/09/21 05:22:29 mss Exp $
 *
 *
 * $Log:	general.h,v $
 * Revision 1.1  89/09/21  05:22:29  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:25:39  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:12:13  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:02:28  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:09:57  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:32:03  mrose
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


#ifndef	_GENERAL_
#define	_GENERAL_

#ifndef	_CONFIG_
#include "config.h"		/* system-specific configuration */
#endif


/* target-dependent defines:

	BSDFORK -	target has BSD vfork
	BSDLIBC -	target has real BSD libc
	BSDSTRS -	target has BSD strings
	SWABLIB -	target has byteorder(3n) routines
 */

#ifdef	SOCKETS
#define	SWABLIB
#endif

#ifdef	WINTLI
#define	SWABLIB
#endif

#ifdef	EXOS
#define	SWABLIB
#endif


#ifdef	BSD42
#define	BSDFORK
#define	BSDLIBC
#define	BSDSTRS
#endif

#ifdef	ROS
#undef	BSDFORK
#undef	BSDLIBC
#define	BSDSTRS
#ifndef	BSD42
#define	BSD42
#endif
#undef	SWABLIB
#endif

#ifdef	SYS5
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif

#ifdef	sgi
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif

#ifdef	HPUX
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#undef	SWABLIB
#endif

#ifdef	HPUX
#undef	SWABLIB
#endif

#ifdef	pyr
#undef	SWABLIB
#endif

#ifdef  apollo
#undef  SWABLIB
#endif


#ifndef	BSDFORK
#define	vfork	fork
#endif

/*    STRINGS */

#ifndef	BSDSTRS
#define	index	strchr
#define	rindex	strrchr
#endif

char   *index ();
char   *mktemp ();
char   *rindex ();
#ifdef	BSDSTRS
char   *sprintf ();
#else
int     sprintf ();
#endif
char   *strcat ();
int     strcmp ();
char   *strcpy ();
int	strlen ();
char   *strncat ();
int     strncmp ();
char   *strncpy ();

char   *getenv ();
char   *calloc (), *malloc (), *realloc ();

#if	defined(SYS5) && !defined(AIX) && !defined(OSX)
#include <memory.h>

#define	bcopy(b1,b2,length)	(void) memcpy ((b2), (b1), (length))
#define	bcmp(b1,b2,length)	memcmp ((b1), (b2), (length))
#define	bzero(b,length)		(void) memset ((b), 0, (length))
#endif

/*    HEXIFY */

int	explode (), implode ();

/*    SPRINTB */

char   *sprintb ();

/*    STR2VEC */

#define	NVEC	100
#define	NSLACK	10


#define	str2vec(s,v)	str2vecX ((s), (v), 0, NULLIP, NULL)

int	str2vecX ();

/*    STR2ELEM */

#define	NELEM	20

int	str2elem ();

/*    STR2SEL */

int	str2sel ();
char   *sel2str ();

/*    GETPASS */

char   *getpassword ();

/*    BADUSER */

int	baduser ();

/*   UTILITIES */

extern char chrcnv[], nochrcnv[];


int	lexequ (), lexnequ ();

int	log_tai ();

int	sstr2arg ();

char    *smalloc (), *strdup ();

/*    MISC */

char   *sys_errname ();

#ifdef	lint
#define	insque(e,p)	INSQUE ((char *) (e), (char *) (p))
#define	remque(e)	REMQUE ((char *) (e))
#endif


void	asprintf (), _asprintf ();

void	isodetailor ();		/* also in tailor.h */


#ifndef	ntohs
unsigned short	ntohs ();
#endif
#ifndef	htons
unsigned short	htons ();
#endif
#ifndef	ntohl
unsigned long	ntohl ();
#endif
#ifndef	htonl
unsigned long	htonl ();
#endif

#endif
