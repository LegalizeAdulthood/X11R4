#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)ow_get.c 1.22 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	ow_get.c Product:	SunView 2.0
 * 
 * Description:
 * 
 * returns values for Openwin's attributes
 * 
 */


/*
 * Include files:
 */
#include <xview_private/ow_impl.h>
#include <xview_private/draw_impl.h>

/*
 * Declaration of Functions Defined in This File (in order):
 */

Xv_opaque       openwin_get();

/******************************************************************/

Xv_opaque
openwin_get(owin_public, get_status, attr, valist)
    Openwin         owin_public;
    int            *get_status;
    Openwin_attribute attr;
    va_list         valist;
{
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);
    Openwin_view_info *view;

    switch (attr) {
      case OPENWIN_NVIEWS:
	return ((Xv_opaque) openwin_count_views(owin));
      case OPENWIN_NTH_VIEW:
	view = openwin_nth_view(owin, va_arg(valist, int));
	if (view != NULL) {
	    return ((Xv_opaque) view->view);
	} else {
	    return ((Xv_opaque) NULL);
	}
      case WIN_VERTICAL_SCROLLBAR:
	view = openwin_nth_view(owin, 0);
	if (view == NULL)
	    return ((Xv_opaque) NULL);
	return ((Xv_opaque) openwin_sb(view, SCROLLBAR_VERTICAL));
      case WIN_HORIZONTAL_SCROLLBAR:
	view = openwin_nth_view(owin, 0);
	if (view == NULL)
	    return ((Xv_opaque) NULL);
	return ((Xv_opaque) openwin_sb(view, SCROLLBAR_HORIZONTAL));
      case OPENWIN_VERTICAL_SCROLLBAR:
	view = (Openwin_view_info *) xv_get(va_arg(valist, Xv_Window), XV_KEY_DATA, openwin_view_context_key);
	if (view == NULL) {
	    view = openwin_nth_view(owin, 0);
	    if (view == NULL)
		return ((Xv_opaque) NULL);
	}
	return ((Xv_opaque) openwin_sb(view, SCROLLBAR_VERTICAL));
      case OPENWIN_HORIZONTAL_SCROLLBAR:
	view = (Openwin_view_info *) xv_get(va_arg(valist, Xv_Window), XV_KEY_DATA, openwin_view_context_key);
	if (view == NULL) {
	    view = openwin_nth_view(owin, 0);
	    if (view == NULL)
		return ((Xv_opaque) NULL);
	}
	return ((Xv_opaque) openwin_sb(view, SCROLLBAR_HORIZONTAL));
      case OPENWIN_AUTO_CLEAR:
	return ((Xv_opaque) status_is(owin, auto_clear));
      case OPENWIN_ADJUST_FOR_VERTICAL_SCROLLBAR:
	return ((Xv_opaque) status_is(owin, adjust_vertical));
      case OPENWIN_ADJUST_FOR_HORIZONTAL_SCROLLBAR:
	return ((Xv_opaque) status_is(owin, adjust_horizontal));
      case OPENWIN_VIEW_CLASS:
	return ((Xv_opaque) owin->view_class);
      case OPENWIN_NO_MARGIN:
	return ((Xv_opaque) status_is(owin, no_margin));
      case OPENWIN_SHOW_BORDERS:
	return ((Xv_opaque) status_is(owin, show_borders));
      case OPENWIN_SELECTED_VIEW:
	return ((Xv_opaque) (owin->seln_view ? owin->seln_view->view : NULL));
      case OPENWIN_SPLIT_INIT_PROC:
	return ((Xv_opaque) (owin->split_init_proc));
	break;
      case OPENWIN_SPLIT_DESTROY_PROC:
	return ((Xv_opaque) (owin->split_destroy_proc));
	break;
      default:
	xv_check_bad_attr(OPENWIN, attr);
	*get_status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}
