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
 *
 *      adjust_screen_tbl_viewports
 *      get_max_dc_coor
 *      init_trans_tbls
 *      alloc_trans
 *      free_trans
 *      get_area_from_transno
 *      activate
 *      push_curr_trans
 *      pop_curr_trans
 *      find_trans
 *      set_window
 *      set_viewport
 *      get_window
 *      get_viewport
 *      get_ws_trans
 *      is_ident
 */

#include <strings.h>

#include "trans.h"
#include "functbl.h"
#include "defs.h"
#include "ws.h"


/*
 *  routines which manage transformations
 */


AREA *trans_tbl;                        /* tbl of trans and
                                           corresponding areas */
TRANS_STK *stk_ptr = (TRANS_STK *) NULL;        /* ptr to stack of
                                                   saved trans */


/*
 *  adjust_screen_tbl_viewports
 *
 *  description:        adjusts viewport of each screen area so
 *                      that entire screen is used
 *
 *  parameters:         none
 */

adjust_screen_tbl_viewports()
{
        Gfloat xmax_dc,ymax_dc;         /* max DC coordinates */
        Gfloat xmax_ndc,ymax_ndc;       /* max ndc coordinates */
        int i;

        get_max_dc_coor(&xmax_dc,&ymax_dc);
        if (ymax_dc > xmax_dc)
        {
                xmax_ndc = xmax_dc / ymax_dc;
                ymax_ndc = 1.0;
        }
        else
        {
                ymax_ndc = ymax_dc / xmax_dc;
                xmax_ndc = 1.0;
        }

        for (i=0; i<screen_tbl_sz; i++)
        {
                screen_tbl[i].viewport[MIN].x *= xmax_ndc;
                screen_tbl[i].viewport[MAX].x *= xmax_ndc;
                screen_tbl[i].viewport[MIN].y *= ymax_ndc;
                screen_tbl[i].viewport[MAX].y *= ymax_ndc;
        }

}  /* adjust_screen_tbl_viewports */


/*
 *  get_max_dc_coor
 *
 *  description:        finds maximum x and y in DC coordinates
 *
 *  parameters:         x (Gfloat *) - max x in DC coor
 *                      y (Gfloat *) - max y in DC coor
 */

get_max_dc_coor(x,y)
Gfloat *x,*y;                   /* max x and y in DC coor */
{

        Gdspsize dsp;

        ginqdisplayspacesize(ws_type,&dsp);
        *x = dsp.device.x;
        *y = dsp.device.y;

        /* to create 4x5 print  using 8x10film
        *x *= .59;
        *y *= .59; */

}  /* get_max_dc_coor */


/*
 *  init_trans_tbls
 *
 *  description:        initializes trans_stk
 *
 *  parameters:         none
 */

init_trans_tbls()
{
        Gint maximum;                   /* max trans no available */
        int i;

        /* inquire how many transformations are available */

        ginqmaxntrannum(&maximum);

        /* initialize trans_tbl */

        trans_tbl = (AREA *) calloc((unsigned) (maximum + 1),
                sizeof(AREA));
        for (i=0; i<=maximum; i++)
                trans_tbl[i] = (AREA) NULL;

 } /* end init_trans_tbls */


/*
 *  alloc_trans
 *
 *  description:        allocates free norm trans no, if one exists
 *
 *  parameters:         area (AREA) - screen area
 *
 */

alloc_trans(area)
AREA area;
{
        IDX transno;                    /* trans no to be allocated */
        int maximum;                    /* highest numbered trans 
                                           available */
        Gpoint viewport[2];             /* viewport for area */
        Gpoint window[2];               /* window for area */

        if (find_trans(area) == NON_EXISTENT)
        {
                /* inquire how many transformations are available
                   maximum is highest numbered transformation */

                ginqmaxntrannum(&maximum);

                /* find free entry in trans_tbl */

                transno = 0;
                do
                        transno++;
                while ((transno<maximum) &&
                        (trans_tbl[transno] != (AREA) NULL));

                /* enter area into trans_tbl */

                if (trans_tbl[transno] != (AREA) NULL)
                        exit_error("alloc_trans",0);
                else
                {
                        trans_tbl[transno] = (AREA) calloc((unsigned)
                                (strlen(area) + 1), sizeof(char));
                        (void) strcpy(trans_tbl[transno],area);
                }
        
                /* set up normalization transformation */

                get_viewport(area,viewport);
                set_viewport(area,viewport);

                get_window(area,window);
                set_window(area,window);

                /* note that area is active (i.e. on screen */

                gsetviewportinputpri(transno,DEFAULT_TRANS,GHIGHER);

                screen_tbl[find_screen_tbl_idx(area)].is_active = TRUE;
        }

} /* end alloc_trans */



