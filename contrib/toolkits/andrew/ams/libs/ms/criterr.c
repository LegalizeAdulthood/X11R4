/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/criterr.c,v 2.4 89/02/15 11:39:50 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/criterr.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/criterr.c,v 2.4 89/02/15 11:39:50 ghoti Exp $ ";
#endif /* lint */

#include <stdio.h>
#include <errprntf.h>

/* This is in a separate file to make it easy for a no-snap client to override it if it so desires. */

CriticalBizarreError(text)
char *text;
{
    BizarreError(text, ERR_CRITICAL);
}
