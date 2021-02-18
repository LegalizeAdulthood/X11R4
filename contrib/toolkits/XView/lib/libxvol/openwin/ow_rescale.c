#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)ow_rescale.c 1.14 89/10/17";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/ow_impl.h>
#include <xview/font.h>

/*
 * Functions in this file : (in order)
 */
Pkg_private     openwin_rescale_views();
/*
 * rescale the sub_tree
 */


Pkg_private
openwin_rescale_views(owin_public, scale)
    Openwin         owin_public;
    int             scale;
{
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);
    Openwin_view_info *view = owin->views;
    Xv_opaque       rect_obj_list;
    int             num_views = 0, i = 0;
    int             parent_width, parent_height;
    Rect            new_rect, parent_new_rect;

    /*
     * first change scale unless this has been in the event func
     */

    parent_new_rect = *(Rect *) xv_get(owin_public, WIN_RECT);
    parent_width = parent_new_rect.r_width;
    parent_height = parent_new_rect.r_height;

    /*
     * Openwin rect has been set. The rescale has changed the font as well
     */

    num_views = openwin_count_views();
    rect_obj_list = window_create_rect_obj_list(num_views);

    for (view = owin->views; view != NULL; view = view->next_view) {
	window_set_rescale_state(view->view, scale);
	window_start_rescaling(view->view);
	window_add_to_rect_list(rect_obj_list, view->view, view->enclosing_rect, i);
	i++;
    }
    window_adjust_rects(rect_obj_list, num_views, parent_width, parent_height);
    i = 0;
    for (view = owin->views; view != NULL; view = view->next_view) {
	if (!window_rect_equal_ith_obj(rect_obj_list, &new_rect, i))
	    openwin_adjust_view(owin, view, &new_rect);
	window_end_rescaling(view->view);
    }
    window_destroy_rect_obj_list(rect_obj_list);
}
