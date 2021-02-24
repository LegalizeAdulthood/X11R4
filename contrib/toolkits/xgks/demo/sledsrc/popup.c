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
 * this file includes:
 *      add_to_currpoptbl
 *      rm_from_currpoptbl
 *      assign_popup_extent
 *      display_popup_area
 *      display_popup_text
 *      display_popup
 *      display_number_popup
 *      switch_popup_state
 *      switch_number_popup_state
 *      draw_high_order_digit
 *      draw_low_order_digit
 *      erase_popup
 *      display_popups
 *      erase_popups
 */

#include <xgks.h>

#include <stdlib.h>

#include "popup.h"

#include "key.h"
#include "draw.h"
#include "color.h"
#include "screen_items.h"
#include "trans.h"
#include "popup.h"
#include "functbl.h"
#include "changes.h"
#include "prompt.h"

void draw_high_order_digit( Gpoint *extent, int digit, IDX clrno);
void draw_low_order_digit( Gpoint *extent, int digit, IDX clrno);

/*
 *  add_to_currpoptbl
 *
 *  description:        adds linked list of popups to currpoptbl
 *
 *  parameters:         head_popup (POPUP *) - head of popup list
 */

void add_to_currpoptbl( POPUP *head_popup)
{
        POPUP *tbl_ptr;

        if (currpoptbl == (POPUP *) NULL)
                currpoptbl = head_popup;
        else
        {
                tbl_ptr = currpoptbl;
                while (tbl_ptr->next != (POPUP *) NULL)
                        tbl_ptr = tbl_ptr->next;
        
                tbl_ptr->next = head_popup;
        }

}  /* add_to_currpoptbl */


/*
 *  rm_from_currpoptbl
 *
 *  description:        removes linked list of popups from
 *                      end of currpoptbl beginning with head_popup
 *
 *  parameters:         head_popup (POPUP *) - head of popup list
 *                              to remove
 */

void rm_from_currpoptbl( POPUP *head_popup)
{
        POPUP *tbl_ptr;

        if (currpoptbl == head_popup)
                currpoptbl = (POPUP *) NULL;

        else if (currpoptbl != (POPUP *) NULL)
        {
                tbl_ptr = currpoptbl;
                while ((tbl_ptr->next != head_popup) &&
                        (tbl_ptr->next != (POPUP *) NULL))
                {
                        tbl_ptr = tbl_ptr->next;
                }

                tbl_ptr->next = (POPUP *) NULL;
        }

}  /* rm_from_currpoptbl */


/*
 *  assign_popup_extent
 *
 *  description:        assigns extent to popup based on
 *                      location in currpoptbl
 *
 *  parameters:         popup (POPUP *) - popup whose extent is to
 *                              be assigned
 */

void assign_popup_extent( POPUP *popup)
{
        POPUP *tbl_ptr;                 /* ptr into currpoptbl */
        int i;                          /* posiion in currpoptbl */
                                        /* initial position is 1 */
        Gpoint window[2];               /* popup area window */
        Gfloat popup_width;             /* width of popup */

        /* determine position of popup in currpoptbl */

        i = 1;
        tbl_ptr = currpoptbl;
        while (tbl_ptr != popup)
        {
                tbl_ptr = tbl_ptr->next;
                i++;
        }

        /* determine x-extent */

        get_window(POPUP_AREA,window);
        popup_width = 12 * (window[MAX].x - window[MIN].x)
                / (MAX_NUM_POPUPS *12 + MAX_NUM_POPUPS - 1);
        popup->extent[MIN].x = window[MAX].x - i * popup_width
                - (i-1) * (popup_width / 12);
        popup->extent[MAX].x = popup->extent[MIN].x + popup_width;

        /* determine y-extent */

        popup->extent[MIN].y = window[MIN].y;
        popup->extent[MAX].y = window[MAX].y;

}  /* end assign_popup_extent */


/*
 *  display_popup_area
 *
 *  description:        displays popup_area
 *
 *  parameters:         none
 */

