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
 *      init_colors_menu
 *      cleanup_color_menu
 *      clr_selector_pal_area_exec
 *      clr_wheel_area_exec
 *      pt_on_clr_wheel
 *      find_hue
 *      clr_mixer_area_exec
 *      adjust_middle_clr
 *      fine_tuner_area_exec
 *      set_tuner_clr
 *      adjust_tuner_clr
 *      arrow_picked
 */

#include <xgks.h>

#include <math.h>

#include "screen_items.h"
#include "functbl.h"
#include "draw.h"
#include "clr_selector.h"
#include "defs.h"
#include "key.h"
#include "color.h"


/*
 *  colors_mitem_exec
 *
 *  description:        exec routine for colors menu item
 *                      causes colors menu to be displayed
 *
 *  parameters:         menu_item (MENU_ITEM *) - colors menu item
 *
 */

colors_mitem_exec(menu_item)
MENU_ITEM *menu_item;
{
        prompt(41);
        switch_sec_menu(menu_item->child);
}  /* end colors mitem_exec */


/*
 *  init_colors_menu
 *
 *  description:        Displays colors menu,
 *                      removes PICT_AREA from screen
 *                      and displays CLR_WHEEL_AREA, FINE TUNER,
 *                      CLR_MIXER_AREA. It also switches PAL_AREA
 *                      to CLR_SELECTOR_PAL_AREA although no visual
 *                      changes are reflected on screen. CLR_PICKED_AREA
 *                      is initialized, also. Lastly, global area
 *                      variables, and clr_picked are initalized.
 *
 *  parameters:         area (AREA) - area containing menu
 *                      menu (MENU *) - colors menu
 */

init_colors_menu(area,menu)
AREA area;
MENU *menu;
{
        /* adjust screen */

        erase_area(PICT_AREA);
        free_trans(PICT_AREA);
        erase_area(FGDCLR_AREA);
        free_trans(FGDCLR_AREA);
        erase_area(BGDCLR_AREA);
        free_trans(BGDCLR_AREA);
        erase_area(FILLCLR_AREA);
        free_trans(FILLCLR_AREA);
        free_trans(PAL_AREA);
        screen_tbl[find_screen_tbl_idx(CLR_SELECTOR_PAL_AREA)].draw();
        screen_tbl[find_screen_tbl_idx(CLR_PICKED_AREA)].draw();
        screen_tbl[find_screen_tbl_idx(TUNER_AREA)].draw();
        screen_tbl[find_screen_tbl_idx(MIXER_AREA)].draw();
        screen_tbl[find_screen_tbl_idx(CLR_WHEEL_AREA)].draw();

        /* display colors menu */

        display_menu_n_popups(area,menu);
        prompt(7);

        /* initialize variables */

        clr_picked = FALSE;
        initialize_undo();
}  /* end init_colors menu */


/*
 *  cleanup_colors_menu
 *
 *  description:        restores PICT_AREA and switches 
 *                      CLR_SELECTOR_PAL_AREA to PAL_AREA,
 *                      and removes CLR_WHEEL_AREA, CLR_PICKED_AREA,
 *                      MIXER_AREA and TUNER_AREA.
 *                      cleans up colors menu.
 *
 *  parameters:         area (AREA) - area containing menu
 *                      menu (MENU *) - menu to cleanup
 */

cleanup_colors_menu(area,menu)
AREA area;                      /* area containing menu */
MENU *menu;                     /* colors menu */
{
        /* cleanup menu */

        cleanup_menu_n_popups(area,menu);

        /* restore screen */

        erase_area(CLR_WHEEL_AREA);
        free_trans(CLR_WHEEL_AREA);
        erase_area(TUNER_AREA);
        free_trans(TUNER_AREA);
        erase_area(MIXER_AREA);
        free_trans(MIXER_AREA);
        if (clr_picked)
                erase_area(CLR_PICKED_AREA);
        free_trans(CLR_PICKED_AREA);
        free_trans(CLR_SELECTOR_PAL_AREA);
        alloc_trans(PAL_AREA);
        screen_tbl[find_screen_tbl_idx(FGDCLR_AREA)].draw();
        screen_tbl[find_screen_tbl_idx(BGDCLR_AREA)].draw();
        screen_tbl[find_screen_tbl_idx(FILLCLR_AREA)].draw();
        screen_tbl[find_screen_tbl_idx(PICT_AREA)].draw();
}  /* end cleanup_colors_menu */


