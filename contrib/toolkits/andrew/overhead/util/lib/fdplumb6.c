/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fdplumb6.c,v 2.4 89/02/10 23:25:28 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fdplumb6.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */


#include <andrewos.h>    /* types */
#include <sys/dir.h>
#include <fdplumbi.h>

DIR *dbg_opendir(name)
char *name;
{
    DIR *d;

    d = opendir(name);
    if (d) {
	RegisterOpenFile(d->dd_fd, name, FDLEAK_OPENCODE_OPENDIR);
    }
    return(d);
}


void dbg_closedir(d)
DIR *d;
{
    RegisterCloseFile(d->dd_fd);
    closedir(d);
}


