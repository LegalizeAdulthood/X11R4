#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)icon_input.c 20.13 89/08/18 Copyr 1984 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/xview.h>
#include <xview/win_input.h>
#include <xview_private/icon_impl.h>

/* ARGSUSED */
Notify_value
icon_input(icon_public, event, arg, type)
    Icon            icon_public;
    Event          *event;
    Notify_arg      arg;
    Notify_event_type type;
{

    switch (event_action(event)) {

      case WIN_REPAINT:
	{

	    icon_display(icon_public, 0, 0);
	    return (NOTIFY_DONE);
	    break;
	}

      case WIN_RESIZE:
	return (NOTIFY_IGNORED);
	break;

      default:
	return (NOTIFY_IGNORED);
	break;
    }

}
