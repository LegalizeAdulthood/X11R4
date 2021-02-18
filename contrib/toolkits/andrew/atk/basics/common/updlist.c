/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/updlist.c,v 2.5 89/02/17 18:40:40 ghoti Exp $ */
/* $ACIS:updlist.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/updlist.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/updlist.c,v 2.5 89/02/17 18:40:40 ghoti Exp $";
#endif /* lint */

#include <class.h>
#include <updlist.eh>
#include <view.ih>

static struct updateitem *freeList = NULL;

boolean updatelist__InitializeObject(classID, self)
    struct classheader *classID;
    struct updatelist *self;
{
    self->updates = NULL;
    return TRUE;
}

void updatelist__FinalizeObject(classID, self)
    struct classheader *classID;
    struct updatelist *self;
{

    struct updateitem *item;

    if (self->updates != NULL) {
        for (item = self->updates; item->next != NULL; item = item->next)
            ;
        item->next = freeList;
        freeList = self->updates;
    }
}

void updatelist__AddTo(self, view)
    struct updatelist *self;
    struct view *view;
{

    register struct updateitem *newui;

    if (freeList != NULL)  {
	newui = freeList;
	freeList = freeList->next;
    }
    else  {
	newui = (struct updateitem *) malloc(sizeof(struct updateitem));
    }
    
    newui->view = view;
    newui->next = self->updates;
    self->updates = newui;
}

void updatelist__DeleteTree(self, tree)
    struct updatelist *self;
    struct view *tree;
{
    struct updateitem *item;
    struct updateitem *next;
    struct updateitem **previous;

    previous = &self->updates;
    for (item = self->updates; item != NULL; item = next) {
        next = item->next;
	if (view_IsAncestor(item->view, tree)) {
            *previous = item->next;
            item->next = freeList;
            freeList = item;
        }
        else
            previous = &item->next;
    }
}


void updatelist__Clear(self)
    struct updatelist *self;
{
    struct updateitem *item;
    struct updateitem *lastitem = NULL;
    struct updateitem *pendingUpdates;

    pendingUpdates = self->updates;
    self->updates = NULL;
    for (item = pendingUpdates; item != NULL ; item = item->next) {
        view_Update(item->view);
        lastitem = item;
    }
    if (lastitem != NULL) {
        lastitem->next = freeList;
        freeList = pendingUpdates;
    }
}
