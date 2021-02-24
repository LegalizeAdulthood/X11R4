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
 */

#if !defined(ADJUST_GRID_H)
#define ADJUST_GRID_H

#include <xgks.h>

#include "screen_items.h"

extern void init_grid_color_popup(POPUP *popup);
extern void switch_grid_color_state(POPUP *popup, Gpoint pt);

extern void adjust_grid(MENU_ITEM *menu_item);
extern void init_grid_menu(AREA area, MENU *menu);

extern void move_grid_init(MENU_ITEM *menu_item);
extern void move_grid_restart(MENU_ITEM *menu_item);
extern void move_grid_exec(Gpoint pt);
extern void move_grid_cleanup(MENU_ITEM *menu_item);

extern void resize_grid_init(MENU_ITEM *menu_item);
extern void resize_grid_restart(MENU_ITEM *menu_item);
extern void resize_grid_exec(Gpoint pt);
extern void resize_grid_cleanup(MENU_ITEM *menu_item);

extern void adjust_grid_undo(MENU_ITEM *menu_item);

#endif
