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
 * menu items
 */

#include <xgks.h>

#include "changes.h"
#include "objects.h"
#include "screen_items.h"

void set_cbreak_mode(int state);

extern void add_objects_init(MENU_ITEM *menu_item);

extern void add_line_init(MENU_ITEM *menu_item);
extern void add_line_restart(MENU_ITEM *menu_item);
extern void add_line_exec(Gpoint pt);
extern void add_line_cleanup(MENU_ITEM *menu_item);

extern void add_curve_init(MENU_ITEM *menu_item);
extern void add_curve_restart(MENU_ITEM *menu_item);
extern void add_curve_exec(Gpoint pt);
extern void add_curve_cleanup(MENU_ITEM *menu_item);

extern void add_box_init(MENU_ITEM *menu_item);
extern void add_box_restart(MENU_ITEM *menu_item);
extern void add_box_exec(Gpoint pt);
extern void add_box_cleanup(MENU_ITEM *menu_item);

extern void add_circle_init(MENU_ITEM *menu_item);
extern void add_circle_restart(MENU_ITEM *menu_item);
extern void add_circle_exec(Gpoint pt);
extern void add_circle_cleanup(MENU_ITEM *menu_item);

extern void add_conic_init(MENU_ITEM *menu_item);
extern void add_conic_restart(MENU_ITEM *menu_item);
extern void add_conic_exec(Gpoint pt);
extern void add_conic_cleanup(MENU_ITEM *menu_item);

extern void add_text_init(MENU_ITEM *menu_item);
extern void add_text_restart(MENU_ITEM *menu_item);
extern void add_text_exec(Gpoint pt);
extern void add_text_cleanup(MENU_ITEM *menu_item);

void add_objects_undo(MENU_ITEM *menu_item);
void alloc_stroke_font_tbl(IDX **list, int *n);

/*
 *      popups
 */

extern void init_line_width_popup(POPUP *popup);
extern void switch_line_width_popup(POPUP *popup, Gpoint pt);
extern Gfloat get_line_width(void);
extern Gint get_line_style(void);
void init_line_connected_popup(POPUP *popup);
extern void switch_line_connected_state(POPUP *popup, Gpoint pt);
extern Gint get_line_connected(void);
extern void init_line_style_popup(POPUP *popup);
extern void switch_line_style_state(POPUP *popup, Gpoint pt);
extern void switch_curve_connected_state(POPUP *popup, Gpoint pt);
extern void init_fill_style_popup(POPUP *popup);
extern void switch_fill_style_state(POPUP *popup, Gpoint pt);
extern Gflinter get_fill_style(void);
extern Gfloat get_char_expansion(void);
extern void init_char_width_popup(POPUP *popup);
extern void switch_char_width_popup(POPUP *popup, Gpoint pt);
extern Gfloat get_char_width(CHAR_OB *ch_ptr);
extern void init_char_height_popup(POPUP *popup);
extern void switch_char_height_popup(POPUP *popup, Gpoint pt);
extern Gfloat get_char_height(void);
extern void init_text_path_popup(POPUP *popup);
extern void switch_text_path_popup(POPUP *popup, Gpoint pt);
extern Gtxpath get_text_path(void);
extern void init_font_style_popup(POPUP *popup);
extern void switch_font_style_popup(POPUP *popup, Gpoint pt);
extern void init_horz_just_popup(POPUP *popup);
extern void add_box_ntart(MENU_ITEM *menu_item);
extern void switch_horz_just_popup(POPUP *popup, Gpoint pt);
extern Gtxhor get_horz_just(void);
extern void init_vert_just_popup(POPUP *popup);
extern void switch_vert_just_popup(POPUP *popup, Gpoint pt);
extern Gtxver get_vert_just(void);
