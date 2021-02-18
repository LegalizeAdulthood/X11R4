#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)window.c 20.81 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifdef _XV_DEBUG
#include <xview_private/xv_debug.h>
#endif

#include <stdio.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <xview/notify.h>
#include <xview_private/windowimpl.h>
#include <xview/server.h>
#include <xview/screen.h>
#include <xview/window.h>
#include <xview_private/xv_color.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>
#include <xview_private/win_keymap.h>
#include <xview_private/win_info.h>

extern Screen_visual *screen_add_visual();

/*
 * Private
 */

static int      parent_dying = 0;	/* Don't destroy this window if */
/* its parent window is dying.  */
/* Server will do this for us.  */
Pkg_private int window_init();
Pkg_private int window_destroy_win_struct();
Pkg_private XID window_new();

#define eexit(msg, error_msg) \
  if (error_msg) { \
      char dummy[128]; \
      (void) sprintf(dummy, "window: %s\n%s", msg, error_msg); \
      xv_error(NULL, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL, \
          XV_ERROR_STRING, dummy, XV_ERROR_PKG, ATTR_PKG_DRAWABLE, 0); \
  } else { \
      char dummy[128]; \
      (void) sprintf(dummy, "window: %s\n", msg); \
      xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL, \
          XV_ERROR_STRING, dummy, XV_ERROR_PKG, ATTR_PKG_DRAWABLE, 0); \
      return XV_ERROR; \
  }



/*
 * Initialize win as a window.  Return XV_ERROR if error and no WIN_ERROR_MSG
 * is specified. Caller is required to pass a screen as the parent_public
 * when creating a root window.
 */
