/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/substr.c,v 2.3 89/02/08 15:46:04 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/substr.c,v $
*/
#include <ms.h>

char   *
        FindSubString (sub, full)
char   *sub,
       *full;
{
    int     slen,
            tries;

    debug(1,("FindSubString %s %s\n", sub, full));
    slen = strlen(sub);
    tries = strlen(full) - slen;
    while (tries-- >= 0) {
	if (*full == *sub && !strncmp(full, sub, slen))
	    return(full);
	++full;
    }
    return(NULL);
}
