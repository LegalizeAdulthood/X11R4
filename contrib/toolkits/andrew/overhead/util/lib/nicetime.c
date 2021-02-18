/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/nicetime.c,v 2.9 89/09/08 10:30:24 cfe Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/nicetime.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/nicetime.c,v 2.9 89/09/08 10:30:24 cfe Exp $";
#endif /* lint */

#include <stdio.h>
#include <andrewos.h>		/* sys/time.h */

char *NiceTime(Time)
long int Time;
{/* Like ctime, but do a more attractive job and don't end with a newline. */
    static char Res[50];
    static char *Mon[] = {	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    struct tm *This;

    This = localtime(&Time);
    osi_SetZone();
    sprintf(Res, "%d %s %04d at %d:%02d:%02d %s",
	     This->tm_mday,
	     Mon[This->tm_mon],
	     This->tm_year + 1900,
	     This->tm_hour,
	     This->tm_min,
	     This->tm_sec,
	     osi_ZoneNames[(osi_IsEverDaylight && This->tm_isdst ? 1 : 0)]);
    return Res;
}
