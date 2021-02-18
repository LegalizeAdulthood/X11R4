#ifndef CONFIG_H
#define CONFIG_H

/*
 * $Header: /users/ricks/xrn-11/R4/RCS/config.h,v 1.14 89/11/17 12:51:04 ricks Rel $
 */

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
 * config.h: configurable defaults
 */

/* place to report bugs and request new features */
#define GRIPES "xrn@eros.Berkeley.EDU"

/*
 * your internet domain name, examples are:
 *
 * .Berkeley.EDU
 * .mit.EDU
 * .CSS.GOV
 * .CS.NET
 * .BITNET
 * .UUCP
 *
 * wouldn't it be nice if there were a call like 'gethostname'
 * that would get the domain name...
 *
 *  can be overriden with the DOMAIN environment variable or
 *  the DOMAIN_FILE (see below)
 */
#ifndef DOMAIN_NAME
#define DOMAIN_NAME ".Berkeley.EDU"
#endif

/* organization name */
#ifndef ORG_NAME
#define ORG_NAME	""
#endif

/* uucp netnews name (if different from your internet host name) */
/* #define UUCPNAME  "/etc/uucpname" */

#ifndef VMS
/*
 * name of the nntp server is in this file - can be overriden
 * by command line option, X resource, or environment variable
 */
#ifndef SERVER_FILE
#define SERVER_FILE "/usr/local/lib/rn/server"
#endif
/*
 * name of the host to use as the users host name in the From field
 * in a composition is in thie file
 */
#ifndef HIDDEN_FILE
#define HIDDEN_FILE "/usr/local/lib/news/hiddenhost"
#endif
/*
 * name of the host to use as the users hosts name in the Path field 
 * in a composition is in the file
 */
#ifndef PATH_FILE
#define PATH_FILE "/usr/local/lib/news/pathhost"
#endif
/*
 * name of the internet domain is in this file
 */
#ifndef DOMAIN_FILE
#define DOMAIN_FILE "/usr/local/lib/news/domain"
#endif
#else
#define SERVER_FILE "SYS$LOGIN:NNTP.SERVER"
#define HIDDEN_FILE "SYS$LOGIN:HIDDEN.HOST"
#define PATH_FILE "SYS$LOGIN:PATH.HOST"
#define DOMAIN_FILE "SYS$LOGIN:DOMAIN.NAME"
#endif

/*
 * maximum size of a article to be posted (in bytes)
 * - including the header and signature file
 */
#define COMPOSITION_SIZE 65535

/*
 * maximum size of a signature file (in bytes), if file is bigger than this,
 * it is not included
 */
#define MAX_SIGNATURE_SIZE 1000

/*
 * if there are more the this many unread articles in the next
 * group, do not prefetch it
 */

/* prefetch articles and groups */
#define PREFETCH

#ifdef PREFETCH
#define PREFETCH_MAX 50
#endif /* PREFETCH */

/*
 * number of article headers to prefetch when searching subjects
 * in the backwards direction
 */
#define SUBJECTS	10

/*
 * For debugging only, define if you want core dumps, rather than
 * a death notification box and an attempt at updating the .newsrc
 * and cleaning up the temporary files
 *
 */
/* #define DUMPCORE */

/*
 * for various bugs in the toolkit / widget set
 */
/*
 * if popup dialogs cause core dumps or behave strangely, define
 * ACCELERATORBUG - you will need to do this if you have not installed
 * MIT patch #8 - this is the case for DEC UWS 2.1 (ULTRIX/RISC) 
 */
/* #define ACCELERATORBUG */

/* #define TRANSLATIONBUG */	/* DEC (VMS) X windows release */
/* #define ERRORBUG */		/* DEC (VMS) X windows release */

/*
 * double clicking eliminates click and drag multiple selections
 * and under some window managers, the use of button 1.
 */
/* #define NODOUBLECLICK */

/* SYSTEM V regex package */
#if defined(macII) || defined(aiws) || defined(hpux) || defined(SYSV)
#define SYSV_REGEX

/* macII is really weird... */
#ifndef macII
#ifndef SYSV
#define SYSV
#endif
#endif

#endif

/* bsd b* functions */
#if defined(sequent) || defined(BSD)
#define BSD_BFUNCS
#endif

/* v{s,f}printf functions */
#if defined(sequent) || defined(ibm032) || defined(hp300) || defined(BSD)
#define NEED_VPRINTF
#endif

/* strtok function */
#if defined(sequent) || defined(BSD)
#define NEED_STRTOK
#endif


/*
 * non-display defaults
 */
#define SAVEMODE       "normal,headers,onedir"
#define SAVE_DIR_DEFAULT	ONEDIR_SAVE
#ifndef VMS
#define SAVEDIR        "~/News"
#define NEWSRCFILE     "~/.newsrc"
#define SAVENEWSRCFILE "~/.oldnewsrc"
#define SIGNATUREFILE  "~/.signature"
#else
#define SAVEDIR	       "SYS$SCRATCH:"
#define NEWSRCFILE     "SYS$LOGIN:NEWS.RC"
#define SAVENEWSRCFILE "SYS$LOGIN:OLDNEWS.RC"
#define SIGNATUREFILE  "SYS$LOGIN:NEWS.SIGNATURE"
#endif
#define TOPLINES "10"
#define MINLINES "3"
#define MAXLINES "8"
#define LINELENGTH "72"
#define BREAKLENGTH "80"

#ifndef VMS
#define PRINTCOMMAND	"enscript"
#define DEADLETTER     "~/dead.letter"
#define SAVEPOSTINGS   "~/Articles"
#define SENDMAIL       "/usr/lib/sendmail -oi -t"
#define TEMPORARY_DIRECTORY "/tmp"
#else
#define PRINTCOMMAND   "PRINT/DELETE"
#define DEADLETTER     "SYS$SCRATCH:"
#define SAVEPOSTINGS   "SYS$SCRATCH:"
#define SENDMAIL       "MAIL "
#define TEMPORARY_DIRECTORY "SYS$SCRATCH:"
#endif

#endif /* CONFIG_H */
