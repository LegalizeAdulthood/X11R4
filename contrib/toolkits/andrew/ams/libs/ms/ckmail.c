/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/ckmail.c,v 2.4 89/02/08 15:31:55 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/ckmail.c,v $
*/
#include <ms.h>
#include <sys/dir.h>

extern char home[], *GetPersonalMailbox();

MS_DoIHaveMail(count)
int *count;
{
    DIR *dirp;
    struct direct *dirent;
    char *Mailbox;

    *count = 0;
    Mailbox = GetPersonalMailbox();
    if ((dirp = opendir(Mailbox)) == NULL) {
	AMS_RETURN_ERRCODE(errno, EIN_OPENDIR, EVIA_DOIHAVEMAIL);
    }
    while ((dirent = readdir(dirp)) != NULL) {
	if (dirent->d_name[0] != '.') {
	    ++*count;
	}
    }
    closedir(dirp);
    return(0);
}
