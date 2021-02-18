/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/shrkdate.c,v 2.4 89/02/10 09:15:23 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/shrkdate.c,v $
*/
#include <ms.h>
#include <andrewos.h> /* sys/time.h */


char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
	"Sep", "Oct", "Nov", "Dec", 0};


ParseAndShrinkDate(bigdate, littledate) 
char *bigdate, *littledate;
{
    struct tm TmBuf;

    debug(1, ("ParseAndShrinkDate %s\n", bigdate));
    if (parsedateheader(bigdate, &TmBuf, 1, 1, 1, 0)) {
	strcpy(littledate, "Bad Date");
	return(-1);
    } else {
	sprintf(littledate, "%2d-%s-%02d", TmBuf.tm_mday,
	    months[TmBuf.tm_mon], TmBuf.tm_year);
	return(0);
    }
}

