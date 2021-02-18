/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
 * $XConsortium: Xos.h,v 1.10 88/09/06 14:30:21 jim Exp $
 * 
 * Copyright 1987 by the Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. M.I.T. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * The X Window System is a Trademark of MIT.
 *
 */

/* This is a collection of things to try and minimize system dependencies
 * in a "signficant" number of source files.
 */

/* Need to check on hpux use of utime instead of utimes */

#ifndef _XOS_H_
#define _XOS_H_

/* Make sure that locally defined system-specific flags are defined */
#include <system.h>

/* make sure that if a SystemV derivitive is used that SYSV is defined */
#if SY_U5x
#ifndef SYSV
#define SYSV	1
#endif SYSV
#endif SY_U5x

#ifdef hpux
#ifndef SYSV
#define SYSV	1
#endif SYSV
#endif hpux

#if SY_AIX11
/* _BSD must be defined to get right "struct direct" from sys/dir.h */
#define _BSD
#endif /* SY_AIX11 */

/*
 * Get major data types (esp. caddr_t)
 */

#ifdef CRAY
#ifndef __TYPES__
#define __TYPES__
#include <sys/types.h>			/* forgot to protect it... */
#endif /* __TYPES__ */
#else
#include <sys/types.h>
#endif /* CRAY */


/*
 * Just about everyone needs the strings routines.  For uniformity, we use
 * the BSD-style index() and rindex() in application code, so any systems that
 * don't provide them need to have #defines here.  Unfortunately, we can't
 * use #if defined() here since makedepend will get confused.
 *
 * The list of systems that currently needs System V stings includes:
 *
 *	hpux
 * 	macII
 *	CRAY
 */

#ifdef SYSV
#define SYSV_STRINGS
#endif /* SYSV */

#ifdef macII				/* since it doesn't define SYSV */
#define SYSV_STRINGS
#endif /* macII */

#ifdef SYSV_STRINGS
#include <string.h>
#define index strchr
#define rindex strrchr
#undef SYSV_STRINGS
#else
#include <strings.h>
#endif /* SYSV_STRINGS */


/*
 * Get open(2) constants
 */

#ifdef SYSV
#include <fcntl.h>
#endif /* SYSV */
#include <sys/file.h>


/*
 * Get struct timeval
 */

#ifdef macII
#include <time.h>		/* need this as well as sys/time.h */
#endif /* macII */

#ifdef SYSV
#include <time.h>
# ifdef mips
# include <bsd/sys/time.h>
# endif /* mips */
#else /* else not SYSV */
#include <sys/time.h>
#endif /* SYSV */

/*
 * More BSDisms
 */

#ifdef SYSV
#define SIGCHLD SIGCLD
#endif /* SYSV */

/* include path for syslog.h BSD vs SYSV */
#ifdef AIX
#include <sys/syslog.h>
#else /* AIX */
#ifndef hpux
#include <syslog.h>
#endif /* hpux */
#endif /* AIX */

/* VMUNIX vs. SY_B4x */
#if SY_B4x
#ifndef VMUNIX
#define	VMUNIX	1
#endif VMUNIX
#endif SY_B4x

/* getdtablesize() and an errno which does not seem to be defined for SYSV */
#ifdef SYSV
#define EDOM				33
#define getdtablesize()			_NFILE
#define setpriority(which,who,prio) (nice((prio)-nice(0)))
#endif SYSV


/*
 * handle (BSD) flock vs. (SYSV) lockf through defines
  * osi == Operating System Independent
  * locking with blocking is not enabled in AFS - so not defined below
 */

/* 
 * Define this as 1 for systems with a readlink(2) system call, 0 for
 * systems without.
 */
  
#if (SY_B4x || (defined(AIX)))
#define OSI_HAS_SYMLINKS 1
#else /* (SY_B4x || (defined(AIX))) */
#define OSI_HAS_SYMLINKS 0
#endif /* (SY_B4x || (defined(AIX))) */


#if OSI_HAS_SYMLINKS
#define osi_readlink(PATH,BUF,SIZE) readlink((PATH),(BUF),(SIZE))
#else
/* If OSI_HAS_SYMLINKS is not defined, osi_readlink is present in libutil. */
extern int osi_readlink();
#endif /* OSI_HAS_SYMLINKS */

