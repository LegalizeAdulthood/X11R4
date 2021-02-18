/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/crtdir.c,v 2.3 89/02/08 15:32:23 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/crtdir.c,v $
*/
#include <ms.h>

MS_CreateNewMessageDirectory(DirName, Overwrite, obsolete)
char *DirName;
int Overwrite;
char *obsolete;
{
    struct MS_Directory *Dir;

    debug(1, ("MS_CreateNewMessageDirectory %s %d\n", DirName, Overwrite));
    if (*DirName != '/') {
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_CREATENEWMESSAGEDIRECTORY);
    }
    if (obsolete && strcmp(DirName, obsolete)) {
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_CREATENEWMESSAGEDIRECTORY);
    }
    return(CreateNewMSDirectory(DirName, &Dir, Overwrite));
}
