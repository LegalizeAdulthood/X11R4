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
 * color structs
 */
#if !defined(COLOR_H)
#define COLOR_H

#include <xgks.h>

#include "changes.h"

#define UNDEFINED -1 /* arbitrary unused val when \
                        hue is undefined */

/* color in hsv color model */

typedef struct hsv HSV;
struct hsv
{
    Gfloat hue; /* hue */
    Gfloat sat; /* saturation */
    Gfloat val; /* value */
};

extern IDX black;
extern IDX white;
extern IDX grey;
extern IDX bgdclr;
extern IDX fgdclr;
extern IDX fillclr;

IDX alloc_clr_tbl_entries(int n);
int get_num_free_clrs(void);
void set_color(IDX idx, Gcobundl *rgb_clr);
void hsv_to_rgb(HSV *hsv_clr, Gcobundl *rgb_clr);
void get_color(IDX idx, Gcobundl *rgb_clr);
void rgb_to_hsv(Gcobundl *rgb_clr, HSV *hsv_clr);
void init_screen_clrs(void);

#endif
