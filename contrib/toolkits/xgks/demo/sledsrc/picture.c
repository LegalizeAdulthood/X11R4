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
 * this file includes:
 *	display_picture_area
 */

#include "key.h"
#include "draw.h"
#include "trans.h"
#include <xgks.h>

/*
 *  display_picture_area
 *
 *  description:	displays picture_area
 *
 *  parameters:		none
 */

display_picture_area()
{
	Gpoint window[2];		/* picture area window */
	Gpoint box[4];			/* pts of window */

	/* draw screen area */

	push_curr_trans();
	if (find_trans(PICT_AREA) == NON_EXISTENT)
		alloc_trans(PICT_AREA);
	activate(PICT_AREA);

	set_aspect_flags(GBUNDLED);

	get_viewport(PICT_AREA,window);
	set_window(PICT_AREA,window);
	load_box(box,window[MIN].x,window[MAX].x,
		window[MIN].y,window[MAX].y);
	gsetfillind(BGDCLR_SOLID);
	gfillarea(4,box);
	gsetfillind(WHITE_HOLLOW);
	gfillarea(4,box);

	set_aspect_flags(GINDIVIDUAL);

	/* redraw objects */

	redraw_objects();

	pop_curr_trans();

}  /* end display_picture_area */