/*
 *  free_trans
 *
 *  description:        free trans corresponding to area
 *
 *  parameters:         area (AREA) - area corresponding to trans to be
 *                                    freed
 */

free_trans(area)
AREA area;                              /* area corresponding
                                           to trans to be freed */
{
        IDX transno;                    /* trans no correponding
                                           to area */

        if (find_trans(area) != NON_EXISTENT)
        {
                transno = find_trans(area);
                gsetviewportinputpri(transno,DEFAULT_TRANS,GLOWER);

                screen_tbl[find_screen_tbl_idx(area)].is_active = FALSE;
                free((char *) trans_tbl[transno]);
                trans_tbl[transno] = (AREA) NULL;
        }

} /* end free_trans */


/*
 *  get_area_trans
 *
 *  description:        returns area of area corresponding
 *                      to trans
 *
 *  parameter:          transno (IDX) - trans no of selected area
 *
 *  returns:            area (AREA) - area of selected area
 */

AREA
get_area_from_transno(transno)
IDX transno;                    /* transno of selected area */
{
        AREA area;              /* selected area */

        area = (AREA) calloc ((unsigned) 
                (strlen(trans_tbl[transno]) + 1), sizeof(char));
        (void) strcpy(area,trans_tbl[transno]);

        return(area);
}  /* end get_area_from_transno */


/*
 *  activate
 *
 *  description:        set trans no corresponding to area as selected
 *                      transformation (for output)
 *
 *  parameters:         area (AREA) - area
 */

activate(area)
AREA area;                              /* screen area */
{
        gselntran(find_trans(area));

}  /* end activate */


/*
 *  push_curr_trans
 *
 *  description:        pushes curr trans on top of trans_stk
 *                      does not alter currently selected norm trans
 *
 *  parameters:         none
 */

push_curr_trans()
{
        TRANS_STK *trans;               /* top stack element */

        /* push currently selected norm trans on stk */

        trans = (TRANS_STK *) calloc(1,sizeof(TRANS_STK));
        ginqcurntrannum(&(trans->transno));
        trans->next = stk_ptr;
        stk_ptr = trans;

}  /* end push_curr_trans */


/*
 *  pop_curr_trans
 *
 *  description:        restores top of trans_stk as current trans
 *
 *  parameters:         none
 */

pop_curr_trans()
{
        TRANS_STK *trans;

        /* if stack is not empty, pop top of stk
           and set curr trans equal to top of stk */

        if (stk_ptr != (TRANS_STK *) NULL)
        {
                trans = stk_ptr;
                stk_ptr = stk_ptr->next;
                gselntran(trans->transno);
                free((char *) trans);
        }

}  /* end pop_curr_trans */


/*
 *  find_trans
 *
 *  description:        find_trans
 *
 *  parameters:         area (AREA) - area to be mapped to trans
 *                                   for future identification
 */

find_trans(area)
AREA area;                              /* screen area */
{
        int transno;                    /* trans no corresponding
                                           to area */
        int maximum;                    /* highest numbered trans 
                                           available */

        /* inquire how many transformations are available
           maximum is highest numbered transformation */

        ginqmaxntrannum(&maximum);

        /* find trans no corresponding to area */

        transno = 0;
        do
                transno++;
        while ((transno < maximum) && !eq(trans_tbl[transno],area));

        if (!eq(trans_tbl[transno],area))
                transno = NON_EXISTENT;

        return(transno);
}  /* end find_trans */


/*
 *  set_viewport
 *
 *  description:        updates screen_tbl and if area
 *                      is currently mapped to actual trans
 *                      gks viewport is reset
 *
 *  parameters:         area (AREA) - associated area 
 *                      viewport (Gpoint *) - new viewport
 */

set_viewport(area,viewport)
AREA area;                              /* viewport to set */
Gpoint *viewport;                       /* viewport[MIN] = (xmin,ymin)
                                           viewport[MAX] = (xmax,ymax) */
{
        IDX idx;                        /* index into screen_tbl */
        IDX transno;                    /* trans corresponding to
                                           screen area */
        Glimit port;

        /* find screen table entry */

        idx = find_screen_tbl_idx(area);

        /* update tbl entry with new viewport value */

        screen_tbl[idx].viewport[MIN].x = viewport[MIN].x;
        screen_tbl[idx].viewport[MIN].y = viewport[MIN].y;
        screen_tbl[idx].viewport[MAX].x = viewport[MAX].x;
        screen_tbl[idx].viewport[MAX].y = viewport[MAX].y;

        /* if transformation allocated, set gks viewport too */

        transno = find_trans(area);
        if (transno != NON_EXISTENT)
                {
                port.xmin = viewport[MIN].x;
                port.xmax = viewport[MAX].x;
                port.ymin = viewport[MIN].y;
                port.ymax = viewport[MAX].y;
                gsetviewport(transno,&port);
                }

}  /* end set_viewport */


