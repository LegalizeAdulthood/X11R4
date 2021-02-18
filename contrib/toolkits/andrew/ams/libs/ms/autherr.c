/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/autherr.c,v 2.5 89/02/15 11:38:47 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/autherr.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/autherr.c,v 2.5 89/02/15 11:38:47 ghoti Exp $ ";
#endif /* lint */

#include <andyenv.h>
#include <ms.h>
#ifdef AFS_ENV
#include <sys/ioctl.h>
#include <afs/errors.h>
#include <afs/prs_fs.h>
#include <afs/venus.h>
#endif /* AFS_ENV */

extern char home[];

MS_CheckAuthentication(Authenticated)
int *Authenticated;
{
#ifdef AFS_ENV
    struct ViceIoctl    blob;
    long authcode;
#endif /* AFS_ENV */

    *Authenticated = 1;
#ifdef AFS_ENV
    if (AMS_ViceIsRunning) {
    authcode = 0;
    blob.out = (char *) &authcode;
    blob.out_size = sizeof authcode;
    blob.in_size = 0;
    if (pioctl(home, VIOCCKCONN, &blob, 1)) {
	AMS_RETURN_ERRCODE(errno, EIN_PIOCTL, EVIA_CHECKAUTH);
    } else {
	if (authcode != 0) {
	    *Authenticated = 0;
	}
    }
    }
#endif /* AFS_ENV */
    return(0);
}