/*
 *  clr_selector_pal_area_exec
 *
 *  description:        picks and places colors in palette area
 *
 *  parameters:         area (AREA) - screen area
 *                      transno (IDX) - transno corresponding
 *                              to area
 *                      pt (Gpoint) - pt picked
 */

clr_selector_pal_area_exec(area,transno,pt)
AREA area;
IDX transno;
Gpoint pt;
{
        static Gcobundl old_clr;
        Gcobundl clr;
        IDX idx;

        check_undo_flag();
        idx = get_clr_idx_from_pt(pt);
        if (clr_picked)
        {
                if (test_undo_flag() == FALSE)
                        get_color(idx,&old_clr);
                get_color(display_clr_picked,&clr);
                set_color(idx,&clr);
                erase_picked_clr();
                log_last_area(area,transno,pt);
        }
        else    /* clr_picked == FALSE */
        {
                get_color(idx,&clr);
                display_picked_clr(clr);
                if (test_undo_flag() == TRUE)
                        set_color(idx,&old_clr);
                log_last_area(area,transno,pt);
        }
}


/*
 *  clr_wheel_area_exec
 *
 *  description:        determines which color was selected off
 *                      color wheel and handles undo
 *
 *  parameters:         area (AREA) - screen area
 *                      transno (IDX) - transno corresponding
 *                              to area
 *                      pt (Gpoint) - pt picked
 */

clr_wheel_area_exec(area,transno,pt)
AREA area;
IDX transno;
Gpoint pt;
{
        static HSV hsv_clr =
        {
                0.0,            /* hue */
                1.0,            /* saturation */
                1.0             /* value */
        };
        Gcobundl rgb_clr;
        extern Gfloat find_hue();

        check_undo_flag();

        /* if pt is not on clr wheel or color has already
           been picked and test_undo_flag()flag is FALSE then return */

        if (!pt_on_clr_wheel(pt))
                return;
        else if ((clr_picked == TRUE) && (test_undo_flag() == FALSE))
                return;

        if (test_undo_flag() == FALSE)
        {
                log_last_area(area,transno,pt);
                hsv_clr.hue = find_hue(clr_wheel_center,pt);
                hsv_to_rgb(&hsv_clr,&rgb_clr);
                display_picked_clr(rgb_clr);
        }
        else  /* test_undo_flag() = TRUE */
        {
                erase_picked_clr();
        }
}  /* end clr_wheel_area_exec */


/*
 *  pt_on_clr_wheel
 *
 *  description:        determines if pt picked is on clr wheel
 *
 *  parameters:         pt (Gpoint) - pt picked by user
 *
 *  returns:            (BOOLEAN) - true if pt on clr wheel, 
 *                              false otherwise
 */

pt_on_clr_wheel(pt)
Gpoint pt;
{
        Gfloat distance;

        distance = dist(pt,clr_wheel_center);

        if ((distance >= inside_rad) && (distance <= outside_rad))
                return(TRUE);
        else
                return(FALSE);
}  /* end pt_on_clr_wheel */


/*
 *  find_hue
 *
 *  description:        calculates hue of color picked based on
 *                      center of color wheel and pt picked
 *
 *  parameters:         center (Gpoint) - center of color wheel
 *                      pt (Gpoint) - pt picked
 *
 *  returns:            hue (Gfloat) - hue of color picked
 */

