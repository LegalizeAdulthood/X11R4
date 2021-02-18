/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/setasct.c,v 2.6 89/05/04 12:34:03 cfe Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/setasct.c,v $
*/
#include <ms.h>
#include <andyenv.h>
#include <sys/stat.h>

MS_SetAssociatedTime(FullName, newvalue)
char *FullName, *newvalue;
{
    struct MS_Directory *Dir;
    struct stat statbuf;

    debug(257, ("MS_SetAssociatedTime %s %s\n", FullName, newvalue));
    if (*FullName != '/' || (newvalue && (strlen(newvalue) >= AMS_DATESIZE))) {
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_SETASSOCIATEDTIME);
    }
    Dir = (struct MS_Directory *) FindInDirCache(FullName);
    if (newvalue == NULL || *newvalue == '\0') {
	newvalue = "0";
    }
#ifdef CMU_ENV
    if (strncmp(newvalue, "zzz", 3) != 0 && (*newvalue < '0' || *newvalue > '3')) {
	char ErrBuf[MAXPATHLEN+100];
	sprintf(ErrBuf, "BUG: setting time for folder %s to ``%s''", ap_Shorten(FullName), newvalue);
	NonfatalBizarreError(ErrBuf);
	if ((long) Dir == (long) -1 || (long) Dir == (long) NULL || strncmp(newvalue, Dir->LastMsgDate, AMS_DATESIZE) > 0) {	    
	    AMS_RETURN_ERRCODE(EFAULT, EIN_PARAMCHECK, EVIA_SETASSOCIATEDTIME);
	}
    }
#endif CMU_ENV
    if ((long) Dir != (long) -1 && (long) Dir != (long) NULL && (Dir->MessageCount <= 0 || strncmp(newvalue, Dir->LastMsgDate, AMS_DATESIZE) >= 0)) {
	debug(257, ("Last message or later, writing time stamp\n"));
	if (stat(FullName, &statbuf)) {
	    AMS_RETURN_ERRCODE(errno, EIN_STAT, EVIA_SETASSOCIATEDTIME);
	}
	return(SetProfileEntry(FullName, newvalue, statbuf.st_mtime));
    } else {
	debug(257, ("Was not the last message (Dir %d count %d date %s), not writing time stamp\n", Dir, Dir ? Dir->MessageCount : -1, Dir ? Dir->LastMsgDate : 0));
	return(SetProfileEntry(FullName, newvalue, 0));
    }
}
