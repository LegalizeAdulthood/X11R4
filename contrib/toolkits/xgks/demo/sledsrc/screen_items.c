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
 *      manage_input
 *      signal_done
 *      picture_area_exec
 *      palette_area_exec
 *      popup_area_exec
 *      get_newmitem
 *      menu_area_exec
 *      get_popup_from_pt
 *      get_menu_item_from_pt
 *      erase_area
 *      set_active_clr_area
 *      draw_screen_background
 */

#include <stdlib.h>

#include "screen_items.h"

#include "defs.h"
#include "popup.h"
#include "key.h"
#include "draw.h"
#include "menu.h"
#include "functbl.h"
#include "trans.h"
#include "draw_palette.h"
#include "prompt.h"
#include "grid.h"
#include "main_menu.h"
#include "palette.h"
#include "dummies.h"

void select_pt( IDX *transno, Gpoint *pt);

POPUP * get_popup_from_pt( Gpoint pt);
void set_active_clr_area( AREA clr_area);

/*
 * dynamic table of current popups
 */

POPUP *currpoptbl;


/*
 *  currently active menus and menu item
 */


MENU_ITEM *currmitem = (MENU_ITEM *) NULL;      /* currently active
                                                   menu item */


BOOLEAN done = FALSE;                   /* activated by menu item 
                                           QUIT */

AREA active_clr_area = FGDCLR_AREA;

/*
 *  manage_input
 *
 *  description:        main routine which accepts input from
 *                      user and calls appropriate routine
 *
 *  parameters:         none
 */

void manage_input(void)
{
        AREA area;                      /* identifies window */
        int transno;                    /* trans no of window 
                                           selected */
        Gpoint pt;                      /* point selected */
        IDX idx;                        /* screen_tbl index */
        MENU *pri_menu;                 /* ptr to menu in
                                           PRI_MENU_AREA */

        AREA get_area_from_transno();   /* finds area from
                                           transno and pt */

        /* display_primary_menu */

        pri_menu = get_menu_from_area(PRI_MENU_AREA);
        idx = find_menu_func_tbl_idx(pri_menu->key);
        (*(menu_func_tbl[idx].init))(PRI_MENU_AREA,pri_menu);

        set_active_clr_area(FGDCLR_AREA);

        /* manage input */

        prompt(2);

        while (!done)
        {
                select_pt(&transno,&pt);
                area = get_area_from_transno(transno);
                if (eq(area,PICT_AREA) && (get_grid_state() == SNAP_ON)
                        && !get_override_state())

                        snap_to_grid(&pt);

                /* if screen area exists and is active 
                   execute associated function 
                   else, reprompt */


                if (area != (AREA) NULL)
                {
                        idx = find_screen_tbl_idx(area);
                        if ((screen_tbl[idx].is_active == TRUE)
                                && (screen_tbl[idx].exec != NULL))
                        {
                                (*(screen_tbl[idx].exec))(area, transno, pt);
                        }
                        else
                        {
                                reprompt(1);
                        }
                        free(area);
                }
                else
                        reprompt(1);
        }

}  /* end manage_input */


/*
 *  signal_done
 *
 *  description:        signals program to stop prompting user for input
 *
 *  parameters:         none
 */

void signal_done(void)
{
        done = TRUE;
}  /* end signal_done */


/*
 *  picture_area_exec
 *
 *  description:        calls appropriate routine and
 *                      passes pt or object
 *
 *  parameters:         area (AREA) - area selected by user
 *                      transno (IDX) - trans no picked
 *                      pt (Gpoint) - pt picked
 */

void picture_area_exec( AREA area, IDX transno, Gpoint pt)
{
        IDX idx;                        /* index of currmitem in 
                                           menu_item_func_tbl */

        if (currmitem != (MENU_ITEM *) NULL)
        {
                idx = find_menu_item_func_tbl_idx(currmitem->key);
                (*(menu_item_func_tbl[idx].exec))(pt);
        }
        else
{
                reprompt(1);
}

}  /* end picture_area_exec */


/*
 *  palette_area_exec
 *
 *  description:        calls set_fgdclr and passes pt or object
 *
 *  parameters:         area (AREA) - area selected by user
 *                      transno (IDX) - trans no picked
 *                      pt (Gpoint) - pt picked
 */

void palette_area_exec( AREA area, IDX transno, Gpoint pt)
{
        /* entry in palette must be calculated */

        if (eq(active_clr_area,FGDCLR_AREA))
                set_fgdclr(get_clr_idx_from_pt(pt));
        else if (eq(active_clr_area,BGDCLR_AREA))
                set_bgdclr(get_clr_idx_from_pt(pt));
        else  /* (eq(active_clr_area,FILLCLR_AREA)) */
                set_fillclr(get_clr_idx_from_pt(pt));

}  /* end palette_area_exec */


