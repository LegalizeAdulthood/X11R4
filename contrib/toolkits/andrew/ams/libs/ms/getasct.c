/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getasct.c,v 2.3 89/02/08 15:32:53 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getasct.c,v $
*/
#include <ms.h>

MS_GetAssociatedTime(FullName, Answer, lim)
char *FullName, *Answer;
int lim;
{
    debug(1, ("MS_GetAssociatedTime %s\n", FullName));
    return(GetAssocTime(FullName, Answer, lim));
}
