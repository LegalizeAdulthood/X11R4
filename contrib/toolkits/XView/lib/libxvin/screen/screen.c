#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)screen.c 20.27 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/scrn_impl.h>
#include <xview/notify.h>
#include <xview/server.h>
#include <xview/window.h>
#include <xview/font.h>
#include <xview_private/xv_color.h>
#include <X11/Xlib.h>

Xv_private void xv_set_default_font();


Pkg_private int
screen_init(parent, screen_public, avlist)
    Xv_opaque       parent;
    Xv_Screen       screen_public;
    Attr_avlist     avlist;
{
    register Screen_info *screen;
    Xv_screen_struct *screen_object;
    register Attr_avlist attrs;
    Xv_font 	    font;
    Font            font_id;
    Display        *display;

    /* Allocate private data and set up forward/backward links. */
    screen = (Screen_info *) xv_alloc(Screen_info);
    screen->public_self = screen_public;
    screen_object = (Xv_screen_struct *)screen_public;
    screen_object->private_data = (Xv_opaque)screen;

    screen->server = parent ? parent : xv_default_server;
    screen->number = 0;

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case SCREEN_NUMBER:
	    screen->number = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  default:
	    break;
	}
    }

    display = (Display *) xv_get(parent, (Attr_attribute)XV_DISPLAY);

    screen_init_default_cms(screen_public);

    /*
     * By default, monochrome leaf windows (ie. windows into which bits are
     * actually written) are set to retained. This can be turned off by a
     * cmdline arg for debugging.
     */
    if (DisplayPlanes(display, screen->number) > 1) {
	screen->retain_windows = FALSE;
    } else {
	screen->retain_windows =
	    !(defaults_get_boolean("window.mono.disableRetained",
				   "Window.Mono.DisableRetained", FALSE));
    }

    /* set the default font in the GC for this screen */
    font = (Xv_font) xv_get(screen->server,
			      (Attr_attribute)SERVER_FONT_WITH_NAME,
			      NULL, NULL);
    if (!font)
	return (XV_ERROR);
    font_id = (Font) xv_get(font, (Attr_attribute)XV_XID);
    xv_set_default_font((Display *) xv_get(screen->server, (Attr_attribute)
					   XV_DISPLAY),
			screen->number,
			font_id);

    screen->root_window =
	xv_create(screen_public, WINDOW,
		  WIN_IS_ROOT,
		  WIN_LAYOUT_PROC, screen_layout,
		  NULL);

    if (!screen->root_window) {
	(void) free((char *) screen);
	return XV_ERROR;
    }
    /* now tell the server it has a new screen */
    (void) xv_set(screen->server,
	   SERVER_NTH_SCREEN, screen->number, screen_public,
	   NULL);

    return (XV_OK);
}

Pkg_private int
screen_destroy(screen_public, status)
    Xv_Screen       screen_public;
    Destroy_status  status;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);

    if (notify_post_destroy(screen->root_window, status, NOTIFY_IMMEDIATE) ==
	NOTIFY_DESTROY_VETOED)
	return XV_ERROR;

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    /* now tell the server it has lost a screen */
    (void) xv_set(screen->server, SERVER_NTH_SCREEN, screen->number,
		  (Xv_Screen) NULL, NULL);

    free((char *) screen);

    return (XV_OK);
}


Xv_private void
screen_set_clip_rects(screen_public, xrect_array, rect_count)
    Xv_Screen       screen_public;
    XRectangle     *xrect_array;
    int             rect_count;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    int             i;

    for (i = 0; i < rect_count; i++)
	screen->clip_xrects.rect_array[i] = xrect_array[i];
    screen->clip_xrects.count = rect_count;
}


Xv_private Xv_xrectlist *
screen_get_clip_rects(screen_public)
    Xv_Screen       screen_public;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);

    return (&screen->clip_xrects);
}


Xv_private void
screen_set_cached_window_busy(screen_public, window, busy)
    Xv_Screen       screen_public;
    Xv_window       window;
    int		    busy;
{
    Screen_info    *screen = SCREEN_PRIVATE(screen_public);
    Xv_cached_window *cached_window;

    for (cached_window = screen->cached_windows; cached_window != NULL;
	 cached_window = cached_window->next) {
	if (cached_window->window == window) {
	    cached_window->busy = busy;
	    break;
	}
    }
    if (cached_window == NULL) {
	xv_error(XV_NULL, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "Unable to return window to screen cache",
		 XV_ERROR_PKG_NAME, "Screen",
		 NULL);
    }
}