void display_popup_area(void)
{
        Gpoint window[2];               /* popup area window */
        Gpoint box[4];                  /* pts of window */
        POPUP *tbl_ptr;                 /* ptr to popup list */
        IDX idx;                        /* popup_func_tbl index */

        push_curr_trans();
        alloc_trans(POPUP_AREA);
        activate(POPUP_AREA);

        set_aspect_flags(GBUNDLED);

        get_window(POPUP_AREA,window);
        load_box(box,window[MIN].x,window[MAX].x,
                window[MIN].y,window[MAX].y);
        gsetfillind(BLACK_SOLID);
        gfillarea(4,box);

        set_aspect_flags(GINDIVIDUAL);

        /* display all active popups in poptbl */

        for (tbl_ptr = currpoptbl;
                tbl_ptr != (POPUP *) NULL;
                tbl_ptr = tbl_ptr->next)
        {
                idx = find_popup_func_tbl_idx(tbl_ptr->key);
                if (tbl_ptr->is_active == TRUE)
                        (*(popup_func_tbl[idx].init))(tbl_ptr);
        }

        pop_curr_trans();


}  /* end display_popup_area */


/*
 *  display_popup_text
 *
 *  description:        display text centered within popup.
 *
 *  parameters:         nlns (int) - no of lines to display
 *                      lines (char **) - text lines
 *                      extent (Gpoint *) - popup extent
 *                      clrno (IDX) - index of color to use
 */

void display_popup_text(int nlns, char **lines, Gpoint extent[], IDX clrno)
{
        Gpoint center;                  /* popup center */

        center.x = (extent[MIN].x + extent[MAX].x) / 2;
        center.y = (extent[MIN].y + extent[MAX].y) / 2;
        center_text(nlns,lines,&center,clrno,POPUP_TEXT);

}  /* end display_popup_text */


/*
 *  erase_popup_text
 *
 *  description:        erase text centered within popup.
 *
 *  parameters:         extent (Gpoint *) - popup extent
 */

void erase_popup_text( Gpoint extent[])
{
        Gpoint box[5];                  /* boundary of extent */

        load_box(box,extent[MIN].x,extent[MAX].x,extent[MIN].y,
                extent[MAX].y);

        gsetfillind(GREY_SOLID);
        gfillarea(4,box);
        gsetfillind(WHITE_HOLLOW);
        gfillarea(4,box);

}  /* end erase_popup_text */


/*
 *  display_popup
 *
 *  description:        display entire popup (i.e. box and text)
 *
 *                      REMEMBER: popup must be in currpoptbl before
 *                      before calling display_popup
 *
 *  parameters:         popup (POPUP *) - popup to display
 *                      nlns (int) - no of lines to display
 *                      lines (char **) - text lines
 */

void display_popup( POPUP *popup, int nlns, char **lines)
{
        Gpoint box[4];                  /* boundary of popup */

        push_curr_trans();
        activate(POPUP_AREA);

        /* draw popup box */

        set_aspect_flags(GBUNDLED);

        assign_popup_extent(popup);
        load_box(box,popup->extent[MIN].x,
                popup->extent[MAX].x,
                popup->extent[MIN].y,
                popup->extent[MAX].y);
        gsetfillind(GREY_SOLID);
        gfillarea(4,box);
        gsetfillind(WHITE_HOLLOW);
        gfillarea(4,box);

        /* draw text */

        display_popup_text(nlns,lines,popup->extent,white);

        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();

        popup->is_active = TRUE;

}  /* end display_popup */


/*
 *  display_number_popup
 *
 *  description:        displays popup with numbers
 *
 *  parameters:         popup (POPUP *) - popup to display
 *                      text (char *) - pointer to text label
 *                      state (int) - popup state (i.e value)
 */

