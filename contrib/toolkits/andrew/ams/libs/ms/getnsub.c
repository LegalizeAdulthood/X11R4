/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getnsub.c,v 2.3 89/02/08 15:33:35 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getnsub.c,v $
*/
MS_GetNextSubsEntry(FullName, NickName, status)
char *FullName; /* Value passed in to MS AND returned */
char *NickName; /* RETURN BUFFER returned from MS */
int *status; /* RETURN VALUE returned from MS */
{
    return(GetNextSubsEntry(FullName, NickName, status));
}
