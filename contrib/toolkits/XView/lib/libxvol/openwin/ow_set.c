#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)ow_set.c 1.39 89/10/17";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	ow_set.c Product:	SunView 2.0
 * 
 * Description:
 * 
 * Sets values for openwin's attributes
 * 
 */


/*
 * Include files:
 */
#include <xview_private/ow_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/font.h>



/*
 * Declaration of Functions Defined in This File (in order):
 */
Xv_opaque       openwin_set();

static Xv_opaque openwin_parse_attr();
static Xv_opaque openwin_set_scrollbar();
static void     openwin_append_view_attrs();
static void     openwin_set_width();
static void     openwin_set_height();


/******************************************************************/

Xv_opaque
openwin_set(owin_public, avlist)
    Openwin         owin_public;
    Attr_avlist     avlist;
{
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);

    return (openwin_parse_attr(owin, avlist));
}


static          Xv_opaque
openwin_parse_attr(owin, argv)
    Xv_openwin_info *owin;
    Attr_avlist     argv;
{
    Openwin_attribute attr;
    Rect            r;
    Xv_Window       view;
    Openwin_view_info *viewinfo;
    Xv_opaque       result = XV_OK;

    for (attr = (Openwin_attribute) argv[0]; attr;
	 argv = attr_next(argv), attr = (Openwin_attribute) argv[0]) {
	switch (attr) {
	  case WIN_COLUMNS:
	    /* If you intend to attach a vertical scrollbar to this openwin,
	     * and you want the view to be the WIN_COLUMNS specified, then
	     * you must specify
	     * OPENWIN_ADJUST_FOR_VERTICAL_SCROLLBAR, TRUE,
	     */
	    if (status_is(owin, created))
		openwin_set_width(owin, (int) argv[1]);
	    else
		owin->nbr_cols = (int) argv[1];
	    ATTR_CONSUME(argv[0]);
	    break;

	  case WIN_ROWS:
	    /* If you intend to attach a horizontal scrollbar to this openwin,
	     * and you want the view to be the WIN_ROWS specified, then
	     * you must specify
	     * OPENWIN_ADJUST_FOR_HORIZONTAL_SCROLLBAR, TRUE,
	     */
	    if (status_is(owin, created))
		openwin_set_height(owin, (int) argv[1]);
	    else
		owin->nbr_rows = (int) argv[1];
	    ATTR_CONSUME(argv[0]);
	    break;

	  case WIN_VERTICAL_SCROLLBAR:
	    if ((Scrollbar) argv[1] != NULL) {
		status_set(owin, adjust_vertical);
	    } else {
		status_reset(owin, adjust_vertical);
	    }
	    if (status_is(owin, created)) {
		(void) openwin_set_scrollbar(owin, (Scrollbar) argv[1], SCROLLBAR_VERTICAL);
	    } else {
		owin->vsb_on_create = (Scrollbar) argv[1];
	    }
	    break;
	  case WIN_HORIZONTAL_SCROLLBAR:
	    if ((Scrollbar) argv[1] != NULL) {
		status_set(owin, adjust_horizontal);
	    } else {
		status_reset(owin, adjust_horizontal);
	    }
	    if (status_is(owin, created)) {
		(void) openwin_set_scrollbar(owin, (Scrollbar) argv[1], SCROLLBAR_HORIZONTAL);
	    } else {
		owin->hsb_on_create = (Scrollbar) argv[1];
	    }
	    break;
	  case OPENWIN_NO_MARGIN:
	    if (!status_is(owin, created)) {
		if ((int) argv[1] == 0) {
		    status_reset(owin, no_margin);
		} else {
		    status_set(owin, no_margin);
		    owin->margin = 0;
		}
	    } else {
		xv_error(OPENWIN_PUBLIC(owin), XV_ERROR_RECOVERABLE,
			 XV_ERROR_CREATE_ONLY, attr,
		XV_ERROR_STRING, "this attribute can only be set on create",
			 0);
	    }
	    break;

	  case OPENWIN_SHOW_BORDERS:
	    if (!status_is(owin, created)) {
		if ((int) argv[1] == 0) {
		    status_reset(owin, show_borders);
		} else {
		    status_set(owin, show_borders);
		}
	    } else {
		xv_error(OPENWIN_PUBLIC(owin), XV_ERROR_RECOVERABLE,
			 XV_ERROR_CREATE_ONLY, attr,
		XV_ERROR_STRING, "this attribute can only be set on create",
			 0);
	    }
	    break;

	  case OPENWIN_SELECTED_VIEW:
	    if (argv[1] == NULL)
		viewinfo = NULL;
	    else
		openwin_viewdata_for_view((Xv_Window) argv[1], &viewinfo);
	    openwin_select_view(OPENWIN_PUBLIC(owin), viewinfo);
	    break;

	  case OPENWIN_AUTO_CLEAR:
	    if ((int) argv[1] == 0) {
		status_reset(owin, auto_clear);
	    } else {
		status_set(owin, auto_clear);
	    }
	    break;
	  case OPENWIN_ADJUST_FOR_VERTICAL_SCROLLBAR:
	    if ((int) argv[1] != status_is(owin, adjust_vertical) &&
		(int) argv[1] == FALSE) {
		status_reset(owin, adjust_vertical);
	    } else if ((int) argv[1] != status_is(owin, adjust_vertical)) {
		status_set(owin, adjust_vertical);
	    }
	    if (status_is(owin, created)) {
		r = *(Rect *) xv_get(OPENWIN_PUBLIC(owin), WIN_RECT);
		openwin_adjust_views(owin, &r);
	    }
	    break;
	  case OPENWIN_ADJUST_FOR_HORIZONTAL_SCROLLBAR:
	    if ((int) argv[1] != status_is(owin, adjust_horizontal) &&
		(int) argv[1] == FALSE) {
		status_reset(owin, adjust_horizontal);
	    } else if ((int) argv[1] != status_is(owin, adjust_horizontal)) {
		status_set(owin, adjust_horizontal);
	    }
	    if (status_is(owin, created)) {
		r = *(Rect *) xv_get(OPENWIN_PUBLIC(owin), WIN_RECT);
		openwin_adjust_views(owin, &r);
	    }
	    break;
	  case OPENWIN_VIEW_ATTRS:
	    if (status_is(owin, created)) {
		OPENWIN_EACH_VIEW(OPENWIN_PUBLIC(owin), view)
		    xv_set_avlist(view, &(argv[1]));
		OPENWIN_END_EACH
	    } else {
		openwin_append_view_attrs(owin, &(argv[1]));
	    }
	    break;
	  case OPENWIN_SPLIT:
	    if (openwin_parse_split_attrs(owin, &(argv[1])) != XV_OK) {
		/* handle the error?? */
	    }
	    break;
	  case XV_END_CREATE:
	    /* openwin size if now correct */
	    owin->cached_rect = *(Rect *) xv_get(OPENWIN_PUBLIC(owin), WIN_RECT);
	    owin->view_class = (Xv_pkg *) xv_get(OPENWIN_PUBLIC(owin), OPENWIN_VIEW_CLASS);
	    if (owin->view_class == NULL) {
		owin->view_class = (Xv_pkg *) WINDOW;
	    }
	    openwin_create_initial_view(owin);
	    /* Note: openwin_set_width and openwin_set_height will each
	     * generate a WIN_RESIZE event on the openwin because they change
	     * the openwin's width and height, respectively.  This will cause
	     * openwin_adjust_views to be called, which calls
	     * openwin_adjust_view, which calls
	     * openwin_view_rect_from_avail_rect, which calls
	     * openwin_adjust_view_scrollbars, which changes the size of the
	     * view to make room for the scrollbar(s).
	     *
	     * N.B.:  The size of the view window will always be determined
	     * by the size of the enclosing openwin.  If adjust_{vertical,
	     * horizontal} is set, then the view is shrunk by the width of
	     * the scrollbar that would go there.  This adjustment is made
	     * in openwin_adjust_view_scrollbars.
	     * openwin_adjust_view_scrollbars is eventually called in the
	     * following situations:
	     *	- from openwin_create_initial_view
	     *  - a WIN_RESIZE event
	     *  - frow openwin_set_width (causes a WIN_RESIZE)
	     *  - from openwin_set_height (causes a WIN_RESIZE)
	     *  - from openwin_set_scrollbar
	     */
	    if (owin->nbr_cols > 0)
		openwin_set_width(owin, owin->nbr_cols);
	    if (owin->nbr_rows > 0)
		openwin_set_height(owin, owin->nbr_rows);
	    status_set(owin, created);
	    break;
	  default:
	    xv_check_bad_attr(OPENWIN, attr);
	    break;
	}
    }
    return (result);
}

