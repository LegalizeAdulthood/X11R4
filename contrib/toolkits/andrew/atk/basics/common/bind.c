/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/bind.c,v 2.4 89/02/17 18:35:24 ghoti Exp $ */
/* $ACIS:bind.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/bind.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/bind.c,v 2.4 89/02/17 18:35:24 ghoti Exp $";
#endif /* lint */

/* bind.c
 */

/*  */

#include <class.h>

#include <bind.eh>

#include <keymap.ih>
#include <menulist.ih>
#include <proctbl.ih>

void bind__BindList(classID,bl,km,ml,type)
struct classheader *classID;
struct bind_Description *bl;
struct keymap *km;
struct menulist *ml;
struct classinfo *type;
{
    while(bl->procName!=NULL || bl->keyVector!=NULL || bl->menuEntry!=NULL){
	struct proctable_Entry *pe;

	if(bl->procName!=NULL)
	    pe=proctable_DefineProc(bl->procName, bl->proc,type,bl->module,bl->doc);
	else
	    pe=NULL;

	if(km!=NULL && bl->keyVector!=NULL)
	    keymap_BindToKey(km,bl->keyVector,pe,bl->keyRock);
	if(ml!=NULL && bl->menuEntry!=NULL)
	    menulist_AddToML(ml,bl->menuEntry,pe,bl->menuRock,bl->menuMask);

	bl++;
    }
}
