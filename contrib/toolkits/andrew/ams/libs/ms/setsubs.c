/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/setsubs.c,v 2.3 89/02/08 15:45:43 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/setsubs.c,v $
*/
#include <ms.h>

extern char home[];

MS_SetSubscriptionEntry(FullName, NickName, status)
char *FullName; /* Value passed in to MS */
char *NickName; /* Value passed in to MS */
int status; /* Ditto */
{
    struct MS_Directory *Dir;

    if (status == AMS_UNSUBSCRIBED) {
	char DefaultMailDir[1+MAXPATHLEN],
	     MailRoot[1+MAXPATHLEN];

	sprintf(MailRoot, "%s/%s", home, MS_TREEROOT);
	if (FindDefaultDir(MailRoot, DefaultMailDir)) {
	    if (AMS_ERRNO != ENOENT) {
		return(mserrcode);
	    }
	    DefaultMailDir[0] = '\0';
	}
	if (!strcmp(DefaultMailDir, FullName)) {
	    AMS_RETURN_ERRCODE(EMSFASCISTSUBSCRIPTION, EIN_PARAMCHECK, EVIA_SETSUBSENTRY);
	}
    } else if (ReadOrFindMSDir(FullName, &Dir, MD_OK)) {
	return(mserrcode);
    }
    return(SetSubsEntry(FullName, NickName, status));
}