static int
openwin_parse_split_attrs(owin, argv)
    Xv_openwin_info *owin;
    Attr_avlist     argv;
{
    Openwin_attribute attr;
    Openwin_split_direction split_direction = OPENWIN_SPLIT_HORIZONTAL;
    Openwin_view_info *view;
    Xv_Window       split_view = NULL;
    int             split_position = 0;
    int             split_viewstart = OPENWIN_SPLIT_NEWVIEW_IN_PLACE;
    Rect            r;
    Scrollbar       sb;

    for (attr = (Openwin_attribute) argv[0]; attr;
	 argv = attr_next(argv), attr = (Openwin_attribute) argv[0]) {
	switch (attr) {
	  case OPENWIN_SPLIT_DIRECTION:
	    split_direction = (Openwin_split_direction) argv[1];
	    break;
	  case OPENWIN_SPLIT_VIEW:
	    split_view = (Xv_Window) argv[1];
	    break;
	  case OPENWIN_SPLIT_POSITION:
	    split_position = (int) argv[1];
	    break;
	  case OPENWIN_SPLIT_INIT_PROC:
	    owin->split_init_proc = (void (*) ()) argv[1];
	    break;

	  case OPENWIN_SPLIT_DESTROY_PROC:
	    owin->split_destroy_proc = (void (*) ()) argv[1];
	    break;

	  case OPENWIN_SPLIT_VIEW_START:
	    split_viewstart = (int) argv[1];
	    break;
	  default:
	    xv_check_bad_attr(OPENWIN, attr);
	    break;
	}
    }

    /* do data validation */

    /* see if a window was passed to be split and if it is valid */
    if (split_view == NULL || openwin_viewdata_for_view(split_view, &view) != XV_OK) {
	/* error invalid view */
	return (XV_ERROR);
    }
    /* see if position is one in the window */
    r = *(Rect *) xv_get(split_view, WIN_RECT);
    if (split_direction == OPENWIN_SPLIT_VERTICAL &&
	(split_position < OPENWIN_SPLIT_VERTICAL_MINIMUM ||
	 split_position > r.r_width - OPENWIN_SPLIT_VERTICAL_MINIMUM)) {
	/* error invalid position */
	return (XV_ERROR);
    } else if (split_direction == OPENWIN_SPLIT_HORIZONTAL &&
	       (split_position < OPENWIN_SPLIT_HORIZONTAL_MINIMUM ||
	  split_position > r.r_height - OPENWIN_SPLIT_HORIZONTAL_MINIMUM)) {
	/* error invalid position */
	return (XV_ERROR);
    }
    /* see if view start is valid */
    if (split_viewstart == OPENWIN_SPLIT_NEWVIEW_IN_PLACE) {
	sb = (split_direction == OPENWIN_SPLIT_VERTICAL) ? openwin_sb(view, SCROLLBAR_HORIZONTAL) : openwin_sb(view, SCROLLBAR_VERTICAL);
	if (sb != NULL) {
	    split_viewstart = (int) xv_get(sb, SCROLLBAR_VIEW_START) + split_position;
	} else {
	    split_viewstart = split_position;
	}
    }
    openwin_split_view(owin, view, split_direction, split_position, split_viewstart);
    return (XV_OK);
}