Pkg_private int
window_init(parent_public, win_public, avlist)
    Xv_Window       parent_public, win_public;
    Attr_avlist     avlist;
{
    Window_info    		*parent;
    Xv_window_struct 		*win_object = (Xv_window_struct *) win_public;
    register Window_info 	*win;
    register Attr_avlist	attrs;
    char           		*error_msg = NULL;
    char           		*default_fontname;
    int             		depth = 0;
    int             		is_root = FALSE, is_sel_window = FALSE;
    int             		input_only = FALSE;
    int             		transparent = FALSE;
    int             		is_client_pane = FALSE;
    Display        		*display;
    Xv_opaque       		screen, server;
    Visual         		*visual;
    int             		screen_num;
    Xv_Cursor       		default_cursor;
    register Xv_Drawable_info 	*info;
    register Xv_Drawable_info 	*parent_info = NULL;
    register int    		convert_cu = FALSE;
    register Bool   		border = FALSE;
    register Bool   		no_decor = FALSE;
    register Bool   		dynamic_color = FALSE;
    Xv_Colormap    		*cmap;

    /* get the name and error message */
    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case WIN_IS_ROOT:	/* create a root window */
	    is_root = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_SELECTION_WINDOW:
	    is_sel_window = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_INPUT_ONLY:	/* create an input only window */
	    input_only = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_TRANSPARENT:/* create a "see through" window */
	    transparent = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_IS_CLIENT_PANE:
	    is_client_pane = TRUE;
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_DEPTH:
	    depth = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_BORDER:
	    border = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_TOP_LEVEL_NO_DECOR:
	    no_decor = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_DYNAMIC_VISUAL:
	    dynamic_color = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_ERROR_MSG:
	    error_msg = (char *) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	    /*
	     * for compatiblity we check these to see if a character unit
	     * macro (ATTR_ROW/COL) has been used.  If so, then we must
	     * replace all of the character units in the avlist before any of
	     * the set procs are called.
	     */
	  case WIN_WIDTH:
	  case WIN_HEIGHT:
	  case WIN_FIT_WIDTH:
	  case WIN_FIT_HEIGHT:
	  case WIN_X:
	  case WIN_Y:
	    convert_cu |= attr_is_cu((int) attrs[1]);
	    break;

	  case WIN_MOUSE_XY:
	    convert_cu |= attr_is_cu((int) attrs[1]) |
		attr_is_cu((int) attrs[2]);
	    break;

	  default:
	    break;
	}
    }
    win = xv_alloc(Window_info);

    if (is_root) {
	parent = 0;
	win->top_level = TRUE;
    } else {
	if (!parent_public) {
	    /* xv_create ensures that xv_default_screen is valid. */
	    parent_public = xv_get(xv_default_screen, XV_ROOT);
	    win->top_level = TRUE;
	} else {
	    win->top_level = FALSE;
	}

	parent = WIN_PRIVATE(parent_public);
	if (!parent->layout_proc) {
	    eexit("Parent window does not support children", error_msg);
	}
	DRAWABLE_INFO_MACRO(parent_public, parent_info);
    }

    if (no_decor && !win->top_level) {
	xv_error(win_public, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "no decoration flag set for non-top level window", 0);
    }

    screen = (is_root) ? parent_public : xv_screen(parent_info);
    screen_num = (int) xv_get(screen, SCREEN_NUMBER);
    server = xv_get(screen, SCREEN_SERVER);
    display = (Display *) xv_get(server, XV_DISPLAY);
    DRAWABLE_INFO_MACRO(win_public, info);

    win_object->private_data = (Xv_opaque) win;
    win->public_self = win_public;
    win->top_level_no_decor = no_decor;
    win->created = FALSE;
    win->has_border = border;
    win->input_only = input_only;
    win->transparent = transparent;
    win->is_client_pane = is_client_pane;
    win->in_fullscreen_mode = FALSE;
    win->being_rescaled = FALSE;
    win->scale = (int) WIN_SCALE_MEDIUM;
    win->map = TRUE;
    win->win_front = FALSE;
    win->layout_proc = window_layout;
    win->xmask = ExposureMask;
    win->collapse_exposures = TRUE;
    win->owner = parent;
    /* caching the parent for performance reasons */
    win->parent = parent_public;
    if (!is_root) {
	win->cache_rect.r_left = win->cache_rect.r_top = DEFAULT_X_Y;
	win->cache_rect.r_width = win->cache_rect.r_height = DEFAULT_WIDTH_HEIGHT;
    }

    if (!is_root && !is_sel_window) {
        default_cursor = (Xv_Cursor) xv_get(server, XV_KEY_DATA, WIN_CURSOR);
        if (!default_cursor) {
	    default_cursor = xv_create(screen, CURSOR,
				   CURSOR_SRC_CHAR, OLC_BASIC_PTR,
				   CURSOR_MASK_CHAR, OLC_BASIC_MASK_PTR,
				   /* Can never free */
				   XV_INCREMENT_REF_COUNT,	
				   0);
	    (void) xv_set(server, XV_KEY_DATA, CURSOR_BASIC_PTR, default_cursor, 0);
	    (void) xv_set(server, XV_KEY_DATA, WIN_CURSOR, default_cursor, 0);
        } 
        (void) xv_set(default_cursor, XV_INCREMENT_REF_COUNT, 0);
        win->cursor = (Xv_opaque) default_cursor;
    } else {
	win->cursor = (Xv_opaque)NULL;
    }

    if (depth == 0) {
	depth = (is_root) ? DefaultDepth(display, screen_num)
		    : xv_depth(parent_info);
    }

    /* 
     * Setting WIN_DYNAMIC_VISUAL on this window overrides everything else.
     * If WIN_DYNAMIC_VISUAL is not set on this window, check to see if visual
     * is to be inherited from the parent.
     */
    if ((dynamic_color == TRUE) && (depth > 1)) {
	info->cms = (Xv_Colormap_Seg *) xv_get(screen, SCREEN_DEFAULT_CMS);
	info->bg = XV_STATIC_CMS_BG(info->cms);
	info->fg = XV_STATIC_CMS_FG(info->cms);
	win->inherit_colors = TRUE;
	xv_dynamic_color(info) = TRUE;
    } else if (!(win->top_level) && (parent->inherit_colors)) {
	info->cms = xv_cms(parent_info);
	info->bg = xv_bg(parent_info);
	info->fg = xv_fg(parent_info);
	dynamic_color = xv_dynamic_color(parent_info);
	win->inherit_colors = TRUE;
    } else {
	info->cms = (Xv_Colormap_Seg *) xv_get(screen, SCREEN_DEFAULT_CMS);
	info->bg = XV_STATIC_CMS_BG(info->cms);
	info->fg = XV_STATIC_CMS_FG(info->cms);
    }

    if ((dynamic_color == TRUE) && (depth > 1)) {
	info->dynamic_color = TRUE;
	visual = (Visual *) xv_get(screen, SCREEN_DYNAMIC_VISUAL);
	cmap = (Xv_Colormap *) xv_get(screen, SCREEN_DYNAMIC_CMAP);
    } else {
	cmap = (Xv_Colormap *) info->cms->cms_cmap;
	if (cmap->cmap_type == XV_STATIC_CMAP) {
	    visual = (Visual *) xv_get(screen, SCREEN_STATIC_VISUAL);
	} else {
	    visual = (Visual *) xv_get(screen, SCREEN_DYNAMIC_VISUAL);
	}
    }

    info->xid = window_new(display, screen, visual, cmap->id, depth, 
			parent_info, win);

    /* Default plane mask is all planes. */
    info->plane_mask = ~0;

    /* this is a window, not a bitmap */
    info->is_bitmap = 0;

    info->visual = screen_add_visual(screen, info->xid, depth);
    if (!info->visual) {
	eexit("Window creation failed to get new visual", error_msg);
    }
    if (is_root) {
	/* Patch up the visual's root_window. */
	info->visual->root_window = win_public;
    }
    info->private_gc = FALSE;

    if (is_client_pane) {
	default_fontname = (char *) getenv("XV_CLIENT_PANE_FONT");
    } else {
	default_fontname = NULL;
    }
    win->font = (Pixfont *) xv_get(server,
				   SERVER_FONT_WITH_NAME, default_fontname);
    (void) xv_set(win->font, XV_INCREMENT_REF_COUNT, 0);

    notify_set_destroy_func(win_public, xv_destroy_status);

    /* register with the notifier */
    if (notify_set_event_func(win_public, window_default_event_func,
			      NOTIFY_SAFE) ==
	NOTIFY_FUNC_NULL) {
	eexit("notify_set_event_func failed in window creation", error_msg);
	/*
	 * BUG: do we need this here? (void)notify_remove(win_public);
	 */
    }
    if (notify_set_event_func(win_public, window_default_event_func,
			      NOTIFY_IMMEDIATE) ==
	NOTIFY_FUNC_NULL) {
	eexit("notify_set_event_func failed in window creation", error_msg);
    }

    /* for compatibility */
    if (convert_cu)
	window_scan_and_convert_to_pixels(win_public, avlist);

    return XV_OK;
}


