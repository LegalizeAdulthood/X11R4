/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getvers.c,v 2.3 89/02/08 15:33:46 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getvers.c,v $
*/
#include <ms.h>

extern char MS_VersionString[];

MS_GetVersion(Buf, lim) 
char *Buf;
int lim;
{
    debug(1, ("MS_GetVersion\n"));
    debug(4, ("Version number is %s\n", MS_VersionString));
    strncpy(Buf, MS_VersionString, lim);
    Buf[lim-1] = '\0';
    return(0);
}