static          Xv_opaque
openwin_set_scrollbar(owin, sb, direction)
    Xv_openwin_info *owin;
    Scrollbar       sb;
    Scrollbar_setting direction;
{
    Openwin_view_info *view = owin->views;
    Rect            r;
    int             view_length;
    Xv_opaque       result = (Xv_opaque) XV_OK;
    Xv_opaque       sb_notify_client;

    /* give the vertical scrollbar to the first view */
    if (sb != NULL) {
	/* if we already have a scrollbar report an error */
	while (view != NULL) {
	    if (openwin_sb(view, direction) != NULL) {
		/* FATAL ERROR */
		return ((Xv_opaque) XV_ERROR);
	    }
	    view = view->next_view;
	}

	/* give this scrollbar to first view */
	openwin_set_sb(owin->views, direction, sb);

	/* Adjust the size of the view and place the scrollbar. */
	r = owin->views->enclosing_rect;
	openwin_adjust_view(owin, owin->views, &r);

	view_length = (direction == SCROLLBAR_VERTICAL) ?
	    (int) xv_get(owin->views->view, WIN_HEIGHT) :
	    (int) xv_get(owin->views->view, WIN_WIDTH);
	view_length = view_length /
	    (int) xv_get(sb, SCROLLBAR_PIXELS_PER_UNIT);

	if (xv_get(sb, WIN_PARENT) != OPENWIN_PUBLIC(owin) ||
	    xv_get(sb, XV_OWNER) != OPENWIN_PUBLIC(owin))
	    xv_set(sb, WIN_PARENT, OPENWIN_PUBLIC(owin),
		   XV_OWNER, OPENWIN_PUBLIC(owin),
		   0);

	xv_set(sb,
	       SCROLLBAR_DIRECTION, direction,
	       SCROLLBAR_VIEW_LENGTH, view_length,
	       XV_SHOW, TRUE,
	       0);

	sb_notify_client = xv_get(sb, SCROLLBAR_NOTIFY_CLIENT);
	if (sb_notify_client == NULL ||
	    sb_notify_client == OPENWIN_PUBLIC(owin)) {
	    xv_set(sb,
		   SCROLLBAR_NOTIFY_CLIENT, owin->views->view,
		   0);
	}
	/* create new scrollbars for other views */
	view = owin->views->next_view;
	while (view != NULL) {
	    openwin_copy_scrollbar(owin, sb, view);
	    r = view->enclosing_rect;
	    openwin_adjust_view(owin, view, &r);
	    view = view->next_view;
	}
    } else {
	/* remove all scrollbars */
	/* set bit so layout code which removes sb's  */
	/* isn't invoked */
	/* for each view unset as having sb and adjust view */
	for (view = owin->views; view != NULL; view = view->next_view) {
	    sb = openwin_sb(view, direction);
	    openwin_set_sb(view, direction, NULL);
	    if (sb != NULL) {
		xv_destroy(sb);
	    }
	}
	r = *(Rect *) xv_get(OPENWIN_PUBLIC(owin), WIN_RECT);
	openwin_adjust_views(owin, &r);
    }
    return (result);
}

