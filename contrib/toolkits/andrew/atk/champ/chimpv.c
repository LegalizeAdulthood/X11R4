/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include "class.h"
#include "scroll.ih"
#include "chlistv.ih"
#include "enode.ih"
#include "enodev.ih"
#include "chimp.ih"
#include "chimpv.eh"

boolean chimpview__InitializeObject(c, self)
struct classheader *c;
struct chimpview *self;
{
    struct scroll *s = scroll_New();

    self->env = enodeview_New();
    self->lv = chlistview_New();
    if (!self->lv || !s || !self->env) return(FALSE);
    enodeview_SetChimpview(self->env, self);
    scroll_SetView(s, self->lv);
    chimpview_SetUp(self, s, self->env, 50, lpair_PERCENTAGE, lpair_VERTICAL, TRUE);
    return(TRUE);
}

void chimpview__SetDataObject(self, c)
struct chimpview *self;
struct chimp *c;
{
    chlistview_SetDataObject(self->lv, c);
    enodeview_SetDataObject(self->env, c->en);
}