Pkg_private XID
window_new(display, screen, visual, cmap_id, depth, parent_info, win)
    Display        	*display;
    Xv_opaque       	screen;
    Visual         	*visual;
    int                 cmap_id;
    int             	depth;
    Xv_Drawable_info 	*parent_info;
    Window_info 	*win;
{
    Window                  new_window;
    XSetWindowAttributes    attrs;
    Rect            	    rect;
    unsigned long   	    value_mask = 0;     
    Xv_object       	    win_public = win->public_self;
    Xv_Drawable_info        *info;

    DRAWABLE_INFO_MACRO(win_public, info);

    if (!parent_info) {
        new_window = RootWindow(display, (int) xv_get(screen, SCREEN_NUMBER));
        win_x_getrect(display, new_window, &rect);
        window_update_cache_rect(win_public, &rect);
    } else {
        attrs.event_mask = win->xmask;
        value_mask = CWEventMask;
        if (!win->input_only) {
            /* BitGravity/background/Cmap are invalid for InputOnly windows */
            attrs.bit_gravity = NorthWestGravity;
            value_mask |= CWBitGravity;
            attrs.colormap = (cmap_id) ? cmap_id : CopyFromParent;
            value_mask |= CWColormap;
            if (!win->transparent) {
                attrs.background_pixel = xv_bg(info);
                value_mask |= CWBackPixel;
                attrs.border_pixel = xv_fg(info);
                value_mask |= CWBorderPixel;
            }
        }
        if (win->top_level_no_decor) {
            attrs.override_redirect = True;
            value_mask |= CWOverrideRedirect;
        }
        if (win->transparent) {
            value_mask |= CWBackPixmap;
            attrs.background_pixmap = None;
        }
	if (win->cursor) {
	    value_mask |= CWCursor;
	    attrs.cursor = (Cursor)xv_get(win->cursor, XV_XID);
	}
        new_window = XCreateWindow(display,
		        xv_xid(parent_info),
		        win->cache_rect.r_left,
		        win->cache_rect.r_top,
		        win->cache_rect.r_width,
		        win->cache_rect.r_height,
		        win->has_border ? WIN_DEFAULT_BORDER_WIDTH : 0,
		        win->input_only ? 0 : depth,
		        win->input_only ? InputOnly : CopyFromParent,
		        visual ? visual : CopyFromParent,
		        value_mask,
		        &attrs);

    }
    XSaveContext(display, new_window, CONTEXT, win_public);
    return (new_window);
}

