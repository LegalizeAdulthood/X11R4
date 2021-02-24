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
 *  stack to save states of which trans are active
 */

#if !defined(TRANS_H)
#define TRANS_H

#include <xgks.h>

#include "screen_items.h"

#define NON_EXISTENT -1

typedef struct trans_stk TRANS_STK;
struct trans_stk
{
    IDX transno;
    TRANS_STK *next;
};

/*
typedef struct extent_holder *EXTENT_PTR;
struct extent_holder
{
        Gpoint pt[2];
        EXTENT_PTR next;
};
*/

#define DEFAULT_TRANS 0

/*
 *  rectangular boundaries are stored as follows:
 *
 *  boundary[MIN].x = xmin
 *  boundary[MAX].x = xmax
 *  boundary[MIN].y = ymin
 *  boundary[MAX].y = ymax
 */

#define MIN 0
#define MAX 1

void push_curr_trans(void);
void activate(AREA area);
void pop_curr_trans(void);
int find_trans(AREA area);
void alloc_trans(AREA area);
void get_window(AREA area, Gpoint *window);
void get_viewport(AREA area, Gpoint *viewport);
void set_window(AREA area, Gpoint *window);
void free_trans(AREA area);
void get_max_dc_coor(Gfloat *x, Gfloat *y);
void adjust_screen_tbl_viewports(void);
void init_trans_tbls(void);
int is_ident(Gfloat *trans);

#endif
