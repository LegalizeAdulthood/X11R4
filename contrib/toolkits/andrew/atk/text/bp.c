/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bp.c,v 1.2 89/06/08 10:15:14 tpn Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bp.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bp.c,v 1.2 89/06/08 10:15:14 tpn Exp $";
#endif /* lint */
#include <class.h>
#include <bp.eh>
char *bp__ViewName(self)
struct bp *self;
{
    return "bpv";
}
