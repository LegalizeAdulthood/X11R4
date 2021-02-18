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

#include "clr_sel_func.h"
#include "add_obj_func.h"
#include "edit_picture.h"
#include "adjust_grid.h"
#include "photo_func.h"

/* screen area functions */

extern int menu_area_exec();
extern int palette_area_exec();
extern int picture_area_exec();
extern int fgdclr_area_exec();
extern int bgdclr_area_exec();
extern int fillclr_area_exec();
extern int popup_area_exec();

extern int display_pri_menu();
extern int display_sec_menu();
extern int display_palette();
extern int display_picture_area();
extern int display_popup_area();
extern int display_fgdclr_area();
extern int display_bgdclr_area();
extern int display_fillclr_area();
extern int display_msg_area();

/* general menu functions */

extern int display_menu_n_popups();
extern int cleanup_menu_n_popups();
extern int cleanup_menu_n_garbage();
extern int display_child_menu();

extern int area_undo();
extern int menu_item_undo();

/* main menu functions */

extern int not_implemented();
extern int quit();
extern int save();
extern int retrieve();
extern int up();
extern int update();
extern int create_pic_file();

/* popup functions */

extern int init_update_popup();
extern int switch_update_state();
extern int init_grid_popup();
extern int switch_grid_state();
extern int init_verbose_popup();
extern int switch_verbose_state();
extern int erase_popup();
