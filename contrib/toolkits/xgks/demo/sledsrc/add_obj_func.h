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
 * menu items
 */

#include <xgks.h>

#include "changes.h"

extern int add_objects_init();

extern int add_line_init();
extern int add_line_restart();
extern int add_line_exec();
extern int add_line_cleanup();

extern int add_curve_init();
extern int add_curve_restart();
extern int add_curve_exec();
extern int add_curve_cleanup();

extern int add_box_init();
extern int add_box_restart();
extern int add_box_exec();
extern int add_box_cleanup();

extern int add_circle_init();
extern int add_circle_restart();
extern int add_circle_exec();
extern int add_circle_cleanup();

extern int add_conic_init();
extern int add_conic_restart();
extern int add_conic_exec();
extern int add_conic_cleanup();

extern int add_text_init();
extern int add_text_restart();
extern int add_text_exec();
extern int add_text_cleanup();

extern int add_objects_undo();


/*
 *	popups
 */

extern int init_line_width_popup();
extern int switch_line_width_popup();
extern Gfloat get_line_width();
extern Gint get_line_style();
extern int init_line_connected_popup();
extern int switch_line_connected_state();
extern Gint get_line_connected();
extern int init_line_style_popup();
extern int switch_line_style_state();
extern Gint switch_line_style_state();
extern int switch_curve_connected_state();
extern int init_fill_style_popup();
extern int switch_fill_style_state();
extern Gflinter get_fill_style();
extern Gfloat get_char_expansion();
extern int init_char_width_popup();
extern int switch_char_width_popup();
extern Gfloat get_char_width();
extern int init_char_height_popup();
extern int switch_char_height_popup();
extern Gfloat get_char_height();
extern int init_text_path_popup();
extern int switch_text_path_popup();
extern Gtxpath get_text_path();
extern int init_font_style_popup();
extern int switch_font_style_popup();
extern IDX get_font_style();
extern int init_horz_just_popup();
extern int switch_horz_just_popup();
extern Gtxhor get_horz_just();
extern int init_vert_just_popup();
extern int switch_vert_just_popup();
extern Gtxver get_vert_just();
