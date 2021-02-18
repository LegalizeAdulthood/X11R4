/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/freedir.c,v 2.3 89/02/08 15:33:19 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/freedir.c,v $
*/
#include <ms.h>

/* This routine frees a directory and the things it points to.  If the
	directory is in the cache, it should be removed from the cache
	BEFORE this routine is called, or you'll regret it. */

/* This routine is not currently used, because nobody ever frees a directory
any more -- they are in a permanent cache, and freeing them will cause a
core dump.  However, this documents what they allocate, and might be useful
in future versions. */

FreeDirectory(Dir)
struct MS_Directory *Dir;
{
    debug(1, ("FreeDirectory\n"));
    if (Dir) {
	int i;

	if (Dir->UNIXDir) free (Dir->UNIXDir);
	if (Dir->AttrCount && Dir->AttrNames) {
	    for (i=0; i<Dir->AttrCount; ++i) {
		free(Dir->AttrNames[i]);
	    }
	    free(Dir->AttrNames);
	}
	free(Dir);
    }
}
