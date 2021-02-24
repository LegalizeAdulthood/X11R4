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

#include <xgks.h>

#include "draw.h"
#include "functbl.h"
#include "key.h"
#include "trans.h"
#include "ws.h"

#define FOUR_BY_FIVE 1
#define EIGHT_BY_TEN 2
#define KONICA_FT1 3
#define ONE_TO_ONE 4

/*
 *  photo
 *
 *  description:        calls photograph menu item if there
 *                      is a non-empty picture to photograph
 *
 *  parameters:         none
 */

photo(menu_item)
    MENU_ITEM *menu_item;
{
    if (no_objects() == FALSE)
        display_child_menu(menu_item);
    else
        full_prompt(42, 2);
} /* end photo */

/*
 *  four_by_five
 *
 *  description:        snaps picture using four by five film
 *
 *  parameters:         four_by_five menu item
 */

four_by_five(menu_item)
    MENU_ITEM *menu_item;
{
    photograph(FOUR_BY_FIVE);
    currmitem = (MENU_ITEM *) NULL;
}; /* four_by_five */

/*
 *  eight_by_ten
 *
 *  description:        snaps picture using four by five film
 *
 *  parameters:         eight_by_ten menu item
 */

eight_by_ten(menu_item)
    MENU_ITEM *menu_item;
{
    photograph(EIGHT_BY_TEN);
    currmitem = (MENU_ITEM *) NULL;
}; /* eight_by_ten */

/*
 *  konica_ft1
 *
 *  description:        snaps picture using four by five film
 *
 *  parameters:         konica_ft1 menu item
 */

konica_ft1(menu_item)
    MENU_ITEM *menu_item;
{
    photograph(KONICA_FT1);
    currmitem = (MENU_ITEM *) NULL;
}; /* konica_ft1 */

/*
 *  one_to_one
 *
 *  description:        snaps picture using four by five film
 *
 *  parameters:         one_to_one menu item
 */

one_to_one(menu_item)
    MENU_ITEM *menu_item;
{
    photograph(ONE_TO_ONE);
    currmitem = (MENU_ITEM *) NULL;
}; /* one_to_one */

/*
 *  photograph 
 *
 *  description:        photographs screen
 *
 *  parameters:         pg (IDX) - page parameter
 */

photograph(pg)
    IDX pg;
{
    REAL xmax_dc, ymax_dc;
    NDCPT new_view[2];
    NDCPT viewport[2];
    REAL dx, dy;
    REAL delta_x, delta_y;
    WCPT box[5];

    /* erase everything and center picture area */

    get_max_dc_coor(&xmax_dc, &ymax_dc);
    get_viewport(PICT_AREA, viewport);
    dx = viewport[MAX].ndc_x - viewport[MIN].ndc_x;
    dy = viewport[MAX].ndc_y - viewport[MIN].ndc_y;

    if ((dy / dx) > (ymax_dc / xmax_dc))
    {
        new_view[MAX].ndc_y = 1.0;
        new_view[MIN].ndc_y = 0.0;
        delta_x = dx / dy;
        new_view[MIN].ndc_x = (xmax_dc / ymax_dc - delta_x) / 2.0;
        new_view[MAX].ndc_x = new_view[MIN].ndc_x + delta_x;
    }
    else
    {
        new_view[MAX].ndc_x = 1.0;
        new_view[MIN].ndc_x = 0.0;
        delta_y = dy / dx;
        new_view[MIN].ndc_y = (ymax_dc / xmax_dc - delta_y) / 2.0;
        new_view[MAX].ndc_y = new_view[MIN].ndc_y + delta_y;
    }

    /* set viewport */

    set_viewport(PICT_AREA, new_view);

    /* draw picture */

    push_curr_trans();
    s_norm_trans(DEFAULT_TRANS);
    load_box(box, 0.0, 1.0, 0.0, 1.0);
    s_fill_idx(BGDCLR_SOLID);
    set_aspect_flags(BUNDLED);
    fill_area(4, box);
    activate(PICT_AREA);
    set_aspect_flags(INDIVIDUAL);
    draw_objects();
    pop_curr_trans();

/* take the picture
           the profile file is .mat_profile
           the log file is mat_log
        */

#ifdef CAMERA
    mat_open(MAT_PROFILE, MAT_LOG_FILE); /* open film recorder */
    mat_page_st(pg);                     /* select parm pg. */
    mat_expose();                        /* take a picture */
    mat_close();                         /* close film recorder */
#endif

    /* prompt user to inform sled when camera is done */

    printf("wait until camera snaps then enter any character\n");
    set_cbreak_mode(ON);
    (void) getchar();
    set_cbreak_mode(OFF);

    /* set viewport */

    set_viewport(PICT_AREA, viewport);

    /* redraw screen area */

    draw_screen_background();
    prompt(41);

    /*      unnecessary redraw 'cause area not reactivated
         *      yes, I know - it's kludgey!

        (*(screen_tbl[find_screen_tbl_idx(MSG_AREA)].draw))();
        */

    (*(screen_tbl[find_screen_tbl_idx(PAL_AREA)].draw))();
    (*(screen_tbl[find_screen_tbl_idx(FGDCLR_AREA)].draw))();
    (*(screen_tbl[find_screen_tbl_idx(BGDCLR_AREA)].draw))();
    (*(screen_tbl[find_screen_tbl_idx(FILLCLR_AREA)].draw))();
    (*(screen_tbl[find_screen_tbl_idx(PICT_AREA)].draw))();
    (*(screen_tbl[find_screen_tbl_idx(PRI_MENU_AREA)].draw))();
    (*(screen_tbl[find_screen_tbl_idx(SEC_MENU_AREA)].draw))();
    (*(screen_tbl[find_screen_tbl_idx(POPUP_AREA)].draw))();

    prompt(2);

} /* end photograph */
