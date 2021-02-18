/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/mvinvice.c,v 2.3 89/02/10 09:14:12 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/mvinvice.c,v $
*/
#include <errno.h>
#include <andrewos.h> /* sys/file.h */

extern int errno;

RenameEvenInVice(ThisFileName, NewFileName)
char   *ThisFileName,
       *NewFileName;
{
    int     ReallyBad = 5;

    while (1) {
	if (!rename(ThisFileName, NewFileName)) {
	    return(0);
	}
	switch (errno) {
	    case EINTR:
		/* Interrupted system call -- try again */
		if (!ReallyBad--) return(-1);
		break;
	    case ENOENT:
		/* May be a vice timing error -- rename might */
		/* have actually worked! */
		if (!access(NewFileName, F_OK)) {
		    return(0);
		}
		errno = ENOENT;
		return(-1);
	    default:
		return(-1);
	}
    }
}
