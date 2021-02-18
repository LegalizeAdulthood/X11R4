/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fdplumb4.c,v 2.4 89/02/10 23:25:21 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fdplumb4.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <stdio.h>
#include <fdplumbi.h>

FILE *dbg_qopen(path, argv, mode)
char *path, *argv[], *mode;
{
    FILE *fp;

    fp = (FILE *) qopen(path, argv, mode);
    if (fp) RegisterOpenFile(fileno(fp), path, FDLEAK_OPENCODE_QOPEN);
    return(fp);
}

FILE *dbg_topen(path, argv, mode, pgrp)
char *path, *argv[], *mode;
int *pgrp;
{
    FILE *fp;
    extern FILE *topen();

    fp = topen(path, argv, mode, pgrp);
    if (fp) RegisterOpenFile(fileno(fp), path, FDLEAK_OPENCODE_TOPEN);
    return(fp);
}

dbg_qclose(fp)
FILE *fp;
{
    RegisterCloseFile(fileno(fp));
    return(qclose(fp));
}

dbg_tclose(fp, seconds, timedout)
FILE *fp;
int seconds, *timedout;
{
    RegisterCloseFile(fileno(fp));
    return(tclose(fp, seconds, timedout));
}

