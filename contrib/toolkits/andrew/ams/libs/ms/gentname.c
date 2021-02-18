/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/gentname.c,v 2.5 89/04/17 14:48:42 nsb Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/gentname.c,v $
*/
#include <stdio.h>
#include <mail.h>

extern char *getprofile();

MS_GenTempFileName(Buf)
char *Buf; /* Out */
{
    GenTempName(Buf);
    return(0);
}

GenTempName(Buf)
char   *Buf;
{
    static char *TempFileLocation = NULL;
    if (!TempFileLocation) {
	TempFileLocation = getprofile("AMStempfilelocation");
	if (!TempFileLocation) TempFileLocation = "/tmp";
    }
    sprintf(Buf, "%s/%s", TempFileLocation, ams_genid(1));
}
