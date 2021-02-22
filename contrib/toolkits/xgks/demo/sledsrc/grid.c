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
 *      adjust_grid
 *      init_grid_menu
 *      get_grid_width
 *      get_grid_color
 *      get_grid_pt
 *      override_snap
 *      resume_snap
 *      get_override_state
 *      draw_grid
 *      erase_grid
 *      snap_to_grid
 *      init_grid_color_popup
 *      switch_grid_color_state
 *      get_grid_color_state
 *      save_old_grid
 *      move_grid_init
 *      move_grid_restart
 *      move_grid_exec
 *      move_grid_cleanup
 *      resize_grid_init
 *      resize_grid_restart
 *      resize_grid_exec
 *      resize_grid_cleanup
 *      adjust_grid_undo
 */

#include "trans.h"
#include "popup.h"
#include "functbl.h"
#include "key.h"
#include "defs.h"
#include "ws.h"
#include "draw.h"
#include "objects.h"
#include "color.h"
#include <math.h>
#include <xgks.h>

IDX grid_color;
Gpoint grid_pt;
Gfloat grid_width;
Gpoint old_grid_pt;
Gfloat old_grid_width;
BOOLEAN override = FALSE;
BOOLEAN undo;
MENU_ITEM *mitem_in_progress;

#define ADJUST_GRID_UNDO        "adjust grid undo menu item"


/*
 *  init_grid
 *
 *  description:        initializes all variables associated
 *                      with grid
 *
 *  parameters:         none
 */


init_grid()
{
        Gpoint window[2];
        Gcobundl rgb;

        get_window(PICT_AREA,window);
        grid_pt = window[MIN];
        grid_color = alloc_clr_tbl_entries(1);
        get_color(white,&rgb);
        set_color(grid_color,&rgb);
        grid_width = (window[MAX].x - window[MIN].x) / 20.0;
}  /* init_grid */


/*
 *  get_grid_width
 *
 *  description:        returns current grid_width
 *
 *  parameters:         none
 *
 *  returns:            (Gfloat) - grid width
 */

Gfloat
get_grid_width()
{
        return(grid_width);
}  /* end get_grid_width */


/*
 *  get_grid_color
 *
 *  description:        returns current grid_color
 *
 *  parameters:         none
 *
 *  returns:            (IDX) - grid color
 */

get_grid_color()
{
        return(grid_color);
}  /* end get_grid_color */


/*
 *  get_grid_pt
 *
 *  description:        returns current grid_pt
 *
 *  parameters:         none
 *
 *  returns:            (Gpoint) - grid pt
 */

Gpoint
get_grid_pt()
{
        return(grid_pt);
}  /* end get_grid_pt */


/*
 *  override_snap
 *
 *  description:        set option to override snapping of user
 *                      selected point to grid
 *
 *  parameters:         none
 */

override_snap()
{
        override = TRUE;
}  /* end override_snap */


/*
 *  resume_snap
 *
 *  description:        unset option to override snapping of user
 *                      selected point to grid
 *
 *  parameters:         none
 */

resume_snap()
{
        override = FALSE;
}  /* end resume_snap */


/*
 *  get_override_state
 *
 *  description:  returns current value of snap option override
 *
 *  parameters: none
 */

get_override_state()
{
        return(override);
}  /* end get_override_state */


/*
 *  draw_grid
 *
 *  description:        draws grid including activating window
 *                      and setting aspect flags
 *
 *  parameters:         none
 */


