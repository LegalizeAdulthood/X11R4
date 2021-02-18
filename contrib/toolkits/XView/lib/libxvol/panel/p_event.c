#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_event.c 20.19 89/10/24";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */


#include <xview_private/panel_impl.h>
#include <xview/font.h>

static Notify_value panel_itimer_expired();

/* ARGSUSED */
Pkg_private     Notify_value
panel_notify_panel_event(window, event, arg, type)
    Xv_Window       window;
    register Event *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    Panel_info     *panel = PANEL_PRIVATE(window);
    Notify_value    result;

    switch (event_action(event)) {
      case ACTION_RESCALE:
	panel_refont(panel, (int) arg);
	result = notify_next_event_func(window, event, arg, type);
	break;
      default:
	result = notify_next_event_func(window, event, arg, type);
	break;
    }

    return (result);
}


Pkg_private     Notify_value
panel_notify_event(paint_window, event, arg, type)
    Xv_Window       paint_window;
    register Event *event;
    Notify_arg      arg;
    Notify_event_type type;
{
    register Panel_info *panel = (Panel_info *) xv_get(paint_window,
					    XV_KEY_DATA, panel_context_key);

    switch (event_action(event)) {
      case KBD_USE:
	(void) frame_kbd_use((Xv_Window) xv_get(PANEL_PUBLIC(panel),
					   WIN_FRAME), PANEL_PUBLIC(panel));
	if (blinking(panel) && panel->caret)
	    panel_itimer_set(panel, panel->timer_full);
	panel_caret_on(panel, FALSE);
	panel->caret_pr = &panel_caret_pr;
	panel_caret_on(panel, TRUE);
	panel->status |= PANEL_HAS_INPUT_FOCUS;
	break;

      case KBD_DONE:
	(void) frame_kbd_done((Xv_Window) xv_get(PANEL_PUBLIC(panel),
					   WIN_FRAME), PANEL_PUBLIC(panel));
	panel_caret_on(panel, FALSE);
	panel_itimer_set(panel, NOTIFY_NO_ITIMER);
	panel->caret_pr = &panel_ghost_caret_pr;
	panel_caret_on(panel, TRUE);
	panel->status &= ~PANEL_HAS_INPUT_FOCUS;
	break;

      case ACTION_RESCALE:{
	    panel_refont(panel, (int) arg);
	    break;
	}

      default:
	break;
    }
    return panel_use_event(panel, event, arg, type, paint_window);
}


/* ARGSUSED */
static          Notify_value
panel_itimer_expired(panel_public, which)
    Panel           panel_public;
    int             which;
{
    register Panel_info *panel = PANEL_PRIVATE(panel_public);

    if (blinking(panel)) {
	panel_caret_invert(panel);
    } else {
	/* turn itimer off */
	panel_itimer_set(panel, NOTIFY_NO_ITIMER);
    }

}

Pkg_private void
panel_itimer_set(panel, timer)
    register Panel_info *panel;
    struct itimerval timer;
{

    (void) notify_set_itimer_func((Notify_client) PANEL_PUBLIC(panel),
	 panel_itimer_expired, ITIMER_REAL, &timer, (struct itimerval *) 0);
}
