/* 
 *		Copyright IBM Corporation 1989
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
 */

#if !defined(MENU_H)
#define MENU_H

#include "screen_items.h"

struct menu_area_tbl_entry
{
	AREA area;
	MENU *menu;
};

extern struct menu_area_tbl_entry menu_tbl[];
extern int menu_tbl_sz;
extern int max_num_menu_entries;

void set_currmitem( MENU_ITEM *menu_item);
extern void display_child_menu(MENU_ITEM *menu_item);
extern MENU * get_menu_from_area(AREA area);
extern AREA get_area_from_menu(MENU *menu);
MENU_ITEM * get_parent( MENU *child_menu);
void switch_sec_menu( MENU *new_menu);
void set_menu( AREA area, MENU *menu);
void find_menu_item_idx( KEY key, MENU **menu, IDX *idx);

#endif
