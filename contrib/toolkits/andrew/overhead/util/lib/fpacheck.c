/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fpacheck.c,v 2.3 89/02/10 23:25:36 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/fpacheck.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

fpacheck()
{
    float f;

    f = 1000.0 * 0.5;
    f /= 2;
    f /= 2.0;
    return ((f == 125) ? 0 : 1);
}
