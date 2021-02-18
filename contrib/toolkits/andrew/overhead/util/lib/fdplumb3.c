/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fdplumb3.c,v 2.4 89/02/10 23:25:17 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fdplumb3.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <stdio.h>
#include <fdplumbi.h>

FILE *dbg_popen(path, type)
char *path, *type;
{
    FILE *fp;
    extern FILE *popen();

    fp = popen(path, type);
    if (fp) RegisterOpenFile(fileno(fp), path, FDLEAK_OPENCODE_POPEN);
    return(fp);
}

dbg_pclose(fp)
FILE *fp;
{
    RegisterCloseFile(fileno(fp));
    return(pclose(fp));
}

