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

#if !defined(CLR_SEL_FUNC_H)
#define CLR_SEL_FUNC_H

#include <xgks.h>

#include "clr_selector.h"
#include "screen_items.h"

/* screen area functions */

extern  void clr_wheel_area_exec( AREA area, IDX transno, Gpoint pt);
extern  void clr_mixer_area_exec( AREA area, IDX transno, Gpoint pt);
extern  void fine_tuner_area_exec( AREA area, IDX transno, Gpoint pt);
extern  void clr_selector_pal_area_exec( AREA area, IDX transno, Gpoint pt);

extern  void display_clr_picked_area(void);
extern void display_clr_mixer_area(void);
extern void display_fine_tuner_area(void);
extern void display_clr_selector_pal_area(void);

/* clr selector menu functions */

extern  void colors_mitem_exec( MENU_ITEM *menu_item);
extern  void init_colors_menu( AREA area, MENU *menu);
extern  void cleanup_colors_menu( AREA area, MENU *menu);
void display_arrow( AREA area, ARROW *arrow, Gfloat new_knob_pos);

#endif
