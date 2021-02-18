/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/shr/RCS/findroot.c,v 2.4 89/02/17 19:00:34 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/shr/RCS/findroot.c,v $
*/
#include <andrewos.h>
#include <ams.h>

extern char *index();

FindTreeRoot(DirName, RootName, ReallyWantParent)
char *DirName, *RootName;
short ReallyWantParent;
{
    char *s, *t;

    strcpy(RootName, DirName);
    for (s = RootName; *s; ++s) {
	if (*s == '/' && !strncmp(s+1, MS_TREEROOT, sizeof(MS_TREEROOT) -1)) {
	    break;
	}
    }
    if (!*s) {
	return(-1);
    }
    if (ReallyWantParent) {
	*s = '\0';
    } else {
	t = index(++s, '/');
	if (t) *t = '\0';
    }
    return(0);
}