static void
openwin_append_view_attrs(owin, argv)
    Xv_openwin_info *owin;
    Attr_avlist     argv;
{
    if (owin->view_avlist == NULL) {
	owin->view_avlist = (Attr_avlist) xv_alloc_n(Openwin_attribute, ATTR_STANDARD_SIZE);
	owin->view_end_avlist = owin->view_avlist;
    }
    owin->view_end_avlist = (Attr_avlist) attr_copy_avlist(owin->view_end_avlist, argv);
}


static void
openwin_set_width(owin, ncols)
    Xv_openwin_info *owin;
    int             ncols;	/* number of columns */
{
    Openwin         owin_public = OPENWIN_PUBLIC(owin);
    Scrollbar       sb = openwin_sb(owin->views, SCROLLBAR_VERTICAL);
    int		    sb_width;
    int             width;

    if (sb)
	sb_width = (int) xv_get(sb, XV_WIDTH);
    else if (status_is(owin, adjust_vertical)) {
	sb_width = scrollbar_width_for_scale(
	    xv_get(xv_get(owin_public, XV_FONT), FONT_SCALE));
    } else
	sb_width = 0;
    width = (int) xv_cols(owin->views->view, ncols) +
	(status_is(owin, no_margin) ? 0 :
	 (int) xv_get(owin_public, WIN_LEFT_MARGIN) +
	 (int) xv_get(owin_public, WIN_RIGHT_MARGIN)) +
	sb_width +
	2 * owin->margin +
	2 * openwin_border_width(owin_public, owin->views->view);
    if ((int) xv_get(owin_public, XV_WIDTH) != width)
	xv_set(owin_public, XV_WIDTH, width, 0);
}

static void
openwin_set_height(owin, nrows)
    Xv_openwin_info *owin;
    int             nrows;	/* number of columns */
{
    Openwin         owin_public = OPENWIN_PUBLIC(owin);
    Scrollbar       sb = openwin_sb(owin->views, SCROLLBAR_HORIZONTAL);
    int		    sb_height;
    int             height;

    if (sb)
	sb_height = (int) xv_get(sb, XV_WIDTH);
    else if (status_is(owin, adjust_horizontal)) {
	sb_height = scrollbar_width_for_scale(
	    xv_get(xv_get(owin_public, XV_FONT), FONT_SCALE));
    } else
	sb_height = 0;
    height = (int) xv_rows(owin->views->view, nrows) +
	(status_is(owin, no_margin) ? 0 :
	 (int) xv_get(owin_public, WIN_TOP_MARGIN) +
	 (int) xv_get(owin_public, WIN_BOTTOM_MARGIN)) +
	sb_height +
	2 * owin->margin +
	2 * openwin_border_width(owin_public, owin->views->view);

    if ((int) xv_get(owin_public, XV_HEIGHT) != height)
	xv_set(owin_public, XV_HEIGHT, height, 0);
}
