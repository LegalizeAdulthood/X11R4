/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getascft.c,v 2.3 89/02/08 15:32:50 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getascft.c,v $
*/
#include <ms.h>

MS_GetAssociatedFileTime(FullName, fdate)
char *FullName;
long *fdate;
{
    debug(1, ("MS_GetAssociatedFileTime %s\n", FullName));
    return(GetAssocFileTime(FullName, fdate));
}
