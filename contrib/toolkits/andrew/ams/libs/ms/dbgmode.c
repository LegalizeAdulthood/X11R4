/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/dbgmode.c,v 2.4 89/08/01 12:33:29 cfe Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/dbgmode.c,v $
*/
#include <ms.h>
#include <andyenv.h>

extern int SNAP_debugmask;

MS_DebugMode(level, snap, malloc)
int level, snap, malloc;
{
    debug(1, ("MS_DebugMode %d %d %d\n", level, snap, malloc));
    MSDebugging = level;
    SNAP_debugmask = snap;
#ifdef ANDREW_MALLOC_ENV
    SetMallocCheckLevel(malloc);
#endif /* #ifdef ANDREW_MALLOC_ENV */
    return(0);
}
