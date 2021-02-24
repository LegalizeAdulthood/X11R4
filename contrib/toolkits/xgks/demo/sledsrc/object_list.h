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
#if !defined(OBJECT_LIST_H)
#define OBJECT_LIST_H

#include "objects.h"

extern OBJECT *get_last_object(void);
void enq_comb_ob( COMB_OB *comb_ob);
void add_to_comb_ob(COMB_OB *comb_ob, OBJECT *object_list, int end);
void redraw_objects(void);
void draw_objects(void);
int no_objects(void);
void update_trans( COMB_OB *comb_ob, Gfloat *trans);
void collect_garbage(void);
void enq_after( COMB_OB *comb_ob, COMB_OB *prev_comb_ob);

#endif