draw_grid()
{
        Gfloat width;
        Gpoint pt;
        Gpoint window[2];
        int no_x_pts, no_y_pts;
        int i,j;
        int idx;
        Gpoint *pts;
        Gmkbundl repr;

        if (screen_tbl[find_screen_tbl_idx(PICT_AREA)].is_active 
                != TRUE)

                return;

        push_curr_trans();
        activate(PICT_AREA);
        set_aspect_flags(GBUNDLED);

        idx = get_grid_color();
        width = get_grid_width();
        pt = get_grid_pt();
        get_window(PICT_AREA,window);

        pt.x -= floor((pt.x - window[MIN].x) / width) * width;
        pt.y -= floor((pt.y - window[MIN].y) / width) * width;

        no_x_pts = floor((window[MAX].x - window[MIN].x) / width)
                + 1;
        no_y_pts = floor((window[MAX].y - window[MIN].y) / width)
                + 1;

        pts = (Gpoint *) calloc((unsigned) no_x_pts,
                sizeof(Gpoint));

        for (i=0; i<no_x_pts; i++)
                pts[i].x = pt.x + i * width;

        repr.type = GMK_PLUS;
        repr.size = 0.3;
        repr.color = idx;
        gsetmarkerrep(ws_id,GRID_MARKER,&repr);

        gsetmarkerind(GRID_MARKER);
        for (i=0; i<no_y_pts; i++)
        {
                pts[0].y = pt.y + i * width;
                for (j=0; j<no_x_pts; j++)
                        pts[j].y = pts[0].y;
                gpolymarker(no_x_pts,pts);
        }
        free((char *) pts);

        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();
}  /* end draw_grid */


/*
 *  erase_grid
 *
 *  description:        draws grid including activating window
 *                      and setting aspect flags
 *
 *  parameters:         none
 */


erase_grid()
{
        Gfloat width;
        Gpoint pt;
        Gpoint window[2];
        Gpoint box[4];
        int no_x_pts, no_y_pts;
        int i,j;
        int idx;
        Gpoint *pts;
        Gmkbundl repr;

        if (screen_tbl[find_screen_tbl_idx(PICT_AREA)].is_active 
                != TRUE)

                return;

        else if (get_update_mode() != QUICK_UPDATE)
        {
                erase_area(PICT_AREA);
                push_curr_trans();
                activate(PICT_AREA);
                set_aspect_flags(GBUNDLED);

                /* redraw window boundary */

                get_window(PICT_AREA,window);
                load_box(box,window[MIN].x,window[MAX].x,
                        window[MIN].y,window[MAX].y);
                gsetfillind(BGDCLR_SOLID);
                gfillarea(4,box);
                gsetfillind(WHITE_HOLLOW);
                gfillarea(4,box);

                /* redraw objects */

                set_aspect_flags(GINDIVIDUAL);
                draw_objects();
                pop_curr_trans();
        }
        else
        {
                push_curr_trans();
                activate(PICT_AREA);
                set_aspect_flags(GBUNDLED);

                idx = get_bgdclr();
                width = get_grid_width();
                pt = get_grid_pt();
                get_window(PICT_AREA,window);

                pt.x -= floor((pt.x - 
                        window[MIN].x) / width) * width;
                pt.y -= floor((pt.y - 
                        window[MIN].y) / width) * width;

                no_x_pts = floor((window[MAX].x - window[MIN].x)
                        / width) + 1;
                no_y_pts = floor((window[MAX].y - window[MIN].y)
                        / width) + 1;

                pts = (Gpoint *) calloc((unsigned) no_x_pts,
                        sizeof(Gpoint));

                for (i=0; i<no_x_pts; i++)
                        pts[i].x = pt.x + i * width;

                repr.type = GMK_PLUS;
                repr.size = 0.3;
                repr.color = idx;
                gsetmarkerrep(ws_id,GRID_MARKER,&repr);

                gsetmarkerind(GRID_MARKER);
                for (i=0; i<no_y_pts; i++)
                {
                        pts[0].y = pt.y + i * width;
                        for (j=0; j<no_x_pts; j++)
                                pts[j].y = pts[0].y;
        
                        /* points must be drawn in small
                        groups due to limits on the 
                        size of data packets */
        
                        gpolymarker(no_x_pts,pts);
                }
                free((char *)pts);

                set_aspect_flags(GINDIVIDUAL);
                pop_curr_trans();
        }
}  /* end erase_grid */



/*
 *  snap_to_grid
 *
 *  description:        converts point to grid point
 *                      nearest to actual point.
 *
 *  parameters:         (Gpoint *) point to convert
 */

