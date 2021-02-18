/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/winit.c,v 1.4 89/02/17 18:19:15 ghoti Exp $ */
/* $ACIS:init.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/winit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/winit.c,v 1.4 89/02/17 18:19:15 ghoti Exp $";
#endif /* lint */

#include <class.h>
#define class_StaticEntriesOnly

#include <wfontd.ih>
#include <wgraphic.ih>
#include <wim.ih>
#include <wcursor.ih>
#include <mrl.ih>
#include <wws.ih>

int wmbasicsinit ()
{
    wmfontdesc_StaticEntry;
    wmgraphic_StaticEntry;
    wmim_StaticEntry;
    wmcursor_StaticEntry;
    mrl_StaticEntry;
    wmws_StaticEntry;
    return 1;
}