Gfloat
find_hue(center,pt)
Gpoint center;
Gpoint pt;
{
        Gfloat x,y;                     /* x and y values after
                                           translating center to
                                           origin */
        Gfloat inv_tan = 0;                     /* arctan(abs(y/x)) */
        Gfloat inv_cot = 0;                     /* arccot(abs(x/y)) */

        Gfloat hue =0;                  /* hue of color selected */

        /* translate pt to origin */

        x = pt.x - center.x;
        y = pt.y - center.y;

        /* calculate angle (in radians) from positive x-axis */

        if (x != 0)
                inv_tan = (360 / (2 * PI)) * atan(abs(y / x));
        if (y != 0)
                inv_cot = (360 / (2 * PI)) * atan(abs(x / y));

        if ((x > 0) && (y >= 0))        /* 0 <= angle < 90 degrees */
                hue = inv_tan;
        else if ((x <= 0) && (y > 0))   /* 90 <= angle < 180 degrees */
                hue = 90 + inv_cot;
        else if ((x < 0) && (y <= 0))   /* 180 <= angle < 270 degrees */
                hue = 180 + inv_tan;
        else if ((x >= 0) && (y < 0))   /* 270 <= angle < 360 degrees */
        {
                hue = 270 + inv_cot;
                if (hue > 360.0)
                        hue = hue - 360.0;
        }
        return(hue);
}  /* end find_hue */


/*
 *  clr_mixer_area_exec
 *
 *  description:        adjusts clr mixer knob and places
 *                      or picks clrs from mixer
 *
 *  parameters:         area (AREA) - screen area
 *                      transno (IDX) - transno corresponding
 *                              to area
 *                      pt (Gpoint) - pt picked
 */

clr_mixer_area_exec(area,transno,pt)
AREA area;
IDX transno;
Gpoint pt;
{
        static Gfloat old_knob_pos;
        static Gcobundl old_clr;
        Gcobundl clr;

        check_undo_flag();
        if (test_undo_flag() == FALSE)
        {
                if (arrow_picked(mixer_arrow,pt))
                {
                        old_knob_pos = mixer_arrow.knob_pos;
                        display_arrow(MIXER_AREA,&mixer_arrow,pt.x);
                        adjust_middle_clr();
                        log_last_area(area,transno,pt);
                }
                else if (in_bounds_delta(left_extent,pt,0.0))
                {
                        if (clr_picked)
                        {
                                get_color(LEFT_CLR,&old_clr);
                                get_color(display_clr_picked,&clr);
                                set_color(LEFT_CLR,&clr);
                                adjust_middle_clr();
                                erase_picked_clr();
                        }
                        else
                        {
                                get_color(LEFT_CLR,&clr);
                                display_picked_clr(clr);
                        }
                        log_last_area(area,transno,pt);
                }
                else if (in_bounds_delta(right_extent,pt,0.0))
                {
                        if (clr_picked)
                        {
                                get_color(RIGHT_CLR,&old_clr);
                                get_color(display_clr_picked,&clr);
                                set_color(RIGHT_CLR,&clr);
                                adjust_middle_clr();
                                erase_picked_clr();
                        }
                        else
                        {
                                get_color(RIGHT_CLR,&clr);
                                display_picked_clr(clr);
                        }
                        log_last_area(area,transno,pt);
                }
                else if (in_bounds_delta(middle_extent,pt,0.0) && !clr_picked)
                {
                        get_color(MIDDLE_CLR,&clr);
                        display_picked_clr(clr);
                        log_last_area(area,transno,pt);
                }
                else
                {
                        reprompt(1);
                }
        }
        else            /* test_undo_flag() == TRUE */
        {
                if (arrow_picked(mixer_arrow,pt))
                {
                        display_arrow(MIXER_AREA,
                                &mixer_arrow,old_knob_pos);
                        adjust_middle_clr();
                }
                else if (in_bounds_delta(left_extent,pt,0.0))
                {
                        if (clr_picked)
                        {
                                erase_picked_clr();
                        }
                        else
                        {
                                get_color(LEFT_CLR,&clr);
                                display_picked_clr(clr);
                                set_color(LEFT_CLR,&old_clr);
                                adjust_middle_clr();
                        }
                }
                else if (in_bounds_delta(right_extent,pt,0.0))
                {
                        if (clr_picked)
                        {
                                erase_picked_clr();
                        }
                        else
                        {
                                get_color(RIGHT_CLR,&clr);
                                display_picked_clr(clr);
                                set_color(RIGHT_CLR,&old_clr);
                                adjust_middle_clr();
                        }
                }
                else /* middle area picked */
                {
                        erase_picked_clr();
                }
        }
}  /* end clr_mixer_area_exec */