#if (defined(SYSV) || defined(AIX))
#ifdef hpux
#include <unistd.h>
#else
#include <sys/lockf.h>
#endif /* hpux */
#define osi_ExclusiveLockNoBlock(fid)	lockf((fid), F_TLOCK, 0)
#define osi_UnLock(fid)			lockf((fid), F_ULOCK, 0)
#define osi_O_READLOCK			O_RDWR
#define osi_F_READLOCK			"r+"
#else /* (defined(SYSV) || defined(AIX)) */
#define osi_ExclusiveLockNoBlock(fid)	flock((fid), LOCK_EX | LOCK_NB)
#define osi_UnLock(fid)			flock((fid), LOCK_UN)
#define osi_O_READLOCK			O_RDONLY
#define osi_F_READLOCK			"r"
#endif /* (defined(SYSV) || defined(AIX)) */

/* handle (BSD) vfork for (AIX) which only knows fork */
#if SY_AIXx
#define	osi_vfork()			fork()
#else /* SY_AIXx */
#define	osi_vfork()			vfork()
#endif /* SY_AIXx */

/* Handle the absence of _setjmp and _longjmp on AIX. */
#if SY_AIXx
#define	osi_setjmp  setjmp
#define	osi_longjmp longjmp
#else /* SY_AIXx */
#define	osi_setjmp  _setjmp
#define	osi_longjmp _longjmp
#endif /* SY_AIXx */

/* Make a time standard. */
struct osi_Times {unsigned long int Secs; unsigned long int USecs;};
/* Set one of the above with a call to osi_GetTimes(&foo) */
#define osi_GetSecs() time((long int *) 0)
#if (SY_AIXx || SY_U5x)
#define osi_SetZone() tzset()
#define osi_ZoneNames tzname
#define osi_SecondsWest timezone
#define osi_IsEverDaylight daylight
#else /* #if (SY_AIXx || SY_U5x) */
/* e.g., BSD */
extern void osi_SetZone();
extern char *osi_ZoneNames[];
extern long int osi_SecondsWest;
extern int osi_IsEverDaylight;
#endif /* #if (SY_AIXx || SY_U5x) */

/* More BSD-isms */
#if (defined(SYSV) || defined(AIX))
#define setreuid(r,e) setuid(r)
#define setlinebuf(file) setvbuf(file, NULL, _IOLBF, BUFSIZ)
#endif

/*
 * Put system-specific definitions here
 */

#ifdef mips
# ifdef SYSTYPE_SYSV
# include <bsd/sys/ioctl.h>
# include <bsd/sys/file.h>
# endif /* SYSTYPE_SYSV */
#endif /* mips */

#ifndef hpux
#define HAS_SYSEXITS 1
#endif /* hpux */

#ifdef hpux
#undef setreuid
#define setreuid(r,e) setresuid(r,e,-1)

#define sigvec sigvector

#define random rand
#define srandom srand
#define initstate(a,b,c) srand(a)
#define bcopy(src, dst, length) memcpy(dst, src, length)
#define bzero(b, length) memset(b, 0, length)
#define bcmp(region1, region2, length) memcmp(region1, region2, length)
#define killpg(pgid, signal) kill(-(pgid), signal)

#include <sys/param.h>
#define getwd(pathname) getcwd(pathname, MAXPATHLEN)
#endif /* hpux */

#ifndef HAS_SYSEXITS
/* hpux, for example, has no sysexits.h file anywhere. */
#ifndef EX__BASE
#define EX_OK 0
#define EX__BASE 64
#define EX_USAGE 64
#define EX_DATAERR 65
#define EX_NOINPUT 66
#define EX_NOUSER 67
#define EX_NOHOST 68
#define EX_UNAVAILABLE 69
#define EX_SOFTWARE 70
#define EX_OSERR 71
#define EX_OSFILE 72
#define EX_CANTCREAT 73
#define EX_IOERR 74
#define EX_TEMPFAIL 75
#define EX_PROTOCOL 76
#define EX_NOPERM 77
#endif /* #ifndef EX__BASE */
#endif /* #ifndef HAS_SYSEXITS */

#endif /* _XOS_H_ */
