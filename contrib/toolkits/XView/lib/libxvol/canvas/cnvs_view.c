#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)cnvs_view.c 20.30 89/11/26";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/cnvs_impl.h>
#include <xview_private/win_keymap.h>
#include <xview/scrollbar.h>
#include <xview/rect.h>
#include <xview/rectlist.h>

static          canvas_view_create_paint_window();

/* ARGSUSED */
Pkg_private int
canvas_view_init(parent, view_public, avlist)
    Canvas          parent;
    Canvas_view     view_public;
    Attr_attribute  avlist[];
{
    Xv_canvas_view *view_object = (Xv_canvas_view *) view_public;
    Canvas_view_info *view;
    int             ret;

    view = xv_alloc(Canvas_view_info);

    /* link to object */
    view_object->private_data = (Xv_opaque) view;
    view->public_self = view_public;
    view->private_canvas = CANVAS_PRIVATE(parent);

    if ((ret = canvas_view_create_paint_window(view)) != XV_OK) {
	xv_free(view);
	return (ret);
    }
    xv_set(view_public,
	   WIN_NOTIFY_SAFE_EVENT_PROC, canvas_view_event,
	   WIN_NOTIFY_IMMEDIATE_EVENT_PROC, canvas_view_event,
	   WIN_CONSUME_PICK_EVENTS,
	   WIN_RESIZE, 0,
	   0);

    return XV_OK;
}

/*ARGSUSED*/ /*VARARGS*/
Pkg_private Xv_opaque
canvas_view_get(view_public, stat, attr, valist)
    Canvas_view     view_public;
    int            *stat;
    Attr_attribute  attr;
    va_list         valist;
{
    Canvas_view_info *view = CANVAS_VIEW_PRIVATE(view_public);

    *stat = XV_OK;

    switch (attr) {
      case CANVAS_VIEW_PAINT_WINDOW:
	return ((Xv_opaque) view->paint_window);

      case CANVAS_VIEW_CANVAS_WINDOW:
	return ((Xv_opaque) CANVAS_PUBLIC(view->private_canvas));

      default:
	xv_check_bad_attr(&xv_canvas_view_pkg, attr);
	*stat = XV_ERROR;
	return (Xv_opaque) 0;
    }
}


/*ARGSUSED*/ /*VARARGS*/
Pkg_private Xv_opaque
canvas_paint_get(paint_public, stat, attr, valist)
    Canvas_paint_window paint_public;
    int            *stat;
    Attr_attribute  attr;
    va_list         valist;
{
    Canvas_view_info *view;
    Canvas_info      *canvas;
    
    switch (attr) {
      case CANVAS_PAINT_CANVAS_WINDOW:
	canvas = (Canvas_info *) xv_get(paint_public,
					XV_KEY_DATA, canvas_context_key);
	return (Xv_opaque) CANVAS_PUBLIC(canvas);

      case CANVAS_PAINT_VIEW_WINDOW:
	view = (Canvas_view_info *) xv_get(paint_public,
					   XV_KEY_DATA, canvas_view_context_key);
	return (Xv_opaque) CANVAS_VIEW_PUBLIC(view);
      default:
	xv_check_bad_attr(&xv_canvas_pw_pkg, attr);
	*stat = XV_ERROR;
	return (Xv_opaque) 0;
    }
}

Pkg_private Xv_opaque
canvas_paint_set(paint_public, avlist)
    Canvas_paint_window paint_public;
    Attr_avlist		avlist;
{
    Attr_attribute	attr;
    Canvas_info    	*canvas;
    Xv_opaque           result = XV_OK;

    for (attr = avlist[0]; attr;
         avlist = attr_next(avlist), attr = avlist[0]) {
        switch (attr) {
	    case WIN_CMS_CHANGE: 
	   	canvas = (Canvas_info *) xv_get(paint_public,
                     			    XV_KEY_DATA, canvas_context_key);
                if (status(canvas, cms_repaint)) {
                    Rect                rect;
                    Rectlist    rl;

                    canvas = (Canvas_info *) xv_get(paint_public,
                                                XV_KEY_DATA, canvas_context_key);
                    rect.r_left = 0;
                    rect.r_top = 0;
                    rect.r_width = (short)xv_get(paint_public, WIN_WIDTH);
                    rect.r_height = (short)xv_get(paint_public, WIN_HEIGHT);
                    rl = rl_null;
                    rl_rectunion(&rect, &rl, &rl);

                    win_set_damage(paint_public, &rl);
                    canvas_inform_repaint(canvas, paint_public);
                    win_clear_damage(paint_public);
                }    
                break;
            
	    default:
		xv_check_bad_attr(&xv_canvas_pw_pkg, attr);
		break;
	}
    }
    return(result);
}
		


Pkg_private int
canvas_view_destroy(view_public, stat)
    Canvas_view     view_public;
    Destroy_status  stat;
{
    Canvas_view_info *view = CANVAS_VIEW_PRIVATE(view_public);

    if ((stat == DESTROY_CLEANUP) || (stat == DESTROY_PROCESS_DEATH)) {
	if (xv_destroy_status(view->paint_window, stat) != XV_OK) {
	    return XV_ERROR;
	}
	if (stat == DESTROY_CLEANUP)
	    free((char *) view);
    }
    return XV_OK;
}

static int
canvas_view_create_paint_window(view)
    Canvas_view_info *view;

