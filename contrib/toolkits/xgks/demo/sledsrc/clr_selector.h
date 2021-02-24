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
 */

#if !defined(CLR_SELECTOR_H)
#define CLR_SELECTOR_H

#include <xgks.h>

#include "changes.h"

#define VERT 0
#define HORZ 1
#define NO_KNOB_DRAWN -1

#define ARROW_LEN .008
#define KNOB_LEN .030
#define KNOB_WD .015

typedef struct arrow_struct ARROW;
struct arrow_struct
{
    int direction;   /* VERT | HORZ */
    Gfloat maxi;     /* max coor */
    Gfloat mini;     /* min coor */
    Gfloat con;      /* constant coor */
    Gfloat knob_pos; /* knob position */
};

extern ARROW mixer_arrow;
extern ARROW val_arrow;
extern ARROW sat_arrow;
extern ARROW hue_arrow;

extern IDX display_clr_picked; /* color picked up by user */
extern IDX mixer_clr;          /* first color in mixer from 
                                           left to right */
#define LEFT_CLR (mixer_clr)
#define MIDDLE_CLR (mixer_clr + 1)
#define RIGHT_CLR (mixer_clr + 2)

extern IDX tuner_clr;       /* fine tuner color */
extern IDX wheel_start_clr; /* first color in color wheel */

extern BOOLEAN clr_picked; /* true if clr picked by user */

extern Gpoint left_extent[2], middle_extent[2],
    right_extent[2];                   /* mixer areas */
extern Gpoint tuner_extent[2];         /* tuner area */
extern Gpoint clr_wheel_center;        /* center of clr clr wheel */
extern Gfloat outside_rad, inside_rad; /* inside and outside rad of color wheeel */

#endif
