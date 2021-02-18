/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/dfltdir.c,v 2.3 89/02/08 15:32:34 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/dfltdir.c,v $
*/
#include <ms.h>

FindDefaultDir(Root, Name)
char *Root, *Name;
{
    int NumGood, NumBad;
    struct MS_Directory *Dir;

    sprintf(Name, "%s/%s", Root, AMS_DEFAULTMAILDIR);
    mserrcode = ReadOrFindMSDir(Name, &Dir, MD_OK);
    if (mserrcode == 0) {
	return(0);
    }
    if (AMS_ERRNO == EMSBADDIRFORMAT) {
	NonfatalBizarreError("Your mail directory was corrupted and is being automatically reconstructed.  Please wait...");
	return(MS_ReconstructDirectory(Name, &NumGood, &NumBad, TRUE));
    }
    if (AMS_ERRNO != ENOENT) {
	return(mserrcode);
    }
    sprintf(Name, "%s/misc", Root);
    mserrcode = ReadOrFindMSDir(Name, &Dir, MD_OK);
    if (mserrcode == 0) {
	return(0);
    }
    if (AMS_ERRNO == EMSBADDIRFORMAT) {
	NonfatalBizarreError("Your misc directory was corrupted and is being automatically reconstructed.  Please wait...");
	return(MS_ReconstructDirectory(Name, &NumGood, &NumBad, TRUE));
    }
    if (AMS_ERRNO != ENOENT) {
	return(mserrcode);
    }
    /* Could try other things here eventually */
    return(mserrcode);
}
