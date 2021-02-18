/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getsnap.c,v 2.3 89/02/08 15:34:09 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getsnap.c,v $
*/
#include <ms.h>

MS_GetSnapshot(dirname, id, SnapshotBuf) 
char *dirname, *id; /* Passed in */
char *SnapshotBuf; /* Passed out */
{
    struct MS_Directory *Dir;
    int msgnum, errsave;

    debug(1, ("MS_GetSnapshot %s %s\n", dirname, id));
    if (ReadOrFindMSDir(dirname, &Dir, MD_READ) != 0) {
	return(mserrcode);
    }
    if (GetSnapshotByID(Dir, id, &msgnum, SnapshotBuf)) {
	errsave = mserrcode;
	CloseMSDir(Dir, MD_READ);
	return(errsave);
    }
    return(CloseMSDir(Dir, MD_READ));
}