void display_number_popup(POPUP *popup, char *text, int state)
{
        Gpoint center;
        Gpoint box[4];

        push_curr_trans();
        activate(POPUP_AREA);

        /* draw popup box */

        set_aspect_flags(GBUNDLED);

        assign_popup_extent(popup);
        load_box(box,popup->extent[MIN].x,
                popup->extent[MAX].x,
                popup->extent[MIN].y,
                popup->extent[MAX].y);
        gsetfillind(GREY_SOLID);
        gfillarea(4,box);
        gsetfillind(WHITE_HOLLOW);
        gfillarea(4,box);

        /* draw text */

        center.x = (popup->extent[MIN].x +
                popup->extent[MAX].x) / 2.0;
        center.y = (popup->extent[MAX].y - popup->extent[MIN].y) 
                * 1.0 / 5.0 + popup->extent[MIN].y;

        center_text(1,&text,&center,white,POPUP_TEXT);

        draw_high_order_digit(popup->extent,(int) (state / 10.0),white);
        draw_low_order_digit(popup->extent,(int) state % 10,white);

        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();

        popup->is_active = TRUE;
}  /* end display_number_popup */


/*
 *  display_header_popup
 *
 *  description:        displays popup with headers
 *
 *  parameters:         popup (POPUP *) - popup to display
 *                      header_text (char *) - header string
 *                      text (char **) - pointer to text label
 */

void display_header_popup( POPUP *popup, char *header_text, char **text, int nlns)
{
        Gpoint center;
        Gpoint box[4];

        push_curr_trans();
        activate(POPUP_AREA);

        /* draw popup box */

        set_aspect_flags(GBUNDLED);

        assign_popup_extent(popup);
        load_box(box,popup->extent[MIN].x,
                popup->extent[MAX].x,
                popup->extent[MIN].y,
                popup->extent[MAX].y);
        gsetfillind(GREY_SOLID);
        gfillarea(4,box);
        gsetfillind(WHITE_HOLLOW);
        gfillarea(4,box);

        /* draw header_text */

        center.x = (popup->extent[MIN].x
                + popup->extent[MAX].x) / 2.0;
        center.y = (popup->extent[MAX].y 
                - popup->extent[MIN].y) 
                * 5.0 / 6.0 + popup->extent[MIN].y;
        center_text(1,&header_text,&center,white,POPUP_TEXT);

        /* draw text */

        center.y = (popup->extent[MAX].y 
                - popup->extent[MIN].y) 
                / 3.0 + popup->extent[MIN].y;
        center_text(nlns,text,&center,white,POPUP_TEXT);

        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();

        popup->is_active = TRUE;
}  /* end display_header_popup */


/*
 *  switch_popup_state
 *
 *  description:        switches popup state to next state
 *                      and redraws popup
 *
 *  parameters:         state - (ENUM *) current popup state
 *                      nostates (int) - no of popup states
 *                      nlns (int) - no of text lines to display
 *                      lines (char **) - nlns * state is the
 *                              starting addr of lines corresponding to
 *                              state
 *                      extent (Gpoint *) - popup extent
 */

void switch_popup_state(int *state, int nostates, int nlns, char **lines, Gpoint extent[])
{
        char **ln_ptr;                  /* ptr to appropriate lines */
        int i;

        push_curr_trans();
        activate(POPUP_AREA);
        set_aspect_flags(GBUNDLED);

        /* erase old popup text */

        ln_ptr = lines;
        for (i=0; i<(nlns * (*state)); i++)
                ln_ptr++;
        erase_popup_text(extent);

        /* switch state */

        *state = ((*state) + 1) % nostates;

        /* display new popup text */

        ln_ptr = lines;
        for (i=0; i<(nlns * (*state)); i++)
                ln_ptr++;
        display_popup_text(nlns,ln_ptr,extent,white);

        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();

}  /* end switch_popup_state */


/*
 *  switch_number_popup_state
 *
 *  description:        if a digit was picked, that digit is
 *                      incremented
 *
 *  parameters:         extent (Gpoint *) - popup extent
 *                      state (int *) - current popup state
 *                      pt (Gpoint) -pt picked by user
 */

