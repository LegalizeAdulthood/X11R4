/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getsubs.c,v 2.2 89/02/08 15:34:12 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getsubs.c,v $
*/
MS_GetSubscriptionEntry(FullName, NickName, status)
char *FullName; /* Value passed in to MS */
char *NickName; /* RETURN BUFFER returned from MS */
int *status; /* RETURN VALUE returned from MS */
{
    return(GetSubsEntry(FullName, NickName, status));
}

