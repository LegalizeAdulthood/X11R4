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
 *      find_screen_tbl_idx
 *      find_popup_tbl_idx
 *      find_menu_item_tbl_idx
 *      find_menu_tbl_idx
 */

#include <strings.h>

#include "defs.h"
#include "functbl.h"
#include "screen_items.h"

/*
 *  find_screen_tbl_idx
 *
 *  description:        returns ptr to screen tbl entry matching
 *                      area
 *
 *  parameters:         area (AREA) - screen area
 */

int find_screen_tbl_idx(AREA area)
{
    int i;

    /* look for area in tbl */

    for (i = 0; (!eq(screen_tbl[i].area, area) && i < screen_tbl_sz);
         i++)
        ;

    /* if found return index, else return -1 */

    if (eq(screen_tbl[i].area, area))
        return (i);
    else
        return (-1);

}; /* end find_screen_tbl_idx */

/*
 *  find_popup_func_tbl_idx
 *
 *  description:        returns index in popup_func_tbl of
 *                      routines corresponding to popup
 *                      uniquely identified by key
 *
 *  parameters:         key (KEY) - unique popup key
 *
 *  returns:            idx (IDX) - index of key in popup_func_tbl
 */

int find_popup_func_tbl_idx(KEY key)
{
    IDX idx;

    for (idx = 0; (!eq(popup_func_tbl[idx].key, key) && (idx < popup_func_tbl_sz)); idx++)
        ;

    if (eq(popup_func_tbl[idx].key, key))
        return (idx);
    else
        return (-1);
} /* find_popup_func_tbl_idx */

/*
 *  find_menu_item_func_tbl_idx
 *
 *  description:        returns index in menu_item_func_tbl of
 *                      routines corresponding to menu_item
 *                      uniquely identified by key
 *
 *  parameters:         key (KEY) - unique menu_item key
 *
 *  returns:            idx (IDX) - index of key in menu_item_func_tbl
 */

int find_menu_item_func_tbl_idx(KEY key)
{
    IDX idx;

    for (idx = 0; (!eq(menu_item_func_tbl[idx].key, key) && (idx < menu_item_func_tbl_sz)); idx++)
        ;

    if (eq(menu_item_func_tbl[idx].key, key))
        return (idx);
    else
        return (-1);
} /* find_menu_item_func_tbl_idx */

/*
 *  find_menu_func_tbl_idx
 *
 *  description:        returns index in menu_func_tbl of
 *                      routines corresponding to menu
 *                      uniquely identified by key
 *
 *  parameters:         key (KEY) - unique menu key
 *
 *  returns:            idx (IDX) - index of key in menu_func_tbl
 */

int find_menu_func_tbl_idx(KEY key)
{
    IDX idx;

    for (idx = 0; (!eq(menu_func_tbl[idx].key, key) && (idx < menu_func_tbl_sz)); idx++)
        ;

    if (eq(menu_func_tbl[idx].key, key))
        return (idx);
    else
        return (-1);
} /* find_menu_func_tbl_idx */
