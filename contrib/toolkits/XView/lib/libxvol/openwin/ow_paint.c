#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)ow_paint.c 1.19 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	ow_paint.c Product:	SunView 2.0
 * 
 * Description:
 * 
 * paint routines for openwin
 * 
 */


/*
 * Include files:
 */
#include <xview_private/ow_impl.h>
#include <xview/rectlist.h>


/*
 * Declaration of Functions Defined in This File (in order):
 */

void            openwin_clear_damage();
void            openwin_paint_borders();
void            openwin_hilite_view();
void            openwin_lolite_view();


/******************************************************************/

void
openwin_clear_damage(window, rl)
    Xv_Window       window;
    Rectlist       *rl;
{
    if (rl) {
	pw_writebackground(window, rl->rl_bound.r_left,
			   rl->rl_bound.r_top, rl->rl_bound.r_width,
			   rl->rl_bound.r_height, PIX_CLR);
    }
}

void
openwin_paint_borders(owin_public)
    Openwin         owin_public;
{
    Openwin_view_info *view;
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);

    for (view = owin->views; view != NULL; view = view->next_view) {
	openwin_paint_border(owin_public, view, TRUE);
    }
}


openwin_paint_border(owin_public, view, on)
    Openwin         owin_public;
    Openwin_view_info *view;
    int             on;
{
    Rect            r;
    int             x, y, w, h;
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);
    int             border_width;
    int             stroke = (on) ? 1 : 0;

    border_width = openwin_border_width(owin_public, view->view);
    win_getrect(view->view, &r);
    x = r.r_left - border_width;
    y = r.r_top - border_width;
    w = r.r_width + 2 * border_width - 1;
    h = r.r_height + 2 * border_width - 1;
    xv_vector(owin_public, x + 0, y + 0, x + w, y + 0, PIX_SRC, stroke);
    xv_vector(owin_public, x + 0, y + h, x + w, y + h, PIX_SRC, stroke);
    xv_vector(owin_public, x + 0, y + 0, x + 0, y + h, PIX_SRC, stroke);
    xv_vector(owin_public, x + w, y + 0, x + w, y + h, PIX_SRC, stroke);

    if (view == owin->seln_view && on)
	openwin_hilite_view(owin_public, view);
    else
	openwin_lolite_view(owin_public, view);
}


void
openwin_hilite_view(owin_public, view)
    Openwin         owin_public;
    Openwin_view_info *view;
{
    Rect            r;
    int             x, y, w, h;
    int             border_width = openwin_border_width(owin_public, view->view);

    win_getrect(view->view, &r);
    x = r.r_left - border_width + 1;
    y = r.r_top - border_width + 1;
    w = r.r_width + 2 * border_width - 3;
    h = r.r_height + 2 * border_width - 3;
    xv_vector(owin_public, x + 0, y + 0, x + w, y + 0, PIX_SRC, 1);
    xv_vector(owin_public, x + 0, y + h, x + w, y + h, PIX_SRC, 1);
    xv_vector(owin_public, x + 0, y + 0, x + 0, y + h, PIX_SRC, 1);
    xv_vector(owin_public, x + w, y + 0, x + w, y + h, PIX_SRC, 1);
}

void
openwin_lolite_view(owin_public, view)
    Openwin         owin_public;
    Openwin_view_info *view;
{
    Rect            r;
    int             x, y, w, h;
    int             border_width = openwin_border_width(owin_public, view->view);

    win_getrect(view->view, &r);
    x = r.r_left - border_width + 1;
    y = r.r_top - border_width + 1;
    w = r.r_width + 2 * border_width - 3;
    h = r.r_height + 2 * border_width - 3;
    xv_vector(owin_public, x + 0, y + 0, x + w, y + 0, PIX_SRC, 0);
    xv_vector(owin_public, x + 0, y + h, x + w, y + h, PIX_SRC, 0);
    xv_vector(owin_public, x + 0, y + 0, x + 0, y + h, PIX_SRC, 0);
    xv_vector(owin_public, x + w, y + 0, x + w, y + h, PIX_SRC, 0);
}
