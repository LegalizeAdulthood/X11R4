#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)openwin.c 1.27 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	openwin.c Product:	SunView 2.0
 * 
 * Description:
 * 
 * Implements general creation and initialization for openwin
 * 
 */

/*
 * Include files:
 */

#include <stdio.h>
#include <xview_private/ow_impl.h>

extern Xv_opaque seln_create();

/*
 * Declaration of Functions Defined in This File (in order):
 */
static          openwin_layout();

/******************************************************************/


Pkg_private int
openwin_init(parent, owin_public, avlist)
    Xv_opaque       parent;
    Xv_opaque       owin_public;
    Xv_opaque      *avlist;
{
    Xv_openwin     *openwin = (Xv_openwin *) owin_public;
    Xv_openwin_info *owin;

    if (!(owin = xv_alloc(Xv_openwin_info))) {
	fprintf(stderr, "can't allocate openwin structure. Abort\n");
	return XV_ERROR;
    }
    owin->public_self = owin_public;
    openwin->private_data = (Xv_opaque) owin;
    owin->margin = OPENWIN_REGULAR_VIEW_MARGIN;
    owin->view_class = (Xv_pkg *) WINDOW;
    owin->cached_rect = *(Rect *) xv_get(owin_public, WIN_RECT);
    owin->seln_client =
	seln_create(openwin_seln_function, openwin_seln_reply, (char *) owin);
    status_set(owin, auto_clear);
    status_set(owin, show_borders);
    owin->layout_proc = (int (*) ()) xv_get(owin_public, WIN_LAYOUT_PROC);
    /*
     * For performance reasons, the openwin borders are always being painted
     * using X borders. This might change when border highlighting is
     * implemented for pane selection. WIN_CONSUME_PICK_EVENT, MS_LEFT &
     * WIN_REPAINT will have to turned on here to implement border
     * highlighting.
     */
    xv_set(owin_public,
	   WIN_NOTIFY_SAFE_EVENT_PROC, openwin_event,
	   WIN_NOTIFY_IMMEDIATE_EVENT_PROC, openwin_event,
	   WIN_IGNORE_EVENT, WIN_REPAINT,
	   WIN_INHERIT_COLORS, TRUE,
	   WIN_LAYOUT_PROC, openwin_layout,
	   0);

    return XV_OK;
}

Pkg_private int
openwin_destroy(owin_public, destroy_status)
    Openwin         owin_public;
    Destroy_status  destroy_status;
{
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);

    if ((destroy_status == DESTROY_CLEANUP) ||
	(destroy_status == DESTROY_PROCESS_DEATH)) {
	if (owin->seln_client != NULL) {
	    seln_destroy(owin->seln_client);
	    owin->seln_client = NULL;
	}
	/* unlink layout procs */
	xv_set(owin_public, WIN_LAYOUT_PROC, owin->layout_proc, 0);
	openwin_destroy_views(owin);

	if (destroy_status == DESTROY_CLEANUP)
	    free((char *) owin);
    }
    return XV_OK;
}


static
openwin_layout(owin_public, child, op, d1, d2, d3, d4, d5)
    Openwin         owin_public;
    Xv_Window       child;
    Window_layout_op op;
    int             d1, d2, d3, d4, d5;
{
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);
    Openwin_view_info *view;
    Scrollbar_setting direction;
    int             last;
    Rect            r;


    switch (op) {
      case WIN_CREATE:
	/* Determine if child is a scrollbar. */
	if (xv_get(child, XV_IS_SUBTYPE_OF, SCROLLBAR)) {
	    direction = (Scrollbar_setting) xv_get(child, SCROLLBAR_DIRECTION);
	    xv_set(owin_public, direction == SCROLLBAR_VERTICAL ?
		   WIN_VERTICAL_SCROLLBAR : WIN_HORIZONTAL_SCROLLBAR,
		   child,
		   0);
	}
	break;

      case WIN_DESTROY:
	if (openwin_viewdata_for_view(child, &view) == XV_OK) {
	    void            (*destroy_proc) ();

	    destroy_proc = owin->split_destroy_proc;
	    openwin_remove_split(owin, view);
	    (void) openwin_fill_view_gap(owin, view);
	    xv_free(view);
	    if (destroy_proc) {
		destroy_proc(owin_public);
	    }
	} else if (!status_is(owin, removing_scrollbars)) {
	    /* must look through data structures since can't */
	    /* do a get on the sb to get information */
	    if (openwin_viewdata_for_sb(owin, child, &view, &direction, &last) == XV_OK) {
		openwin_set_sb(view, direction, NULL);
		/* only re-adjust if last view with sb */
		if (last) {
		    if (direction == SCROLLBAR_VERTICAL) {
			status_reset(owin, adjust_vertical);
		    } else {
			status_reset(owin, adjust_horizontal);
		    }
		    r = *(Rect *) xv_get(OPENWIN_PUBLIC(owin), WIN_RECT);
		    openwin_adjust_views(owin, &r);
		}
	    }
	}
	break;
      default:
	break;
    }

    if (owin->layout_proc != NULL) {
	return (owin->layout_proc(owin_public, child, op, d1, d2, d3, d4, d5));
    } else {
	return TRUE;
    }
}