/*
 *  adjust_middle_clr
 *
 *  description:        adjusts middle color based on knob
 *                      pos and left and right colors.
 *                      the middle color is based on a linear
 *                      interpolation of the rgb values
 *                      of the left and right colors.
 *
 *  parameters:         none
 */

adjust_middle_clr()
{
        Gfloat p;
        Gcobundl left,right,middle;

        p = (mixer_arrow.knob_pos - mixer_arrow.mini)
                / (mixer_arrow.maxi - mixer_arrow.mini);

        get_color(LEFT_CLR,&left);
        get_color(RIGHT_CLR,&right);

        middle.red = (1-p) * left.red + p * right.red;
        middle.green = (1-p) * left.green + p * right.green;
        middle.blue = (1-p) * left.blue + p * right.blue;

        set_color(MIDDLE_CLR,&middle);
}  /* end adjust_middle_clr */


/*
 *  fine_tuner_area_exec
 *
 *  description:        handles all knob ajustments and
 *                      color selections in fine tuner area
 *
 *  parameters:         area (AREA) - area picked
 *                      transno (IDX) - corresponding trans no
 *                      pt (Gpoint) - pt picked
 */

fine_tuner_area_exec(area,transno,pt)
AREA area;
IDX transno;
Gpoint pt;
{
        static Gfloat old_knob_pos;
        static Gcobundl old_clr;
        Gcobundl rgb_clr;

        check_undo_flag();
        if (test_undo_flag() == FALSE)
        {
                if (arrow_picked(hue_arrow,pt))
                {
                        old_knob_pos = hue_arrow.knob_pos;
                        display_arrow(TUNER_AREA,&hue_arrow,pt.x);
                        adjust_tuner_clr();
                        log_last_area(area,transno,pt);
                }
                else if (arrow_picked(val_arrow,pt))
                {
                        old_knob_pos = val_arrow.knob_pos;
                        display_arrow(TUNER_AREA,&val_arrow,pt.y);
                        adjust_tuner_clr();
                        log_last_area(area,transno,pt);
                }
                else if (arrow_picked(sat_arrow,pt))
                {
                        old_knob_pos = sat_arrow.knob_pos;
                        display_arrow(TUNER_AREA,&sat_arrow,pt.y);
                        adjust_tuner_clr();
                        log_last_area(area,transno,pt);
                }
                else if (in_bounds_delta(tuner_extent,pt,0.0))
                {
                        if (clr_picked)
                        {
                                get_color(tuner_clr,&old_clr);
                                get_color(display_clr_picked,&rgb_clr);
                                set_tuner_clr(rgb_clr);
                                erase_picked_clr();
                        }
                        else
                        {
                                get_color(tuner_clr,&rgb_clr);
                                display_picked_clr(rgb_clr);
                        }
                        log_last_area(area,transno,pt);
                }
                else
                        reprompt(1);
        }
        else    /* test_undo_flag() == TRUE */
        {
                if (arrow_picked(hue_arrow,pt))
                {
                        display_arrow(TUNER_AREA,
                                &hue_arrow,old_knob_pos);
                        adjust_tuner_clr();
                }
                else if (arrow_picked(val_arrow,pt))
                {
                        display_arrow(TUNER_AREA,
                                &val_arrow,old_knob_pos);
                        adjust_tuner_clr();
                }
                else if (arrow_picked(sat_arrow,pt))
                {
                        display_arrow(TUNER_AREA,
                                &sat_arrow,old_knob_pos);
                        adjust_tuner_clr();
                }
                else if (in_bounds_delta(tuner_extent,pt,0.0))
                {
                        if (clr_picked)
                        {
                                erase_picked_clr();
                        }
                        else
                        {
                                get_color(tuner_clr,&rgb_clr);
                                display_picked_clr(rgb_clr);
                                set_tuner_clr(old_clr);
                        }
                }
        }

#ifdef PRINTCOLOR
        get_color(tuner_clr,&rgb_clr);
        printf("tuner color: r = %.3f, g = %.3f, b = %.3f\n",
                rgb_clr.red, rgb_clr.green, rgb_clr.blue);
#endif
}  /* end fine_tuner_area_exec */


