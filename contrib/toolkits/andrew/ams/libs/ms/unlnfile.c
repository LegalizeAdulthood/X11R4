/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/unlnfile.c,v 2.3 89/02/08 15:46:07 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/unlnfile.c,v $
*/
#include <ms.h>

long    MS_UnlinkFile (FileName)
char   *FileName;
{
    debug(1, ("Entering MS_UnlinkFile %s\n", FileName));
    if (unlink(FileName)) {
	AMS_RETURN_ERRCODE(errno, EIN_UNLINK, EVIA_UNLINKFILE);
    }
    return(0);
}
