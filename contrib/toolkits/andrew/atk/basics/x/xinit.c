/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xinit.c,v 1.1 89/09/08 13:43:03 wjh Exp $ */
/* $ACIS:init.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xinit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xinit.c,v 1.1 89/09/08 13:43:03 wjh Exp $";
#endif /* lint */

#include <class.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <cmenu.h>

#define class_StaticEntriesOnly

#include <xfontd.ih>
#include <xgraphic.ih>
#include <xim.ih>
#include <xcursor.ih>
#include <xws.ih>

int xbasicsinit ()
{
    xfontdesc_StaticEntry;
    xgraphic_StaticEntry;
    xim_StaticEntry;
    xcursor_StaticEntry;
    xws_StaticEntry;
    return 1;
}