/*
 *  set_tuner_clr
 *
 *  description:        places clr in fine tuner and sets knobs
 *
 *  parameters:         rgb_clr (Gcobundl) - clr to set
 */

set_tuner_clr(rgb_clr)
Gcobundl rgb_clr;
{
        HSV hsv_clr;

        rgb_to_hsv(&rgb_clr,&hsv_clr);
        set_color(tuner_clr,&rgb_clr);
        display_arrow(TUNER_AREA,&hue_arrow,(hsv_clr.hue / 360.0) *
                (hue_arrow.maxi - hue_arrow.mini) +
                hue_arrow.mini);
        display_arrow(TUNER_AREA,&val_arrow,hsv_clr.val *
                (val_arrow.maxi - val_arrow.mini) +
                val_arrow.mini);
        display_arrow(TUNER_AREA,&sat_arrow,hsv_clr.sat *
                (sat_arrow.maxi - sat_arrow.mini) +
                sat_arrow.mini);
}  /* set_tuner_clr */


/*
 *  adjust_tuner_clr
 *
 *  description:        sets tuner clr to clr defined by hue, val
 *                      and sat arrows
 *
 *  parameters:         none
 */

adjust_tuner_clr()
{
        HSV hsv_clr;
        Gcobundl rgb_clr;

        hsv_clr.hue = 360 * (hue_arrow.knob_pos - hue_arrow.mini) /
                (hue_arrow.maxi - hue_arrow.mini);
        hsv_clr.val = (val_arrow.knob_pos - val_arrow.mini) /
                (val_arrow.maxi - val_arrow.mini);
        hsv_clr.sat = (sat_arrow.knob_pos - sat_arrow.mini) /
                (sat_arrow.maxi - sat_arrow.mini);
        hsv_to_rgb(&hsv_clr,&rgb_clr);
        set_color(tuner_clr,&rgb_clr);
}  /* adjust_tuner_clr */


/*
 *  arrow_picked
 *
 *  description:        determines if arrow picked by user
 *
 *  parameters:         arrow (ARROW) - arrow to test against
 *                      pt (Gpoint) - pt picked
 *
 *  returns:            BOOLEAN - true if arrow picked
 */

arrow_picked(arrow,pt)
ARROW arrow;
Gpoint pt;
{
        if (arrow.direction == VERT)
        {
                if ((arrow.mini <= pt.y) &&
                        (arrow.maxi >= pt.y) &&
                        (abs(arrow.con - pt.x)
                        <= (ARROW_LEN * 2.0)))
                {
                        return(TRUE);
                }
                else
                {
                        return(FALSE);
                }
        }
        else /* direction == HORZ */
        {
                if ((arrow.mini <= pt.x) &&
                        (arrow.maxi >= pt.x) &&
                        (abs(arrow.con - pt.y)
                        <= (ARROW_LEN * 2.0)))
                {
                        return(TRUE);
                }
                else
                {
                        return(FALSE);
                }
        }
}  /* end arrow_picked */
