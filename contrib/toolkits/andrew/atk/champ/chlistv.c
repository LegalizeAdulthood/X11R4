/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include "class.h"
#include "view.ih"
#include "chlist.ih"
#include "chlistv.eh"

boolean chlistview__InitializeObject(c, self)
struct classheader *c;
struct chlistview *self;
{
    return(TRUE);
}

struct view *chlistview__Hit(self, action, x, y, nclicks)
struct chlistview *self;
enum view_MouseAction action;
long x, y, nclicks;
{
    int pos, min, max, split;
    struct chlist *l;

    super_Hit(self, action, x, y, 3); /* always select whole line */
    l = (struct chlist *) chlistview_GetDataObject(self);
    pos = chlistview_GetDotPosition(self);
    min = 0;
    max = l->numitems - 1;
    while (max > min) {
	split = min + (max-min)/2;
	if (l->ItemList[split].loc > pos) {
	    max = split - 1;
	} else {
	    if (min == split) {
		if (l->ItemList[max].loc <= pos) {
		    min = max;
		}
		break;
	    }
	    min = split;
	}
    }
    if (min < l->numitems) {
	if (l->ItemList[min].proc) {
	    (l->ItemList[min].proc)(l->ItemList[min].rock, l, action, nclicks);
	}
    }
    return((struct view *) self);
}

void chlistview__ActivateItem(self, pos)
struct chlistview *self;
int pos;
{
    int i, len;
    struct chlist *l;

    l = (struct chlist *) chlistview_GetDataObject(self);
    for (i=0; i < l->numitems && l->ItemList[i].loc < pos; ++i) {
	;
    }
    if (i<l->numitems) {
	len = l->ItemList[i].loc - l->ItemList[i-1].loc;
    } else {
	len = chlist_GetLength(l) - l->ItemList[i-1].loc;
    }
    --i;
    chlistview_SetDotPosition(self, l->ItemList[i].loc);
    chlistview_SetDotLength(self, len);
    if (l->ItemList[i].proc) {
	(l->ItemList[i].proc)(l->ItemList[i].rock, l, view_LeftDown, 3);
    }
    chlistview_WantInputFocus(self, self);
}
