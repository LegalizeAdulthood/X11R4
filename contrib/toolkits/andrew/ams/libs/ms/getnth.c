/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getnth.c,v 2.3 89/02/08 15:33:38 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getnth.c,v $
*/
#include <ms.h>

MS_GetNthSnapshot(DirName, n, SnapshotBuf)
char *DirName, *SnapshotBuf;
int n;
{
    struct MS_Directory *Dir;
    long errsave;

    if (ReadOrFindMSDir(DirName, &Dir, MD_READ)) {
	return(mserrcode);
    }
    if (n == -1) {
	n = Dir->MessageCount - 1;
    }
    errsave = GetSnapshotByNumber(Dir, n, SnapshotBuf);
    CloseMSDir(Dir, MD_READ);
    return(errsave);
}
