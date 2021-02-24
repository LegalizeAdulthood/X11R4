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

#if !defined(PALETTE_H)
#define PALETTE_H

#include <xgks.h>

#include "screen_items.h"

#define	MAX_PAL_ENTRIES	80
#define	NOT_ALLOCATED	-1

extern int nrows, ncols;
extern IDX palstartclr;
extern IDX display_fgdclr;
extern IDX display_bgdclr;
extern IDX display_fillclr;

extern AREA active_clr_area;

int get_fgdclr(void);
int get_fillclr(void);
int get_clr_idx_from_pt( Gpoint pt);
int get_bgdclr(void);
void init_palette_clrs( char *filename);
void init_clr_area_clrs(void);
void set_fgdclr(IDX idx);
void set_bgdclr(IDX idx);
void set_fillclr(IDX idx);

#endif
