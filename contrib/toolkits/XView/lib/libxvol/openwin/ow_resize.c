#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)ow_resize.c 1.29 89/10/17";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	ow_resize.c Product:	SunView 2.0
 * 
 * Description:
 * 
 * Adjusts sizes of views in accordance with my resize
 * 
 */


/*
 * Include files:
 */
#include <xview_private/ow_impl.h>
#include <xview/font.h>


/*
 * Declaration of Functions Defined in This File (in order):
 */
int             openwin_adjust_views();
void            openwin_adjust_view();
void            openwin_place_scrollbar();

static          openwin_adjust_view_rect();
static          openwin_adjust_view_scrollbars();
static          openwin_adjust_view_by_margins();
int             openwin_border_width();

/******************************************************************/

int
openwin_adjust_views(owin, owin_rect)
    Xv_openwin_info *owin;
    Rect           *owin_rect;
{
    Openwin_view_info *view = owin->views;
    Rect            r;
    int             v_change, h_change;

    v_change = owin_rect->r_height - owin->cached_rect.r_height;
    h_change = owin_rect->r_width - owin->cached_rect.r_width;

    /* find the views that are on the vertical edge */
    for (view = owin->views; view != NULL; view = view->next_view) {
	if (view->right_edge) {
	    /* see whether changes are legal */
	    r = view->enclosing_rect;
	    r.r_width += h_change;
	    if (r.r_width <= 0) {
		/* error */
		return (XV_ERROR);
	    }
	    /* for performance handle case where view is also */
	    /* on the bottom edge */
	    if (view->bottom_edge) {
		/* see whether changes are legal */
		r.r_height += v_change;
		if (r.r_height <= v_change) {
		    /* error */
		    return (XV_ERROR);
		}
	    }
	    openwin_adjust_view(owin, view, &r);
	} else if (view->bottom_edge) {
	    r = view->enclosing_rect;
	    /* see whether changes are legal */
	    r.r_height += v_change;
	    if (r.r_height <= v_change) {
		/* error */
		return (XV_ERROR);
	    }
	    openwin_adjust_view(owin, view, &r);
	}
    }
}

void
openwin_adjust_view(owin, view, view_rect)
    Xv_openwin_info *owin;
    Openwin_view_info *view;
    Rect           *view_rect;
{
    Rect            r, sb_r;
    Scrollbar       sb;

    r = view->enclosing_rect = *view_rect;

    openwin_view_rect_from_avail_rect(owin, view, &r);

    if (r.r_width <= 0) {
	r.r_width = view_rect->r_width;
    } else if (r.r_height <= 0) {
	r.r_height = view_rect->r_height;
    }
    /* place the scrollbars */
    if ((sb = openwin_sb(view, SCROLLBAR_VERTICAL)) != NULL) {
	openwin_place_scrollbar(OPENWIN_PUBLIC(owin), view->view,
		   openwin_sb(view, SCROLLBAR_VERTICAL), SCROLLBAR_VERTICAL,
				&r, &sb_r);
	xv_set(sb, WIN_RECT, &sb_r, 0);
    }
    if ((sb = openwin_sb(view, SCROLLBAR_HORIZONTAL)) != NULL) {

	openwin_place_scrollbar(OPENWIN_PUBLIC(owin), view->view,
	       openwin_sb(view, SCROLLBAR_HORIZONTAL), SCROLLBAR_HORIZONTAL,
				&r, &sb_r);
	xv_set(sb, WIN_RECT, &sb_r, 0);
    }
    /*
     * now place the view.  Must do this after placing the sb's because if
     * the sb's are moved after the view has been resized, they cause an
     * exposure to be sent to the view, causing a second repaint.
     */
    openwin_adjust_view_rect(owin, view, &r);
}

void
openwin_place_scrollbar(owin_public, view_public, sb, direction, r, sb_r)
    Xv_object	    owin_public;
    Scrollbar       sb;
    Xv_opaque       view_public;
    Scrollbar_setting direction;
    Rect           *r, *sb_r;
{
    int             border_width;

    if (sb == NULL)
	return;
    border_width = openwin_border_width(owin_public, view_public);

    if (direction == SCROLLBAR_VERTICAL) {
	sb_r->r_left = r->r_left + r->r_width + border_width;
	sb_r->r_top = r->r_top - border_width;
	sb_r->r_width = (int) xv_get(sb, WIN_WIDTH);
	sb_r->r_height = r->r_height + (2 * border_width);
    } else {
	sb_r->r_left = r->r_left - border_width;
	sb_r->r_top = r->r_top + r->r_height + border_width;
	sb_r->r_width = r->r_width + (2 * border_width);
	sb_r->r_height = (int) xv_get(sb, WIN_HEIGHT);
    }
}


