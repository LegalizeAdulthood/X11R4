/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	fwrtallc.c
	fwriteallchars() -- Do fwrite, resuming if interrupted
*/

/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fwrtallc.c,v 2.4 89/02/10 23:25:44 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fwrtallc.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fwrtallc.c,v 2.4 89/02/10 23:25:44 ghoti Exp $";
#endif /* lint */

#include <stdio.h>

extern int errno;

int fwriteallchars(Thing, NItems, stream)
char *Thing;
int NItems;
FILE *stream;
{
    int Code, ToWrite;

    Code = 0;
    ToWrite = NItems;
    errno = 0;
    while (ToWrite > 0) {
	Code = fwrite(Thing, sizeof(char), ToWrite, stream);
	if (Code < 0) return(Code);
	if (Code == 0 && (errno != 0 || ferror(stream) || feof(stream)))
	    return(Code);
	if (Code == ToWrite) return(NItems);
	if (Code > ToWrite || errno != 0 || ferror(stream) || feof(stream))
	    return(Code + NItems - ToWrite);
	ToWrite -= Code;
	Thing += Code;
    }
    return(Code);
}
