/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fdplumb5.c,v 2.4 89/02/10 23:25:24 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fdplumb5.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <stdio.h>
#include <fdplumbi.h>

dbg_t2open(name, argv, r, w)
char *name, *argv[];
FILE **r, **w;
{
    int code;

    code = t2open(name, argv, r, w);
    if (!code) {
	RegisterOpenFile(fileno(*w), "t2-r", FDLEAK_OPENCODE_T2OPEN);
	RegisterOpenFile(fileno(*r), "t2-w", FDLEAK_OPENCODE_T2OPEN);
    }
    return(code);
}


dbg_t2close(fp, seconds, timedout)
FILE *fp;
int seconds, *timedout;
{
    RegisterCloseFile(fileno(fp));
    return(t2close(fp, seconds, timedout));
}