/*
 *  set_window
 *
 *  description:        updates screen_tbl and if area
 *                      is currently mapped to actual trans
 *                      gks window is reset
 *
 *  parameters:         area (AREA) - associated area 
 *                      window (Gpoint *) - new window
 */

set_window(area,window)
AREA area;                              /* window to set */
Gpoint *window;                         /* window[MIN] = (xmin,ymin)
                                           window[MAX] = (xmax,ymax) */
{
        IDX idx;                        /* index into screen_tbl */
        IDX transno;                    /* trans corresponding to
                                           screen area */
        Glimit wind;

        /* find screen table entry */

        idx = find_screen_tbl_idx(area);

        /* update tbl entry with new window value */

        screen_tbl[idx].window[MIN].x = window[MIN].x;
        screen_tbl[idx].window[MIN].y = window[MIN].y;
        screen_tbl[idx].window[MAX].x = window[MAX].x;
        screen_tbl[idx].window[MAX].y = window[MAX].y;

        /* if transformation allocated, set gks window too */

        transno = find_trans(area);
        if (transno != NON_EXISTENT)
                {
                wind.xmin = window[MIN].x;
                wind.xmax = window[MAX].x;
                wind.ymin = window[MIN].y;
                wind.ymax = window[MAX].y;
                gsetwindow(transno,&wind);
                }

}  /* end set_window */


/*
 *  get_viewport
 *
 *  description:        get viewport associated with screen area
 *                      returned in variable viewport
 *
 *  parameters:         area (AREA) - screen area
 *                      viewport (Gpoint *) - screen area viewport
 */

get_viewport(area,viewport)
AREA area;                              /* screen area */
Gpoint *viewport;                       /* screen area viewport */
{
        IDX idx;                        /* index into screen_tbl */

        /* find screen table entry */

        idx = find_screen_tbl_idx(area);

        /* copy viewport info to return*/

        viewport[MIN].x = screen_tbl[idx].viewport[MIN].x;
        viewport[MIN].y = screen_tbl[idx].viewport[MIN].y;
        viewport[MAX].x = screen_tbl[idx].viewport[MAX].x;
        viewport[MAX].y = screen_tbl[idx].viewport[MAX].y;

}  /* end get_viewport */



/*
 *  get_window
 *
 *  description:        get window associated with screen area
 *                      returned in variable window
 *
 *  parameters:         area (AREA) - screen area
 *                      window (Gpoint *) - screen area window
 */

get_window(area,window)
AREA area;                              /* screen area */
Gpoint *window;                         /* screen area window */
{
        IDX idx;                        /* index into screen_tbl */

        /* find screen table entry */

        idx = find_screen_tbl_idx(area);

        /* copy window info to return */

        window[MIN].x = screen_tbl[idx].window[MIN].x;
        window[MIN].y = screen_tbl[idx].window[MIN].y;
        window[MAX].x = screen_tbl[idx].window[MAX].x;
        window[MAX].y = screen_tbl[idx].window[MAX].y;

}  /* end get_window */


/*
 *  get_ws_trans
 *
 *  description:        get workstation window and viewport
 *
 *  parameters:         (Gpoint *) wind, view - workstation
 *                      window and viewport
 */

get_ws_trans(wind,view)
Gpoint wind[],view[];
{
        Gwsti tran;

        ginqwstran(ws_id,&tran);
        wind[0].x = tran.current.w.xmin;
        wind[1].x = tran.current.w.xmax;
        wind[0].y = tran.current.w.ymin;
        wind[1].y = tran.current.w.ymax;
        view[0].x = tran.current.v.xmin;
        view[1].x = tran.current.v.xmax;
        view[0].y = tran.current.v.ymin;
        view[1].y = tran.current.v.ymax;

        printf("ws_window: min = (%.2f, %.2f), max = (%.2f, %.2f)\n",
                wind[0].x,wind[0].y,wind[1].x,wind[1].y);
        printf("ws_viewport: min = (%.2f, %.2f), max = (%.2f, %.2f)\n",
                view[0].x,view[0].y,view[1].x,view[1].y);
}  /* end get_ws_trans */


/*
 *  is_ident
 *
 *  description:        tests if trans equals identity trans
 *
 *  parameters:         trans (Gfloat *) - trans to test
 *
 *  returns:            (BOOLEAN) - TRUE if trans is identity trans
 */

is_ident(trans)
Gfloat *trans;
{
        if ((trans[0] == 1.0) && (trans[1] == 0.0)
                && (trans[2] == 0.0) && (trans[3] == 0.0)
                && (trans[4] == 1.0) && (trans[5] == 0.0))

                return(TRUE);
        else
                return(FALSE);
}  /* end is_ident */
