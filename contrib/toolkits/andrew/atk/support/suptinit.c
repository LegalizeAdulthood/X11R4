/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/suptinit.c,v 2.4 89/02/17 17:15:06 ghoti Exp $ */
/* $ACIS:suptinit.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/suptinit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/support/RCS/suptinit.c,v 2.4 89/02/17 17:15:06 ghoti Exp $";
#endif /* lint */


#include <class.h>

#define class_StaticEntriesOnly

#include <buffer.ih>
#include <complete.ih>
#include <dict.ih>
#include <envrment.ih>
#include <mark.ih>
#include <nstdmark.ih>
#include <rectlist.ih>
#include <style.ih>
#include <stylesht.ih>
#include <tree23.ih>
#include <viewref.ih>

int support_Initialize()
{
    buffer_StaticEntry;
    completion_StaticEntry;
    dictionary_StaticEntry;
    environment_StaticEntry;
    mark_StaticEntry;
    nestedmark_StaticEntry;
    rectlist_StaticEntry;
    style_StaticEntry;
    stylesheet_StaticEntry;
    tree23int_StaticEntry;
    viewref_StaticEntry;

    return 1;
}
