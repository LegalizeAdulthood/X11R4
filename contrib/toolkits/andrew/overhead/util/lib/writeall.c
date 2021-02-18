/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	writeall.c -- Do write, resuming if interrupted
*/
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/writeall.c,v 2.3 89/02/10 23:29:15 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/writeall.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/writeall.c,v 2.3 89/02/10 23:29:15 ghoti Exp $";
#endif /* lint */

extern int errno;

int writeall(fd, Buf, NBytes)
int fd;
char *Buf;
int NBytes;
{
    int Code, ToWrite;

    Code = 0;
    ToWrite = NBytes;
    errno = 0;
    while (ToWrite > 0) {
	Code = write(fd, Buf, ToWrite);
	if (Code < 0) return(Code);
	if (Code == ToWrite) return(NBytes);
	if (Code > ToWrite || errno != 0) return(Code + NBytes - ToWrite);
	ToWrite -= Code;
	Buf += Code;
    }
    return(Code);
}
