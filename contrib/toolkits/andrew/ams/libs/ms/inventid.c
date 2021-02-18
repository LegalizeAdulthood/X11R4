/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/inventid.c,v 2.4 89/08/02 09:49:18 cfe Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/inventid.c,v $
*/
#include <ms.h>

InventID(msg)
struct MS_Message *msg;
{
    debug(1, ("Invent ID\n"));

    strcpy(AMS_ID(msg->Snapshot), ams_genid(1));
    return(0);
}