/*
 *  fgdclr_area_exec
 *
 *  description:        sets active_clr_area to FGDCLR_AREA
 *
 *  parameters:         area (AREA) - area selected by user
 *                      transno (IDX) - trans no picked
 *                      pt (Gpoint) - pt picked
 */

void fgdclr_area_exec( AREA area, IDX transno, Gpoint pt)
{
        set_active_clr_area(FGDCLR_AREA);

}  /* end fgdclr_area_exec */


/*
 *  bgdclr_area_exec
 *
 *  description:        sets active_clr_area to FGDCLR_AREA
 *
 *  parameters:         area (AREA) - area selected by user
 *                      transno (IDX) - trans no picked
 *                      pt (Gpoint) - pt picked
 */

void bgdclr_area_exec( AREA area, IDX transno, Gpoint pt)
{
        set_active_clr_area(BGDCLR_AREA);

}  /* end bgdclr_area_exec */


/*
 *  fillclr_area_exec
 *
 *  description:        sets active_clr_area to FGDCLR_AREA
 *
 *  parameters:         area (AREA) - area selected by user
 *                      transno (IDX) - trans no picked
 *                      pt (Gpoint) - pt picked
 */

void fillclr_area_exec( AREA area, IDX transno, Gpoint pt)
{
        set_active_clr_area(FILLCLR_AREA);

}  /* end fillclr_area_exec */


/*
 *  popup_area_exec
 *
 *  description:        calls appropriate popup exec routine 
 *
 *  parameters:         area (AREA) - area selected by user
 *                      transno (IDX) - trans no picked
 *                      pt (Gpoint) - pt picked
 */

void popup_area_exec( AREA area, IDX transno, Gpoint pt)
{
        POPUP *popup;                   /* popup selected */
        IDX idx;                        /* popup_func_tbl index
                                           for selected popup */

        popup = get_popup_from_pt(pt);

        if (popup != (POPUP *) NULL)
        {
                idx = find_popup_func_tbl_idx(popup->key);
                (*(popup_func_tbl[idx].exec))(popup,pt);
        }

}  /* end popup_area_exec */


MENU_ITEM *newmitem;            /* new menu item */


/*
 *  get_newmitem
 *
 *  description:        returns pointer to newmitem key
 *                      this variable should NOT be changed
 *
 *  parameters:         none
 *
 *  returns:            (KEY) - newmitem's key
 */

KEY get_newmitem(void)
{
        if (newmitem == (MENU_ITEM *) NULL)
                return((KEY) NULL);
        else
                return(newmitem->key);
}  /* end get_newmitem */


/*
 *  menu_area_exec
 *
 *  description:        cleans up old menu item and initiates new
 *
 *  parameters:         area (AREA) - area selected by user
 *                      transno (IDX) - trans no picked
 *                      pt (Gpoint) - pt picked
 */

void menu_area_exec( AREA area, IDX transno, Gpoint pt)
{
        IDX idx;                        /* index of menu item funcs */
        MENU *menu;                     /* menu containing item */

        MENU_ITEM *parent;              /* parent menu item of
                                           currmitem */
        MENU_ITEM *get_menu_item_from_pt();

        /* determine which menu item, newmitem, was selected */
        
        menu = get_menu_from_area(area);
        if (menu != (MENU *) NULL)
        {
                parent = get_parent(menu);
                newmitem = get_menu_item_from_pt(area,pt);
        }
        else
        {
                reprompt(1);
                return;
        }

        if (newmitem == (MENU_ITEM *) NULL)
        {
                reprompt(1);
                return;
        }

        if (currmitem == (MENU_ITEM *) NULL) 
        { 
                idx = find_menu_item_func_tbl_idx(newmitem->key);
                if (menu_item_func_tbl[idx].init != NULL)
                        (*(menu_item_func_tbl[idx].init))(newmitem); 
        }
        else if (currmitem == newmitem)
        {
                idx = find_menu_item_func_tbl_idx(currmitem->key);
                if (menu_item_func_tbl[idx].restart != NULL)
                {
                        (*(menu_item_func_tbl[idx].restart))(newmitem);
                }
        }
        else if (parent == newmitem)
        {
                idx = find_menu_item_func_tbl_idx(currmitem->key);
                if (menu_item_func_tbl[idx].cleanup != NULL)
                        (*(menu_item_func_tbl[idx].cleanup))(currmitem);
                idx = find_menu_item_func_tbl_idx(newmitem->key);
                if (menu_item_func_tbl[idx].restart != NULL)
                        (*(menu_item_func_tbl[idx].restart))(newmitem);
        }
        else  /* a different menu item has been picked */
        {
                idx = find_menu_item_func_tbl_idx(currmitem->key);
                if (menu_item_func_tbl[idx].cleanup != NULL)
                        (*(menu_item_func_tbl[idx].cleanup))(currmitem);
                idx = find_menu_item_func_tbl_idx(newmitem->key);
                if (menu_item_func_tbl[idx].init != NULL)
                        (*(menu_item_func_tbl[idx].init))(newmitem);
        }

}  /* end menu_area_exec */


