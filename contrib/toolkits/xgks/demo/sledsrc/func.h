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
#if !defined(FUNC_H)
#define FUNC_H

#include "add_obj_func.h"
#include "adjust_grid.h"
#include "clr_sel_func.h"
#include "edit_picture.h"
#include "photo_func.h"

/* screen area functions */

extern void menu_area_exec(AREA area, IDX transno, Gpoint pt);
extern void palette_area_exec(AREA area, IDX transno, Gpoint pt);
extern void picture_area_exec(AREA area, IDX transno, Gpoint pt);
extern void fgdclr_area_exec(AREA area, IDX transno, Gpoint pt);
extern void bgdclr_area_exec(AREA area, IDX transno, Gpoint pt);
extern void fillclr_area_exec(AREA area, IDX transno, Gpoint pt);
extern void popup_area_exec(AREA area, IDX transno, Gpoint pt);

extern void display_pri_menu(void);
extern void display_sec_menu(void);
extern void display_palette(void);
extern void display_picture_area(void);
extern void display_fgdclr_area(void);
extern void display_bgdclr_area(void);
extern void display_fillclr_area(void);
extern void display_msg_area(void);

/* general menu functions */

extern void display_menu_n_popups(AREA area, MENU *menu);
extern void cleanup_menu_n_popups(AREA area, MENU *menu);
extern void cleanup_menu_n_garbage(AREA area, MENU *menu);

extern void area_undo(MENU_ITEM *item);
extern int menu_item_undo(void);

/* main menu functions */

extern void not_implemented(MENU_ITEM *mitem_in_progress);
extern void quit(MENU_ITEM *item);
extern void save(MENU_ITEM *item);
extern void retrieve(MENU_ITEM *item);
extern void up(MENU_ITEM *menu_item);
extern void update(MENU_ITEM *menu_item);
extern void create_pic_file(MENU_ITEM *item);

/* popup functions */

extern void init_update_popup(POPUP *popup);
extern void switch_update_state(POPUP *popup, Gpoint pt);
extern void init_grid_popup(POPUP *popup);
extern void switch_grid_state(POPUP *popup, Gpoint pt);
extern void init_verbose_popup(POPUP *popup);
extern void switch_verbose_state(POPUP *popup, Gpoint pt);
extern void erase_popup(POPUP *popup);

#endif