openwin_view_rect_from_avail_rect(owin, view, r)
    Xv_openwin_info *owin;
    Openwin_view_info *view;
    Rect           *r;
{
    openwin_adjust_view_scrollbars(owin, view, r);
    openwin_adjust_view_by_margins(owin, view, owin->margin, r);
}

static
openwin_adjust_view_rect(owin, view, view_rect)
    Xv_openwin_info *owin;
    Openwin_view_info *view;
    Rect           *view_rect;
{
    Scrollbar       vsb, hsb;

    vsb = openwin_sb(view, SCROLLBAR_VERTICAL);
    hsb = openwin_sb(view, SCROLLBAR_HORIZONTAL);

    /* clear borders if painted */
    if (status_is(owin, show_borders))
	openwin_paint_border(OPENWIN_PUBLIC(owin), view, FALSE);
    xv_set(view->view, WIN_RECT, view_rect, 0);
    /* repaint borders is shown */
    if (status_is(owin, show_borders))
	openwin_paint_border(OPENWIN_PUBLIC(owin), view, TRUE);

    if (vsb != NULL) {
	xv_set(vsb,
	       SCROLLBAR_VIEW_LENGTH, view_rect->r_height / (int) xv_get(vsb, SCROLLBAR_PIXELS_PER_UNIT),
	       0);
    }
    if (hsb != NULL) {
	xv_set(hsb,
	       SCROLLBAR_VIEW_LENGTH, view_rect->r_width / (int) xv_get(hsb, SCROLLBAR_PIXELS_PER_UNIT),
	       0);
    }
}

static
openwin_adjust_view_scrollbars(owin, view, avail_rect)
    Xv_openwin_info *owin;
    Openwin_view_info *view;
    Rect           *avail_rect;
{
    int             vsb_w, hsb_h;

    /* no computation if not adjusted or already adjusted */
    if (!status_is(owin, adjust_vertical) && !status_is(owin, adjust_horizontal)) {
	return;
    }

    vsb_w = hsb_h = scrollbar_width_for_scale(
	xv_get(xv_get(OPENWIN_PUBLIC(owin), XV_FONT), FONT_SCALE));

    if (status_is(owin, adjust_vertical) && vsb_w < avail_rect->r_width) {
	avail_rect->r_width -= vsb_w;
    }
    if (status_is(owin, adjust_horizontal) && hsb_h < avail_rect->r_height) {
	avail_rect->r_height -= hsb_h;
    }
}

static
openwin_adjust_view_by_margins(owin, view, margin, view_rect)
    Xv_openwin_info *owin;
    Openwin_view_info *view;
    int             margin;
    Rect           *view_rect;
{
    int             n_vmargins, n_hmargins;
    int             border_width = 0;

    if (status_is(owin, no_margin)) {
	n_vmargins = n_hmargins = 0;
    } else {
	n_vmargins = n_hmargins = 2;
    }

#ifndef FIXME
    border_width = openwin_border_width(OPENWIN_PUBLIC(owin), view->view);
#else
    if (view->view == NULL || (int) xv_get(view->view, WIN_BORDER) == TRUE) {
	border_width = OPENWIN_VIEW_BORDER_WIDTH;
    }
#endif

#ifdef FIXME
    if (openwin_sb(view, SCROLLBAR_VERTICAL) != NULL
	|| status_is(owin, adjust_vertical)) {
	n_vmargins = 0;
    }
    if (openwin_sb(view, SCROLLBAR_HORIZONTAL) != NULL
	|| status_is(owin, adjust_horizontal)) {
	n_hmargins = 0;
    }
#endif

    view_rect->r_width -= n_vmargins * margin + 2 * border_width;
    view_rect->r_height -= n_hmargins * margin + 2 * border_width;
    view_rect->r_left += border_width;
    view_rect->r_top += border_width;
}

int
openwin_border_width(owin_public, view_public)
    Openwin         owin_public;
    Xv_opaque       view_public;
{
    /*
     * OPENWIN_SHOW_BORDERS & WIN_BORDER are the same now since borders are
     * always drawn using X window borders for performance reasons. However,
     * this might have to change when we implement border highlighting for
     * pane selection.
     */

    if (((int) xv_get(owin_public, OPENWIN_SHOW_BORDERS) == TRUE) ||
	(view_public && (int) xv_get(view_public, WIN_BORDER) == TRUE)) {
	return (WIN_DEFAULT_BORDER_WIDTH);
    } else {
	return (0);
    }
}
