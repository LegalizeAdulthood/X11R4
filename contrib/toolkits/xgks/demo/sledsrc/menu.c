/*
 *              Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *  this file contains:
 *      get_menu_from_area
 *      get_area_from_menu
 *      find_menu_item_idx
 *      set_menu
 *      get_parent
 *      switch_sec_menu
 *      display_child_menu
 *      not_implemented
 *      set_currmitem
 */

#include <xgks.h>

#include <stdlib.h>

#include "defs.h"
#include "error.h"
#include "functbl.h"
#include "key.h"
#include "menu.h"
#include "prompt.h"
#include "screen_items.h"
#include "undo.h"

int menu_tbl_sz = 2;

struct menu_area_tbl_entry menu_tbl[2] = {
    {
        PRI_MENU_AREA, /* area */
        NULL           /* menu */
    },

    {
        SEC_MENU_AREA, /* area */
        NULL           /* menu */
    }
};

/*
 *  get_menu_from_area
 *
 *  description:        finds menu in area and returns ptr
 *                      to area
 *
 *  parameters:         area (AREA) - area containing menu
 */

MENU *get_menu_from_area(AREA area)
{
    int i;

    for (i = 0; (i < menu_tbl_sz && !eq(menu_tbl[i].area, area)); i++)
        ;

    if (eq(menu_tbl[i].area, area))
        return (menu_tbl[i].menu);
    else
        return ((MENU *) NULL);
}; /* end get_menu_from_area */

/*
 *  get_area_from_menu
 *
 *  description:        finds area containing menu and returns area
 *
 *  parameters:         menu (MENU) * - menu containing area
 */

AREA get_area_from_menu(MENU *menu)
{
    AREA area;
    int i;

    for (i = 0; (i < menu_tbl_sz && !eq(menu_tbl[i].menu->key, menu->key)); i++)
        ;

    if (eq(menu_tbl[i].menu->key, menu->key))
    {
        area = (AREA) calloc((unsigned)
                                 strlen(menu_tbl[i].area),
            sizeof(char));
        (void) strcpy(area, menu_tbl[i].area);
        return (menu_tbl[i].area);
    }
    else
        return ((AREA) NULL);
} /* end get_menu_from_area */

/*
 *  find_menu_item_idx
 *
 *  description:        finds menu containing item and idx 
 *                      of item in menu
 *
 *  parameters:         key (KEY) - menu item key
 *                      menu (MENU **) - menu containing item
 *                      idx (IDX *) - menu index
 *
 *  returns:            menu and menu item index in menu
 */

void find_menu_item_idx(KEY key, MENU **menu, IDX *idx)
{
    BOOLEAN found;
    int i = 1;
    int j = 1;
    MENU_ITEM *menu_item_ptr;

    /* determine which menu contains menu_item */
    /* i will be the one larger than the index
           of menu in menu_tbl */
    /* j will be the index of menu item in menu */

    found = FALSE;
    for (i = 0; ((i < menu_tbl_sz) && (!found)); i++)
    {
        menu_item_ptr = menu_tbl[i].menu->item;
        for (j = 0; ((j < menu_tbl[i].menu->noitems) && (!found));
             j++)
        {
            if (eq(menu_item_ptr->key, key))
                found = TRUE;
            else
                menu_item_ptr++;
        }
    }

    if (found)
    {
        *idx = j - 1;
        *menu = menu_tbl[i - 1].menu;
    }
    else
        exit_error("find_menu_item_idx", 10);
} /* end find_menu_item_idx */

/*
 *  set_menu
 *
 *  description:        sets tbl entry corresponding to area
 *                      to menu
 *
 *  parameters:         area (AREA) - tbl key
 *                      menu (MENU) - menu to enter
 */

void set_menu(AREA area, MENU *menu)
{
    int i;

    for (i = 0; (i < menu_tbl_sz && !eq(menu_tbl[i].area, area)); i++)
        ;

    if (eq(menu_tbl[i].area, area))
        menu_tbl[i].menu = menu;
    else
        exit_error("set_menu", 8);
} /* end set_menu */

/*
 *  get_parent
 *
 *  description:        finds parent menu item of child menu
 *
 *  parameters:         child_menu (MENU *) - child menu
 *
 *  returns:            (MENU_ITEM *) parent menu item of child menu
 */

MENU_ITEM *get_parent(MENU *child_menu)
{
    int i = 0;

    if (child_menu->parent != (MENU *) NULL)
        for (i = 0; child_menu->parent->item[i].child != child_menu; i++)
            ;

    if (child_menu->parent != (MENU *) NULL)
        return (&(child_menu->parent->item[i]));
    else
        return ((MENU_ITEM *) NULL);
}; /* get_parent */

/*
 *  switch_sec_menu
 *
 *  description:        switches secondary menu to new menu
 *
 *  parameters:         new_menu (MENU *) - new secondary menu
 */

void switch_sec_menu(MENU *new_menu)
{
    IDX idx;
    MENU *sec_menu;

    sec_menu = get_menu_from_area(SEC_MENU_AREA);
    if (sec_menu != (MENU *) NULL)
    {
        idx = find_menu_func_tbl_idx(sec_menu->key);
        (*(menu_func_tbl[idx].cleanup))(SEC_MENU_AREA, sec_menu);
    }

    set_menu(SEC_MENU_AREA, new_menu);
    sec_menu = new_menu;
    currmitem = (MENU_ITEM *) NULL;

    idx = find_menu_func_tbl_idx(sec_menu->key);
    (*(menu_func_tbl[idx].init))(SEC_MENU_AREA, sec_menu);

} /* end switch_sec_menu */

void display_child_menu(MENU_ITEM *menu_item)
{
    switch_sec_menu(menu_item->child);
    initialize_undo();
    prompt(2);

} /* end display_child_menu */

/*
 *  not_implemented
 *
 *  description:        tells user func has not been implemented
 *                      and reprompts user
 *
 *  parameters:         menu_item (MENU_ITEM *) - menu item picked
 */

void not_implemented(MENU_ITEM *menu_item)
{
    reprompt(0);
    currmitem = (MENU_ITEM *) NULL;

} /* end not_implemented */

/*
 *  set_currmitem
 *
 *  description:        sets currmitem to specified menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - new value for
 *                              currmitem
 */

void set_currmitem(MENU_ITEM *menu_item)
{
    currmitem = menu_item;
} /* end set_currmitem */
