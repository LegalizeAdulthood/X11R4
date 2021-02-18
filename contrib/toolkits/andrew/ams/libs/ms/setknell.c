/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/setknell.c,v 2.6 89/08/31 15:45:01 aw0g Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/setknell.c,v $
*/
#include <mailconf.h>

int DeathKnell;

MS_SetDeathKnell(dk)
int dk;
{
    if(dk>AMS_MaximumDeathKnell)
     dk=AMS_MaximumDeathKnell;
    DeathKnell = dk;
    return(0);
}