{
    Canvas_view     view_public = CANVAS_VIEW_PUBLIC(view);
    Canvas_info    *canvas = view->private_canvas;
    Canvas          canvas_public = CANVAS_PUBLIC(canvas);
    Xv_Window       split_paint;
    Scrollbar       sb;

    if (canvas->width == 0) {
	canvas->width = (int) xv_get(view_public, WIN_WIDTH);
    }
    if (canvas->height == 0) {
	canvas->height = (int) xv_get(view_public, WIN_HEIGHT);
    }
    if (canvas->paint_avlist == NULL) {
	view->paint_window = xv_create(view_public, CANVAS_PAINT_WINDOW,
				       WIN_WIDTH, canvas->width,
				       WIN_HEIGHT, canvas->height,
			     WIN_NOTIFY_SAFE_EVENT_PROC, canvas_paint_event,
			WIN_NOTIFY_IMMEDIATE_EVENT_PROC, canvas_paint_event,
				     WIN_RETAINED, status(canvas, retained),
			       WIN_X_PAINT_WINDOW, status(canvas, x_canvas),
				    XV_KEY_DATA, canvas_context_key, canvas,
				    XV_KEY_DATA, canvas_view_context_key, view,
				       0);
    } else {
	view->paint_window = xv_create(view_public, CANVAS_PAINT_WINDOW,
				       ATTR_LIST, canvas->paint_avlist,
				       WIN_WIDTH, canvas->width,
				       WIN_HEIGHT, canvas->height,
			     WIN_NOTIFY_SAFE_EVENT_PROC, canvas_paint_event,
			WIN_NOTIFY_IMMEDIATE_EVENT_PROC, canvas_paint_event,
				     WIN_RETAINED, status(canvas, retained),
			       WIN_X_PAINT_WINDOW, status(canvas, x_canvas),
				    XV_KEY_DATA, canvas_context_key, canvas,
				    XV_KEY_DATA, canvas_view_context_key, view,
				       0);
	xv_free(canvas->paint_avlist);
	canvas->paint_avlist = canvas->paint_end_avlist = NULL;
    }

    if (view->paint_window == NULL) {
	return ((int) XV_ERROR);
    }
    if (status(canvas, created)) {
	split_paint = (Xv_Window) xv_get(canvas_public, CANVAS_NTH_PAINT_WINDOW, 0);
	if (split_paint != NULL) {
	    /* inherit certain attributes from the split window */

	    xv_set(view->paint_window,
		   WIN_BACKGROUND_COLOR, xv_get(split_paint, WIN_BACKGROUND_COLOR),
		   0);
	    
/* BUG: you can't xv_get this attribute from the split window
	    xv_set(view->paint_window,
		   WIN_BIT_GRAVITY, xv_get(split_paint, WIN_BIT_GRAVITY),
		   0);
*/
	    xv_set(view->paint_window,
		   WIN_COLUMN_GAP, xv_get(split_paint, WIN_COLUMN_GAP),
		   0);
	    
	    xv_set(view->paint_window,
		   WIN_COLUMN_WIDTH, xv_get(split_paint, WIN_COLUMN_WIDTH),
		   0);

	    xv_set(view->paint_window,
		   WIN_CURSOR, xv_get(split_paint, WIN_CURSOR),
		   0);

	    xv_set(view->paint_window,
		   WIN_EVENT_PROC, xv_get(split_paint, WIN_EVENT_PROC),
		   0);

	    xv_set(view->paint_window,
		   WIN_FOREGROUND_COLOR, xv_get(split_paint, WIN_FOREGROUND_COLOR),
		   0);

/* BUG: you can't xv_get this attribute from the split window
	    xv_set(view->paint_window,
		   WIN_IS_CLIENT_PANE, xv_get(split_paint, WIN_IS_CLIENT_PANE),
		   0);
*/
	    xv_set(view->paint_window,
		   WIN_ROW_GAP, xv_get(split_paint, WIN_ROW_GAP),
		   0);
	    
	    xv_set(view->paint_window,
		   WIN_ROW_HEIGHT, xv_get(split_paint, WIN_ROW_HEIGHT),
		   0);

/* BUG: you can't xv_get this attribute from the split window
	    xv_set(view->paint_window,
		   WIN_WINDOW_GRAVITY, xv_get(split_paint, WIN_WINDOW_GRAVITY),
		   0);
*/
	    
	    xv_set(view->paint_window,
		   WIN_X_EVENT_MASK, xv_get(split_paint, WIN_X_EVENT_MASK),
		   0);

	    sb = (Scrollbar) xv_get(canvas_public, OPENWIN_VERTICAL_SCROLLBAR, view_public);
	    if (sb != NULL) {
		canvas_scroll(view->paint_window, sb);
	    }
	    sb = (Scrollbar) xv_get(canvas_public, OPENWIN_HORIZONTAL_SCROLLBAR, view_public);
	    if (sb != NULL) {
		canvas_scroll(view->paint_window, sb);
	    }
	}
    } else {
	xv_set(view->paint_window,
	       WIN_CONSUME_EVENTS,
	           KBD_USE,
	           KBD_DONE,
	           WIN_ASCII_EVENTS,
	           ACTION_HELP,
	           WIN_MOUSE_BUTTONS,
	           0,
	       0);
	status_set(canvas, created);
    }

    return (XV_OK);
}
