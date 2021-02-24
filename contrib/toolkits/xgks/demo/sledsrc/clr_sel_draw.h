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

#if !defined(CLR_SEL_DRAW_H)
#define CLR_SEL_DRAW_H

#include "changes.h"
#include "clr_selector.h"

#define		CLR_WHEEL_TEXT	"clr wheel text key"
#define		CLR_PICKED_TEXT	"clr picked text key"
#define		MIXER_TEXT	"clr mixer text key"
#define		TUNER_TEXT	"fine tuner text key"

void init_clr_selector_clrs(void);
void draw_knob( ARROW *arrow, IDX fill_color);
void draw_arrow( ARROW *arrow, IDX line_color);
void erase_picked_clr(void);
void display_picked_clr(Gcobundl rgb_clr);
void display_clr_wheel_area(void);

#endif
