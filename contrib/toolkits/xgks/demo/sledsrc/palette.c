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
 *  this file contains:
 *      set_fgdclr
 *      get_fgdclr
 *      set_bgdclr
 *      get_bgdclr
 *      init_palette_clrs
 *      get_clr_idx_from_pt
 */
#include <xgks.h>

#include <stdio.h>

#include "changes.h"
#include "color.h"
#include "defs.h"
#include "error.h"
#include "io.h"
#include "key.h"
#include "palette.h"
#include "trans.h"
#include "ws.h"

Gint nrows;                      /* number of rows in
                                           in color table */
Gint ncols;                      /* number of columns in
                                           in color table */
IDX palstartclr = NOT_ALLOCATED; /* index of first palette
                                           color in color table */
IDX display_fgdclr;              /* fgdclr area clr tbl entry */
IDX display_bgdclr;              /* bgdclr area clr tbl entry */
IDX display_fillclr;             /* fillclr area clr tbl entry */

/*
 *  set_fgdclr
 *
 *  description:        sets fgdclr to color in palette
 *                      and sets color table entry display_fgdclr
 *                      to color in color table entry fgdclr
 *
 *  parameters:         idx (IDX) - index of color to set fgdclr to
 */

void set_fgdclr(IDX idx)
{
    Gcobundl rgb_clr; /* current fgdclr */

    fgdclr = idx;

    get_color(idx, &rgb_clr);
    set_color(display_fgdclr, &rgb_clr);

} /* end set_fgdclr */

/*
 *  get_fgdclr
 *
 *  parameters:         none
 *
 *  returns:            index of colorin palette which matches fgdclr
 */
int get_fgdclr(void)
{
    return (fgdclr);
} /* end get_fgdclr */

/*
 *  set_bgdclr
 *
 *  description:        sets bgdclr to color in palette
 *                      and sets color table entry display_bgdclr
 *                      to color in color table entry bgdclr
 *
 *  parameters:         idx (IDX) - index of color to set bgdclr to
 */

void set_bgdclr(IDX idx)
{
    Gcobundl rgb_clr; /* current bgdclr */

    bgdclr = idx;
    get_color(idx, &rgb_clr);
    set_color(display_bgdclr, &rgb_clr);

} /* end set_bgdclr */

/*
 *  get_bgdclr
 *
 *  parameters:         none
 *
 *  returns:            display_bgdclr (IDX) - index of bgdclr
 */

int get_bgdclr(void)
{
    return (display_bgdclr);
} /* end get_bgdclr */

/*
 *  set_fillclr
 *
 *  description:        sets fillclr to color in palette
 *                      and sets color table entry display_fillclr
 *                      to color in color table entry fillclr
 *
 *  parameters:         idx (IDX) - index of color to set fillclr to
 */

void set_fillclr(IDX idx)
{
    Gcobundl rgb_clr; /* current fillclr */

    fillclr = idx;
    get_color(idx, &rgb_clr);
    set_color(display_fillclr, &rgb_clr);

} /* end set_fillclr */

/*
 *  get_fillclr
 *
 *  parameters:         none
 *
 *  returns:            index of colorin palette which matches fillclr
 */

int get_fillclr(void)
{
    return (fillclr);
} /* end get_fillclr */

/*
 *  init_palette_clrs
 *
 *  description:        allocates color table space
 *                      for palette colors and initializes
 *                      colors in color table
 *                      as well as initializing fgdclr to first
 *                      entry in color table (top row, leftmost column)
 *
 *  parameters:         filename (char *) - name of file
 *                              containing palette size
 *                              and initial colors
 *                              in hsv form
 */

void init_palette_clrs(char *filename)
{
    FILE *fd;         /* fd for file containing
                                           colors */
    HSV hsv_clr;      /* color as read in */
    Gcobundl rgb_clr; /* same color in rgb form */
    int i;

    /* open file for reading */

    fd = fopen(filename, "r");
    if (fd == (FILE *) NULL)
        exit_error("init_palette_clrs", 6);

    /* read in number of rows and columns */

    get_int(fd, &(nrows), 7);
    get_int(fd, &(ncols), 7);

    /* read in colors and enter into color table */

    if (palstartclr == NOT_ALLOCATED)
        palstartclr = alloc_clr_tbl_entries(MAX_PAL_ENTRIES);

    for (i = palstartclr; i < (palstartclr + nrows * ncols); i++)
    {
        get_real3(fd, &(hsv_clr.hue), &(hsv_clr.sat),
            &(hsv_clr.val), 7);
        hsv_to_rgb(&hsv_clr, &rgb_clr);
        set_color(i, &rgb_clr);
    }

    /* close file */

    fclose(fd);
} /* init_palette_clrs */

/*
 *  init_clr_area_clrs
 *
 *  description:        initializes clr tbl entries for
 *                      fgdclr, bgdclr and fillclr
 *
 *  parameters:         none
 */

void init_clr_area_clrs(void)
{
    display_fgdclr = alloc_clr_tbl_entries(1);
    set_fgdclr(palstartclr);
    display_bgdclr = alloc_clr_tbl_entries(1);
    set_bgdclr(black);
    display_fillclr = alloc_clr_tbl_entries(1);
    set_fillclr(palstartclr);

} /* end init_clr_area_clrs */

/*
 *  get_clr_idx_from_pt
 *
 *  description:        determines color index from pt
 *
 *  parameters:         pt (Gpoint) - pt corresponding
 *                              to desired color
 *
 *  returns:            (IDX) - index of palette color in clr tbl
 */

int get_clr_idx_from_pt(Gpoint pt)
{
    Gpoint window[2]; /* window[MIN] = (xmin,ymin)
                                           window[MAX] = (xmax,ymax) */

    int row, col; /* row and col containing pt */
    Gfloat w, h;  /* width & height of pal clr */

    /*
         * since idx based on row and col of color,
         * determine row and col of selected color
         *
         * rows (columns) are numbered 0...nrows-1 (ncols-1)
         */

    get_window(PAL_AREA, window);
    w = (window[MAX].x - window[MIN].x) / ncols;
    col = min2(((int) (pt.x / w)), (ncols - 1));

    h = (window[MAX].y - window[MIN].y) / nrows;
    row = min2(((int) ((window[MAX].y - pt.y) / h)),
        (nrows - 1));
    return (palstartclr + row * ncols + col);
} /* end get_clr_idx_from_pt */