Xv_private void
window_set_bit_gravity(win_public, value)
    Xv_Window       win_public;
    int             value;
{
    register Xv_Drawable_info *info;
    XSetWindowAttributes win_attr;

    win_attr.bit_gravity = value;
    DRAWABLE_INFO_MACRO(win_public, info);
    XChangeWindowAttributes(xv_display(info), xv_xid(info), CWBitGravity, &win_attr);

}

void
xv_main_loop(win_public)
    Xv_Window       win_public;
{
    Window_info    *win = WIN_PRIVATE(win_public);
    register Xv_Drawable_info *info;
    Display        *display;
    extern int      sview_gprof_start;

    DRAWABLE_INFO_MACRO(win_public, info);
    display = xv_display(info);
    /*
     * sync with the server to make sure we have all outstanding
     * ConfigureNotify events in the queue. Then process the events, then
     * finally map the frame.
     */
    xv_set(xv_server(info), SERVER_SYNC_AND_PROCESS_EVENTS, 0);


    /* install win in tree */
    (void) win_insert(win_public);

    win->map = TRUE;
    if (xv_get(xv_server(info), SERVER_JOURNALLING)) {	/* server is in
							 * journalling mode */
	xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);
    }
#ifdef GPROF
    {
	int             pend;
	if (sview_gprof_start) {
	    XSync(display, 0);
	    do {
		xv_scheduler_internal();	/* process pending enqueued
						 * events */
		XSync(display, 0);

	    } while (ioctl(display->fd, FIONREAD, (char *) &pend) > sizeof(XEvent));

	    moncontrol(0);
	}
    }
#endif
    XFlush(display);		/* flush anthing left in the buffer */
    (void) notify_start();	/* and then loop in the notifier */
    if (xv_get(xv_default_server, SERVER_JOURNALLING))
	xv_set(xv_default_server, SERVER_JOURNAL_SYNC_EVENT, 1, 0);
}

int				/* bool */
window_done(win)
    Xv_Window       win;
{
    register Xv_Window owner = win;
    register Xv_Window grand_owner;

    /*
     * find the frame for this window. Chase up the owner list until we hit
     * the root window.
     */
    while ((grand_owner = window_get(owner, WIN_OWNER))) {
	/*
	 * remember the current window as the child of the next owner.
	 */
	win = owner;
	owner = grand_owner;
    }
    /*
     * now grand_owner is NULL, owner is a root window, and win is the frame
     * to destroy.
     */
    xv_destroy(win);
    return TRUE;
}

/* A flag to tell us if the parent window is going to be destoryed. */
Pkg_private int
window_get_parent_dying()
{
    return (parent_dying);
}

Pkg_private int
window_set_parent_dying()
{
    parent_dying = TRUE;
}

Pkg_private int
window_unset_parent_dying()
{
    parent_dying = FALSE;
}

Pkg_private int
window_destroy_win_struct(win_public, status)
    register Xv_Window win_public;
    Destroy_status  status;
{
    register Window_info *win = WIN_PRIVATE(win_public);
    register Xv_Drawable_info *info;

    switch (status) {
      case DESTROY_PROCESS_DEATH:
      case DESTROY_CLEANUP:
	/* Decrement the ref count on all ref counted objects */
	if (win->cursor)
	    (void) xv_set(win->cursor, XV_DECREMENT_REF_COUNT, 0);
	if (win->menu)
	    (void) xv_set(win->menu, XV_DECREMENT_REF_COUNT, 0);
	if (win->font)
	    (void) xv_set(win->font, XV_DECREMENT_REF_COUNT, 0);

	DRAWABLE_INFO_MACRO(win_public, info);
	if (xv_cms(info)) {
	    screen_remove_cms(xv_get(win_public, XV_SCREEN), xv_cms(info));
	}
	/*
	 * Remove conditions from notifier.  Could remove just conditions set
	 * in window_init(), but we do everyone a favor here because it is
	 * overwhelmingly the common case.
	 */
	if (status == DESTROY_CLEANUP)
	    (void) notify_remove(win_public);
	if (win->owner && win->owner->layout_proc)
	    (win->owner->layout_proc) (WIN_PUBLIC(win->owner), win_public,
				       WIN_DESTROY);
	/* If this is DESTROY_CLEANUP and the parent window is not going to */
	/* be destroyed, destroy the window.				    */
	if ((status == DESTROY_CLEANUP) && (!window_get_parent_dying())) {
	    win_free(win_public);
	    free(win);
	} else
	    XDeleteContext(xv_display(info), xv_xid(info), 1);
	break;
      default:
	break;
    }
    return XV_OK;
}
