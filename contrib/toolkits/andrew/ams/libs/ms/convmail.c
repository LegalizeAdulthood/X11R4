/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/convmail.c,v 2.4 89/02/08 15:32:28 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/convmail.c,v $
*/
#include <ms.h>
#include <sys/dir.h>

extern char home[];

MS_ConvertOldMail(good, bad)
int *good, *bad;
{
    char OldName[1+MAXPATHLEN], NewName[1+MAXPATHLEN], DirName[1+MAXPATHLEN], NewDirName[1+MAXPATHLEN], ErrorText[256];
    DIR *dirp;
    int mybad = 0;
    struct direct *dirent;

    debug(1, ("MS_ConvertOldMail"));
    if (EnsureMailDirExists()) {
	return(mserrcode);
    }
    *good = *bad = 0;
    sprintf(DirName, "%s/Maillib", home);
    sprintf(NewDirName, "%s/.MESSAGES/%s", home, AMS_DEFAULTMAILDIR);
    mserrcode = MS_SetSubscriptionEntry(NewDirName, AMS_DEFAULTMAILDIR, AMS_ALWAYSSUBSCRIBED);
    if (mserrcode) {
	return(mserrcode);
    }
    if ((dirp = opendir(DirName)) == NULL) {
	if (errno == ENOENT) {
	    return(0); /* Nothing to convert, that's fine */
	}
	AMS_RETURN_ERRCODE(errno, EIN_OPENDIR, EVIA_CONVERTOLD);
    }
    while ((dirent = readdir(dirp)) != NULL) {
	if (!strcmp(dirent->d_name, ".")) continue;
	if (!strcmp(dirent->d_name, "..")) continue;
	sprintf(OldName, "%s/%s", DirName, dirent->d_name);
	sprintf(NewName, "%s/.MESSAGES/%s/%s", home, AMS_DEFAULTMAILDIR, dirent->d_name);
	if (!strcmp(dirent->d_name, "view.grt") || !strcmp(dirent->d_name, "messages.grt")) {
	    if (unlink(OldName)) {
		sprintf(ErrorText, "Could not unlink file %s", ap_Shorten(dirent->d_name));
		NonfatalBizarreError(ErrorText);
	    }
	} else {
	    if (RenameEvenInVice(OldName, NewName)) {
		++mybad;
		sprintf(ErrorText, "Could not convert old mail file %s", ap_Shorten(dirent->d_name));
		NonfatalBizarreError(ErrorText);
	    }
	}
    }
    closedir(dirp);
    if (rmdir(DirName)) {
	sprintf(ErrorText, "Could not remove old Maillib directory (%d)", errno);
	NonfatalBizarreError(ErrorText);
    }
    mserrcode = MS_ReconstructDirectory(NewDirName, good, bad, 1);
    *bad += mybad;
    return(mserrcode);
}
