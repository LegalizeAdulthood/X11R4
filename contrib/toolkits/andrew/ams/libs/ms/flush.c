/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/flush.c,v 2.6 89/02/15 11:40:14 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/flush.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/flush.c,v 2.6 89/02/15 11:40:14 ghoti Exp $ ";
#endif /* lint */

#include <andyenv.h>
#include <andrewos.h>

#include <ms.h>
#ifdef AFS_ENV
#include <sys/ioctl.h>
#include <afs/errors.h>
#include <afs/prs_fs.h>
#include <afs/venus.h>
#endif /* AFS_ENV */

extern char *rindex();

ViceFlush(fname)
char *fname;
{
#ifdef AFS_ENV
    struct ViceIoctl blob;
    if (AMS_ViceIsRunning) {
	blob.out_size = 0;
	blob.in_size = 0;

	if (pioctl(fname, VIOCFLUSHCB, &blob)) {
	    debug(1, ("Callback flush of %s FAILED!\n", fname));
	    if (pioctl(fname, VIOCFLUSH, &blob)) {
		debug(1, ("Both flushes of %s FAILED!\n", fname));
		AMS_RETURN_ERRCODE(errno, EIN_PIOCTL, EVIA_PREFETCHMSG);
	    }
	}
	debug(1, ("ViceFlush of %s succeeded\n", fname));
    }
#endif /* AFS_ENV */
    return(0);
}

ViceFlushPlusParent(fname)
char *fname;
{
    long code = 0;
#ifdef AFS_ENV
    char *s;

    if (AMS_ViceIsRunning) {
    if (ViceFlush(fname)) return(mserrcode);
    s = rindex(fname, '/');
    if (s) {
	*s = '\0';
	code = ViceFlush(fname);
	*s = '/';
    }
    }
#endif /* AFS_ENV */
    return(code);
}
