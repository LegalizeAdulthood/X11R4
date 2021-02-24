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
 *  this file includes:
 *      display_palette
 *      display_fgdclr
 *      display_bgdclr
 *      display_fillclr
 *      draw_clr_area
 */

#include <xgks.h>

#include <strings.h>

#include "draw_palette.h"

#include "palette.h"
#include "color.h"
#include "defs.h"
#include "key.h"
#include "trans.h"
#include "draw.h"
#include "changes.h"

/*
 *  display_palette
 *
 *  description:        displays palette
 *
 *  parameters:         none
 */

void display_palette(void)
{
        Gindattr attr;
        int row, col;                   /* tbl entry being displayn */
        Gfloat wd, ht;                  /* wd and ht of conic */
        Gpoint window[2];               /* boundary of palette area */
        Gpoint box[4];                  /* pts on boundary */
        IDX color_idx;                  /* color of (row,color) */
        Gpoint center;                  /* center of (row,col) */

        push_curr_trans();
        if (find_trans(PAL_AREA) == NON_EXISTENT)
                alloc_trans(PAL_AREA);
        activate(PAL_AREA);
        set_aspect_flags(GBUNDLED);

        /* display palette background */

        get_window(PAL_AREA,window);
        load_box(box,window[MIN].x,window[MAX].x,
                window[MIN].y,window[MAX].y);
        gsetfillind(GREY_SOLID);
        gfillarea(4,box);
        gsetfillind(WHITE_HOLLOW);
        gfillarea(4,box);

        /* display individual color splotches in palette */

        set_aspect_flags(GINDIVIDUAL);

        ginqindivattr(&attr);

        wd = 2 * (window[MAX].x - window[MIN].x)
                / (Gfloat)(2 * ncols + ncols + 1);
        ht = 2 * (window[MAX].y - window[MIN].y)
                / (Gfloat)(2 * nrows + nrows + 1);
        color_idx = palstartclr - 1;

        for (row=0; row < nrows; row++)
                for (col=0; col < ncols; col++)
        {
                center.x = wd + col * 1.5 * wd;
                center.y = window[MAX].y - ht
                        - row * 1.5 * ht;
                color_idx ++;

                gsetfillcolorind(color_idx);
                gsetfillintstyle(GSOLID);
                draw_conic(center,wd,ht);

                gsetfillcolorind(black);
                gsetfillintstyle(GHOLLOW);
                draw_conic(center,wd,ht);
        }

        /* restore previous state */

        gsetfillcolorind(attr.flcolor);
        gsetfillintstyle(attr.flstyle);
        pop_curr_trans();

}  /* end display_palette */


/*
 *  display_fgdclr_area
 *
 *  description:        displays fgdclr 
 *
 *  parameters:         none
 */

void display_fgdclr_area(void)
{
        if (eq(active_clr_area,FGDCLR_AREA))
                draw_clr_area(FGDCLR_AREA,TRUE);
        else
                draw_clr_area(FGDCLR_AREA,FALSE);

}  /* end display_fgdclr_area */


/*
 *  display_bgdclr_area
 *
 *  description:        displays bgdclr 
 *
 *  parameters:         none
 */

void display_bgdclr_area(void)
{
        if (eq(active_clr_area,BGDCLR_AREA))
                draw_clr_area(BGDCLR_AREA,TRUE);
        else
                draw_clr_area(BGDCLR_AREA,FALSE);

}  /* end display_bgdclr_area */


/*
 *  display_fillclr_area
 *
 *  description:        displays fillclr 
 *
 *  parameters:         none
 */

void display_fillclr_area(void)
{
        if (eq(active_clr_area,FILLCLR_AREA))
                draw_clr_area(FILLCLR_AREA,TRUE);
        else
                draw_clr_area(FILLCLR_AREA,FALSE);

}  /* end display_fillclr_area */


/*
 *  draw_clr_area
 *
 *  description:        draws clr area with clr splotch 
 *                      of specified color and with
 *                      specified heading
 *
 *  parameters:         fill_area_tbl_area (IDX) - index of color
 *                              for color splotch
 *                      clr_area (AREA) - color area
 *                      hi_lite (BOOLEAN) - true if area should be
 *                              hilited
 *                      text (char **) - text to display
 */

void draw_clr_area( AREA clr_area, BOOLEAN hi_lite)
{
        IDX fill_area_tbl_entry;
        char **lines;
        Gfloat wd, ht;                  /* wd and ht of conic */
        Gpoint window[2];               /* boundary of palette area */
        Gpoint box[4];                  /* pts on boundary */
        Gpoint center;                  /* center of (row,col) */

        static char *fgdclr_lines[] =
        {
                "FOREGROUND",
                "COLOR"
        };
        static char *bgdclr_lines[] =
        {
                "BACKGROUND",
                "COLOR"
        };
        static char *fillclr_lines[] =
        {
                "FILL",
                "COLOR"
        };

        if (eq(clr_area,FGDCLR_AREA))
        {
                fill_area_tbl_entry = FGDCLR_SOLID;
                lines = fgdclr_lines;
        }
        else if (eq(clr_area,BGDCLR_AREA))
        {
                fill_area_tbl_entry = BGDCLR_SOLID;
                lines = bgdclr_lines;
        }
        else /* (eq(clr_area,FILLCLR_AREA)) */
        {
                fill_area_tbl_entry = FILLCLR_SOLID;
                lines = fillclr_lines;
        }

        push_curr_trans();
        if (find_trans(clr_area) == NON_EXISTENT)
                alloc_trans(clr_area);
        activate(clr_area);

        set_aspect_flags(GBUNDLED);

        /* fill in window in bgd_area_clr and outline in 
           fgd_area_clr */

        get_window(clr_area,window);
        load_box(box,window[MIN].x,window[MAX].x,
                window[MIN].y,window[MAX].y);

        if (hi_lite == TRUE)
        {
                gsetfillind(WHITE_SOLID);
                gfillarea(4,box);
        }
        else
        {
                gsetfillind(GREY_SOLID);
                gfillarea(4,box);
                gsetfillind(WHITE_HOLLOW);
                gfillarea(4,box);
        }

        /* display fill_clr */

        wd = (3.0 / 4.0) * (window[MAX].x - window[MIN].x);
        ht = (1.0 / 2.0) * (window[MAX].y - window[MIN].y);
        center.x = (window[MAX].x - window[MIN].x) 
                * (1.0 / 2.0) + window[MIN].x;
        center.y = (window[MAX].y - window[MIN].y) 
                * (3.0 / 8.0) + window[MIN].y;

        gsetfillind(fill_area_tbl_entry);
        draw_conic(center,wd,ht);
        gsetfillind(BLACK_HOLLOW);
        draw_conic(center,wd,ht);

        /* display text */

        center.y = (window[MAX].y - window[MIN].y) 
                * (13.0 / 16.0) + window[MIN].y;
        
        center_text(2,lines,&center,((hi_lite == TRUE) ? grey : white),
                CLR_AREA_TEXT);

        set_aspect_flags(GINDIVIDUAL);

        pop_curr_trans();

}  /* end draw_clr_area */