snap_to_grid(pt)
Gpoint *pt;
{
        int d;
        Gpoint gridpt;
        Gfloat width;

        gridpt = get_grid_pt();
        width = get_grid_width();

        d = abs(pt->x - gridpt.x) / grid_width;

        if (pt->x < (gridpt.x + width / 2.0))
                pt->x = gridpt.x;
        else if ((pt->x - d * width) < ((d + 1) * width - pt->x))
                pt->x = d * width;
        else
                pt->x = (d+1) * width;

        d = abs(pt->y - gridpt.y) / grid_width;

        if (pt->y < (gridpt.y + width / 2.0))
                pt->y = grid_pt.y;
        else if ((pt->y - d * width) < ((d + 1) * width - pt->y))
                pt->y = d * width;
        else
                pt->y = (d+1) * width;
}  /* end snap_to_grid */


static int grid_color_state = WHITE_GRID;

static char *grid_color_text[] =
{
        "WHITE",
        "GRID",
        "GREY",
        "GRID",
        "BLACK",
        "GRID"
};


/*
 *  init_grid_color_popup
 *
 *  description:        displays popup box and text
 *
 *  parameters:         popup (POPUP *) - grid_color popup
 */

init_grid_color_popup(popup)
POPUP *popup;
{
        char **ln_ptr;
        int i;

        ln_ptr = grid_color_text;
        for (i=0; i<(2 * grid_color_state); i++)
                ln_ptr++;
        display_popup(popup,2,ln_ptr);

}  /* end init_grid_color_popup */


/*
 *  switch_grid_color_state
 *
 *  description:        advances popup state to next state and
 *                      redraws popup
 *
 *  parameters:         popup (POPUP *) - grid_color popup
 *                      pt (Gpoint) - unused
 */

switch_grid_color_state(popup,pt)
POPUP *popup;
Gpoint pt;
{
        Gcobundl rgb;

        switch_popup_state(&grid_color_state,3,2,grid_color_text,popup->extent);
        switch (grid_color_state)
        {
                case WHITE_GRID:        get_color(white,&rgb);
                                        set_color(grid_color,&rgb);
                                        break;

                case GREY_GRID:         get_color(grey,&rgb);
                                        set_color(grid_color,&rgb);
                                        break;

                case BLACK_GRID:        get_color(black,&rgb);
                                        set_color(grid_color,&rgb);
                                        break;
        }
}  /* end switch_grid_color_state */


/*
 *  save_old_grid
 *
 *  description:        saves old grid description
 *                      so that grid can be restored
 *                      if user wants to undo changes
 *
 *  parameters:         none
 */


save_old_grid()
{
        undo = TRUE;
        old_grid_pt = grid_pt;
        old_grid_width = grid_width;
}  /* end save old grid */


/*
 *  move_grid_init
 *
 *  description:        initializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - move_grid menu item
 */

move_grid_init(menu_item)
MENU_ITEM *menu_item;
{
        set_currmitem(menu_item);
        hilite(menu_item->key);
        push_curr_trans();
        activate(PICT_AREA);
        override_snap();
        prompt(30);
}  /* end move_grid_init */     


/*
 *  move_grid_restart
 *
 *  description:        reintializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - move_grid menu item
 */

move_grid_restart(menu_item)
MENU_ITEM *menu_item;
{
        if (eq(get_newmitem(),ADJUST_GRID_UNDO))
                set_currmitem(menu_item);
        else
                prompt(30);
}  /* end move_grid_restart */  


/*
 *  move_grid_exec
 *
 *  description:        moves grid to new location
 *
 *  parameters:         pt (Gpoint) - pt selected
 */

move_grid_exec(pt)
Gpoint pt;
{
        save_old_grid();
        erase_grid();
        grid_pt = pt;
        draw_grid();
        prompt(30);
}  /* end move_grid_exec */


/*
 *  move_grid_cleanup
 *
 *  description:        cleans up move_grid menu item
*
*  parameters:          menu_item (MENU_ITEM *) - move_grid menu item
*/

move_grid_cleanup(menu_item)
MENU_ITEM *menu_item;
{
if (!eq(get_newmitem(),ADJUST_GRID_UNDO))
{
        pop_curr_trans();
        unhilite(menu_item->key);
        mitem_in_progress = (MENU_ITEM *) NULL;
        resume_snap();
}
else
{
        mitem_in_progress = menu_item;
}
set_currmitem((MENU_ITEM *) NULL);
}  /* end move_grid_cleanup */


BOOLEAN first_point_picked;

