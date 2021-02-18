/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getbody.c,v 2.4 89/04/16 11:30:08 nsb Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getbody.c,v $
*/
#include <ms.h>

MS_GetPartialBody(DirName, id, Buf, BufLim, offset, remaining, ct)
char *DirName, *id, *Buf;
int BufLim, offset, *remaining, *ct;
{
    struct MS_Directory *Dir;
    char FileName[MAXPATHLEN+1];

    debug(1, ("MS_GetPartialBody %s %s\n", DirName, id));
    *ct = *remaining = 0;
    if (ReadOrFindMSDir(DirName, &Dir, MD_OK) != 0) {
	return(mserrcode);
    }
    GetBodyFileName(Dir->UNIXDir, id, FileName);
    if (!offset) {
	ConsiderLoggingRead(FileName);
    }
    return(MS_GetPartialFile(FileName, Buf, BufLim, offset, remaining, ct));
}