void switch_number_popup_state(Gpoint *extent, int *state, Gpoint pt, int mini, int maxi)
{
        Gfloat x,y;
        int high_order_digit;
        int low_order_digit;
        int min_high_order_digit;
        int min_low_order_digit;
        int max_high_order_digit;
        int max_low_order_digit;

        x = (extent[MIN].x + extent[MAX].x) 
                / 2.0;
        y = (extent[MAX].y - extent[MIN].y) 
                * 2.0 / 3.0 + extent[MIN].y;

        /* determine if either low order or 
           high order digit was picked */

        if (pt.y > y)
        {
                reprompt(1);
                return;
        }

        high_order_digit = (*state / 10);
        low_order_digit = (*state % 10);

        min_high_order_digit = (mini / 10);
        min_low_order_digit = (mini % 10);

        max_high_order_digit = (maxi / 10);
        max_low_order_digit = (maxi % 10);

        if (pt.x < x)
        {
                /* high order digit picked */

                draw_high_order_digit(extent,high_order_digit,grey);
                high_order_digit = (high_order_digit + 1) % 10;
                if (high_order_digit > max_high_order_digit)
                        high_order_digit = min_high_order_digit;
                draw_high_order_digit(extent,high_order_digit,white);

                /* state cannot be zero */

                if ((high_order_digit == min_high_order_digit) 
                        && (low_order_digit < min_low_order_digit))
                {
                        draw_low_order_digit(extent,low_order_digit,grey);
                        low_order_digit = min_low_order_digit;
                        draw_low_order_digit(extent,low_order_digit,white);
                }
                else if ((high_order_digit == max_high_order_digit) 
                        && (low_order_digit > max_low_order_digit))
                {
                        draw_low_order_digit(extent,low_order_digit,grey);
                        low_order_digit = max_low_order_digit;
                        draw_low_order_digit(extent,low_order_digit,white);
                }
        }
        else
        {
                /* low order digit picked */

                draw_low_order_digit(extent,low_order_digit,grey);
                low_order_digit = (low_order_digit + 1) % 10;

                /* state cannot be zero */


                if ((high_order_digit == min_high_order_digit) 
                        && (low_order_digit < min_low_order_digit))
                {
                        low_order_digit = min_low_order_digit;
                }
                else if ((high_order_digit == max_high_order_digit) 
                        && (low_order_digit > max_low_order_digit))
                {
                        low_order_digit = max_low_order_digit;
                }

                draw_low_order_digit(extent,low_order_digit,white);
        }

        *state = high_order_digit * 10 + low_order_digit;
} /* end switch_number_popup_state */


/*
 *  draw_high_order_digit
 *
 *  description:        draws high order digit in desired color
 *
 *  parameters:         extent (Gpoint *) - popup extent
 *                      digit (int) - digit to draw
 *                      clrno (IDX) - clr tbl index
 */


void draw_high_order_digit( Gpoint *extent, int digit, IDX clrno)
{
        char *string;
        Gfloat x;
        Gpoint center;

        push_curr_trans();
        activate(POPUP_AREA);
        set_aspect_flags(GBUNDLED);

        x = (extent[MIN].x + extent[MAX].x) 
                / 2.0;

        /* draw digit */

        center.x = x - (x - extent[MIN].x) / 4.0;
        center.y = x;
        string = (char *) calloc (2,sizeof(char));
        sprintf(string,"%d",digit);
        center_text(1,&string,&center,clrno,POPUP_NUMBER_TEXT);
        free((char *) string);

        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();
}  /* end draw_high_order_digit */


/*
 *  draw_low_order_digit
 *
 *  description:        draws low order digit in desired color
 *
 *  parameters:         extent (Gpoint *) - popup extent
 *                      digit (int) - digit to draw
 *                      clrno (IDX) - clr tbl index
 */


void draw_low_order_digit( Gpoint *extent, int digit, IDX clrno)
{
        char *string;
        Gfloat x;
        Gpoint center;

        push_curr_trans();
        activate(POPUP_AREA);
        set_aspect_flags(GBUNDLED);

        x = (extent[MIN].x + extent[MAX].x) 
                / 2.0;

        /* draw digit */

        center.x = x + (extent[MAX].x - x) / 4.0;
        center.y = x;
        string = (char *) calloc (2,sizeof(char));
        sprintf(string,"%d",digit);
        center_text(1,&string,&center,clrno,POPUP_NUMBER_TEXT);
        free((char *) string);

        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();
}  /* end draw_low_order_digit */


