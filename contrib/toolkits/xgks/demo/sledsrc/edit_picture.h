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

#if !defined(EDIT_PICTURE_H)
#define EDIT_PICTURE_H

#include "screen_items.h"

extern void edit_picture_init(MENU_ITEM *item);

/* menu item functions */

extern void edit_picture_undo(MENU_ITEM *item);
extern void erase(MENU_ITEM *item);

extern void rotate_init(MENU_ITEM *item);
extern void rotate_restart(MENU_ITEM *item);
extern void rotate_exec(Gpoint pt);
extern void rotate_cleanup(MENU_ITEM *item);

extern void move_init(MENU_ITEM *item);
extern void move_restart(MENU_ITEM *item);
extern void move_exec(Gpoint pt);
extern void move_cleanup(MENU_ITEM *item);

extern void copy_init(MENU_ITEM *item);
extern void copy_restart(MENU_ITEM *item);
extern void copy_exec(Gpoint pt);
extern void copy_cleanup(MENU_ITEM *item);

extern void delete_init(MENU_ITEM *item);
extern void delete_restart(MENU_ITEM *item);
extern void delete_exec(Gpoint pt);
extern void delete_cleanup(MENU_ITEM *item);

extern void combine_init(MENU_ITEM *item);
extern void combine_restart(MENU_ITEM *item);
extern void combine_exec(Gpoint pt);
extern void combine_cleanup(MENU_ITEM *item);

extern void split_init(MENU_ITEM *item);
extern void split_restart(MENU_ITEM *item);
extern void split_exec(Gpoint pt);
extern void split_cleanup(MENU_ITEM *item);

/* popup functions */

extern void init_angle_popup(POPUP *popup);
extern void switch_angle_state(POPUP *popup, Gpoint pt);

#endif