/*
 *  resize_grid_init
 *
 *  description:        initializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - resize_grid menu item
 */

resize_grid_init(menu_item)
MENU_ITEM *menu_item;
{
        set_currmitem(menu_item);
        hilite(menu_item->key);
        push_curr_trans();
        activate(PICT_AREA);
        first_point_picked = FALSE;
        override_snap();
        prompt(31);
}  /* end resize_grid_init */   


/*
 *  resize_grid_restart
 *
 *  description:        reintializes menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - resize_grid menu item
 */

resize_grid_restart(menu_item)
MENU_ITEM *menu_item;
{
        if (eq(get_newmitem(),ADJUST_GRID_UNDO))
                set_currmitem(menu_item);
        else
                prompt(31);
        first_point_picked = FALSE;
}  /* end resize_grid_restart */        


/*
 *  resize_grid_exec
 *
 *  description:        resizes grid to new location
 *
 *  parameters:         pt (Gpoint) - pt selected
 */

resize_grid_exec(pt)
Gpoint pt;
{
        static Gpoint first_pt;
        Gfloat dx, dy;

        if (!first_point_picked)
        {
                snap_to_grid(&pt);
                first_pt = pt;
                first_point_picked = TRUE;
                prompt(32);
        }
        else
        {
                save_old_grid();
                erase_grid();
                dx = abs(first_pt.x - pt.x);
                dy = abs(first_pt.y - pt.y);
                grid_width = max2(dx,dy);
                draw_grid();
                first_point_picked = FALSE;
                prompt(31);
        }
}  /* end resize_grid_exec */


/*
 *  resize_grid_cleanup
 *
 *  description:        cleans up resize_grid menu item
 *
 *  parameters:         menu_item (MENU_ITEM *) - resize_grid menu item
 */

resize_grid_cleanup(menu_item)
MENU_ITEM *menu_item;
{
        if (!eq(get_newmitem(),ADJUST_GRID_UNDO))
        {
                pop_curr_trans();
                unhilite(menu_item->key);
                mitem_in_progress = (MENU_ITEM *) NULL;
                resume_snap();
        }
        else
        {
                mitem_in_progress = menu_item;
        }
        set_currmitem((MENU_ITEM *) NULL);
}  /* end resize_grid_cleanup */


/*
 *  adjust_grid_undo
 *
 *  description:        undoes last alteration to grid size or location
 *
 *  parameters:         menu_item (MENU_ITEM *) - undo menu item
 */

adjust_grid_undo(menu_item)
MENU_ITEM *menu_item;
{
        Gpoint temppt;
        Gfloat tempwd;

        if (undo == TRUE)
        {
                hilite(menu_item->key);
                erase_grid();

                temppt = grid_pt;
                grid_pt = old_grid_pt;
                old_grid_pt = temppt;

                tempwd = grid_width;
                grid_width = old_grid_width;
                old_grid_width = tempwd;

                draw_grid();
                unhilite(menu_item->key);
        }

        /* restart menu item which was in progress when undo was
           picked */

        if (mitem_in_progress != (MENU_ITEM *) NULL)
        {
                menu_item_func_tbl[find_menu_item_func_tbl_idx(
                        mitem_in_progress->key)].restart(
                        mitem_in_progress);
        }
}  /* end adjust_grid_undo */


/*
 *  adjust_grid
 *
 *  description:        init_exec routine for adjust_grid menu item
 *                      activates adjust_grid menu only if grid
 *                      on visible
 *
 *  parameters:         menu_item (MENU_ITEM *) - adjust_grid menu item
 */

adjust_grid(menu_item)
MENU_ITEM *menu_item;
{
        if (get_grid_state() == GRID_OFF)
                reprompt(33);
        else
                display_child_menu(menu_item);
}  /* end adjust_grid */


/*
 *  init_grid_menu
 *
 *  description:        init routine for adjust grid menu
 *
 *  parameters:         area (AREA) - menu area
 *                      menu (MENU *) - adjust grid menu
 */

init_grid_menu(area,menu)
AREA area;
MENU * menu;
{
        undo = FALSE;
        mitem_in_progress = (MENU_ITEM *) NULL;
        display_menu_n_popups(area,menu);
}  /* end init_grid_menu */