/*
 *  switch_header_popup_state
 *
 *  description:        switches popup state to next state
 *                      and redraws popup
 *
 *  parameters:         state - (ENUM *) current popup state
 *                      nostates (int) - no of popup states
 *                      nlns (int) - no of text lines to display
 *                      lines (char **) - nlns * state is the
 *                              starting addr of lines corresponding to
 *                              state
 *                      extent (Gpoint *) - popup extent
 */

void switch_header_popup_state( int *state, int nostates, int nlns, char **lines, Gpoint extent[])
{
        char **ln_ptr;                  /* ptr to appropriate lines */
        int i;
        Gpoint center;

        push_curr_trans();
        activate(POPUP_AREA);
        set_aspect_flags(GBUNDLED);

        /* erase old popup text */

        ln_ptr = lines;
        for (i=0; i<(nlns * (*state)); i++)
                ln_ptr++;
        center.x = (extent[MIN].x + extent[MAX].x) / 2.0;
        center.y = (extent[MAX].y - extent[MIN].y) 
                / 3.0 + extent[MIN].y;
        center_text(nlns,ln_ptr,&center,grey,POPUP_TEXT);

        /* switch state */

        *state = ((*state) + 1) % nostates;

        /* display new popup text */

        ln_ptr = lines;
        for (i=0; i<(nlns * (*state)); i++)
                ln_ptr++;
        center_text(nlns,ln_ptr,&center,white,POPUP_TEXT);

        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();

}  /* end switch_header_popup_state */


/*
 *  erase_popup
 *
 *  description:        erases popup from screen and sets
 *                      active flag to false.
 *
 *  parameters:         popup (POPUP *) - popup to erase
 */

void erase_popup( POPUP *popup)
{
        Gpoint box[4];          /* pts of popup extent */

        /* erase popup from screen */

        push_curr_trans();
        activate(POPUP_AREA);
        set_aspect_flags(GBUNDLED);

        load_box(box,popup->extent[MIN].x,popup->extent[MAX].x,
                popup->extent[MIN].y,popup->extent[MAX].y);
        gsetfillind(BLACK_SOLID);
        gfillarea(4,box);

        set_aspect_flags(GINDIVIDUAL);
        pop_curr_trans();

        /* note that it is unpickable */

        popup->is_active = FALSE;
}  /* end erase_popup */


/*
 *  display_popups
 *
 *  description:        displays list of popups
 *                      checks for empty list
 *
 *  parameters:         poplst (POPUP *) - list of popups to display
 */

void display_popups( POPUP *poplst)
{
        POPUP *tbl_ptr;
        IDX idx;

        if (poplst != (POPUP *) NULL)
        {
                add_to_currpoptbl(poplst);
                if (screen_tbl[find_screen_tbl_idx(POPUP_AREA)].is_active
                        == TRUE)
                        for (tbl_ptr = poplst;
                                tbl_ptr != (POPUP *) NULL;
                                tbl_ptr = tbl_ptr->next)
                        {
                                idx = find_popup_func_tbl_idx(tbl_ptr->key);
                                (*(popup_func_tbl[idx].init))(tbl_ptr);
                        }
        }
}  /* end display_popups */


/*
 *  erase_popups
 *
 *  description:        erases list of popups and removes them
 *                      from poptbl
 *
 *  parameters:         poplst (POPUP *) - list of popups
 */

void erase_popups( POPUP *poplst)
{
        POPUP *tbl_ptr;
        IDX idx;

        rm_from_currpoptbl(poplst);
        for (tbl_ptr = poplst;
                tbl_ptr != (POPUP *) NULL;
                tbl_ptr = tbl_ptr->next)
        {
                idx = find_popup_func_tbl_idx(tbl_ptr->key);
                (*(popup_func_tbl[idx].cleanup))(tbl_ptr);
        }
}  /* end erase_popups */
