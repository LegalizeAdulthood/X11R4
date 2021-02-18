#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)ow_evt.c 1.24 89/10/17";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	ow_evt.c Product:	SunView 2.0
 * 
 * Description:
 * 
 * Handles events to the openwin window. Handles events to view window as well
 * -- clears damage
 * 
 */


/*
 * Include files:
 */
#include <xview_private/ow_impl.h>


/*
 * Declaration of Functions Defined in This File (in order):
 */

Notify_value    openwin_event();
Notify_value    openwin_view_event();

/******************************************************************/

Notify_value
openwin_event(owin_public, event, arg, type)
    Openwin         owin_public;
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    Xv_openwin_info *owin = OPENWIN_PRIVATE(owin_public);
    Rect            r;

    switch (event_action(event)) {
      case ACTION_SELECT:
	if (event_is_down(event))
	    openwin_select(owin_public, event);
	break;
      case ACTION_RESCALE:
	openwin_rescale_views(owin_public, (int) arg);
	break;
      case WIN_RESIZE:
	r = *(Rect *) xv_get(owin_public, WIN_RECT);
	openwin_adjust_views(owin, &r);
	owin->cached_rect = r;
	break;
      case WIN_REPAINT:
	/*
	 * should never get here since borders are always drawn using X
	 * borders and WIN_REPAINT is turned off for the openwin.
	 * 
	 * openwin_clear_damage(owin_public, win_get_damage(owin_public)); if
	 * (status_is(owin, show_borders))
	 * openwin_paint_borders(owin_public);
	 */
	break;
      default:
	break;
    }

    return notify_next_event_func(owin_public, (Notify_event) event, arg, type);
}

Notify_value
openwin_view_event(window_public, event, arg, type)
    Xv_Window       window_public;
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{

    switch (event_action(event)) {
      case WIN_REPAINT:
	{
	    /* clear the damaged area */
	    Openwin_view_info *view = (Openwin_view_info *) xv_get(window_public,
				     XV_KEY_DATA, openwin_view_context_key);

	    if (status_is(view->owin, auto_clear)) {
		openwin_clear_damage(window_public, win_get_damage(window_public));
	    }
	    break;
	}
      case ACTION_SPLIT_HORIZONTAL:
	{
	    Openwin_view_info *view = (Openwin_view_info *) xv_get(window_public,
				     XV_KEY_DATA, openwin_view_context_key);

	    xv_set(OPENWIN_PUBLIC(view->owin),
		   OPENWIN_SPLIT,
		   OPENWIN_SPLIT_VIEW, view->view,
		   OPENWIN_SPLIT_DIRECTION, OPENWIN_SPLIT_HORIZONTAL,
		   OPENWIN_SPLIT_POSITION, (int) arg,
		   0,
		   0);
	    break;
	}
      case ACTION_SPLIT_VERTICAL:
	{
	    Openwin_view_info *view = (Openwin_view_info *) xv_get(window_public,
				     XV_KEY_DATA, openwin_view_context_key);

	    xv_set(OPENWIN_PUBLIC(view->owin),
		   OPENWIN_SPLIT,
		   OPENWIN_SPLIT_VIEW, view->view,
		   OPENWIN_SPLIT_DIRECTION, OPENWIN_SPLIT_VERTICAL,
		   OPENWIN_SPLIT_POSITION, (int) arg,
		   0,
		   0);
	    break;
	}
      case ACTION_SPLIT_DESTROY:
	{
	    Openwin_view_info *view = (Openwin_view_info *) xv_get(window_public,
				     XV_KEY_DATA, openwin_view_context_key);

	    if (view->owin->seln_view != NULL)
		openwin_select_view(OPENWIN_PUBLIC(view->owin), NULL);
	    if (openwin_count_views(view->owin) > 1)
		xv_destroy_safe(window_public);
	    return (NOTIFY_DONE);
	}
      default:
	break;
    }
    return notify_next_event_func(window_public, (Notify_event) event, arg, type);
}
