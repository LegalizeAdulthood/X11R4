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
 * Declaration of conversion functions from enum's to integers and back:
 */
#if !defined(CONVERSIONS_H)
#define CONVERSIONS_H

#include <xgks.h>

Gflinter num_to_style( int i);
Gtxver num_to_vert(int i);
Gtxhor num_to_horz(int i);
Gtxpath num_to_path(int i);
Gsegvis num_to_vis( int i);

int style_to_num(Gflinter i);
int vert_to_num(Gtxver i);
int horz_to_num(Gtxhor i);
int path_to_num(Gtxpath i);
int vis_to_num(Gsegvis i); 

#endif
