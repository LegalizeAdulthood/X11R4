/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include "class.h"
#include "enode.eh"

boolean enode__InitializeObject(c, self)
struct classheader *c;
struct enode *self;
{
    self->event = NULL;
    self->mychimp = NULL;
    return(TRUE);
}

void enode__SetEvent(self, event)
struct enode *self;
struct eventnode *event;
{
    self->event = event;
    enode_NotifyObservers(self, 0);
}

void enode__SetChimp(self, chimp)
struct enode *self;
struct chimp *chimp;
{
    self->mychimp = chimp;
}
