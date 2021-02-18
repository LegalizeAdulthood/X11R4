/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/examples/ex10/RCS/hello.c,v 2.5 89/09/13 11:41:00 tpn Exp $ */

#include <class.h>

#include "hello.eh"

boolean helloworld__InitializeObject(classID,hw)
struct classheader *classID;
struct helloworld *hw;   
{
    hw->x = POSUNDEF;
    hw->y = POSUNDEF;
    hw->blackOnWhite = TRUE;
    return TRUE;
}