/*
 *  get_popup_from_pt
 *
 *  description:        finds popup corresponding to pt
 *
 *  parameters:         pt (Gpoint) - pt corresponding to popup
 *
 *  returns:            tbl_ptr (POPUP *) - popup selected
 */

POPUP * get_popup_from_pt( Gpoint pt)
{
        POPUP *tbl_ptr;

        /* find popup in tbl */

        for (tbl_ptr = currpoptbl; (!(in_bounds_delta(tbl_ptr->extent,pt,0.0)) 
                && tbl_ptr->next != (POPUP *) NULL);
                tbl_ptr = tbl_ptr->next);

        if (!(in_bounds_delta(tbl_ptr->extent,pt,0.0)))
                return((POPUP *) NULL);
        else
                return(tbl_ptr);

};  /* get_popup_from_pt */


/*
 *  get_menu_item_from_pt
 *
 *  description:        finds menu item from pickid
 *
 *  parameters:         menu (MENU *) - ptr to menu
 *                      pt (PICKID) - pick number of menu item
 */

MENU_ITEM *get_menu_item_from_pt( AREA area, Gpoint pt)
{
        MENU *menu;
        Gpoint window[2];               /* window[MIN] = (xmin,ymin)
                                           window[MAX] = (xmax,ymax) */
        Gfloat h;                       /* ht of menu item */
        Gfloat d;                       /* distance from ymax to pt */
        MENU_ITEM *menu_item_ptr;
        int i;

        menu = get_menu_from_area(area);

        get_window(area,window);
        h = (window[MAX].y - window[MIN].y)
                / max_num_menu_entries;
        d = window[MAX].y - pt.y;

        if (menu->noitems < ((int) (d / h) ))
                menu_item_ptr = (MENU_ITEM *) NULL;
        else
        {
                menu_item_ptr = menu->item;
                for (i=0; i< ((int) (d/h) - 1); i++)
                        menu_item_ptr++;
        }

        return(menu_item_ptr);
};  /* end get_menu_item_from_pt */


/*
 *  erase_area
 *
 *  description:        erases screen area by drawing solid black box
 *                      over area. does NOT free transformation
 *
 *  parameters:         area (AREA) - name of area to erase
 *
 */

void erase_area( AREA area)
{
        Gpoint window[2];                       /* area world window */
        Gpoint box[4];                          /* pts of rectangle 
                                                   bounding window */

        push_curr_trans();
        activate(area);
        set_aspect_flags(GBUNDLED);
        get_window(area,window);
        load_box(box,window[MIN].x,
                window[MAX].x,
                window[MIN].y,
                window[MAX].y);

        gsetfillind(BLACK_SOLID);
        gfillarea(4,box);
        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();
}  /* end erase_area */


/*
 *  set_active_clr_area
 *
 *  description:        sets active clr area to clr_area
 *
 *  parameters:         clr_area (AREA) - clr area
 */

void set_active_clr_area( AREA clr_area)
{
        if (!eq(active_clr_area,clr_area))
        {
                if (active_clr_area != (AREA) NULL)
                {
                        draw_clr_area(active_clr_area,FALSE);
                        free(active_clr_area);
                }
                active_clr_area = (AREA) calloc((unsigned) 
                        (strlen(clr_area) + 1), sizeof(char));
                (void) strcpy(active_clr_area,clr_area);
                draw_clr_area(active_clr_area,TRUE);
        }
}  /* end set_active_clr_area */


/*
 *  draw_screen_background
 *
 *  description:        draws black box over screen
 *
 *  parameters:         none
 */

void draw_screen_background(void)
{
        Gpoint box[4];

        push_curr_trans();
        gselntran(DEFAULT_TRANS);

        set_aspect_flags(GBUNDLED);
        load_box(box,0.0,1.0,0.0,1.0);
        gsetfillind(BLACK_SOLID);
        gfillarea(4,box);
        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();
}  /* end init_screen */
