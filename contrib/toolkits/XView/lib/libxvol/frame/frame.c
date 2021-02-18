#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)frame.c 20.55 89/10/17";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <xview_private/fm_impl.h>
#include <xview_private/frame_cmd.h>
#include <xview_private/frame_base.h>
#include <xview/server.h>
#include <xview_private/draw_impl.h>




/*
 * only called by xv_main_loop, to determine whether or not the frame should
 * be iconic initially
 */
Xv_private
frame_iconic_initially(frame_public)
    Frame           frame_public;
{
    return (status_get(FRAME_PRIVATE(frame_public), icon_at_start));
}

Xv_private void
frame_cmdline_help(name)
    char           *name;
{
    xv_usage(name);
}

static int
check_for_configure_event(display, xevent, args)
    Xv_object		display;
    XEvent		*xevent;
    char		*args;
{
    return ((xevent->type & 0177) == ConfigureNotify);
}


extern void
frame_get_rect(frame, rect)
    Frame           frame;
    Rect           *rect;
{
    
    XWindowAttributes	xwin_attr;
    Xv_Drawable_info	*info;      
    XID   		win_xid;
    register Xv_object	display;
    int			status;
    Atom		*atom;
    Atom		type;
    int			format;
    long		nitems;
    long		bytes;
    int			*prop;
    int			top, bottom, left, right;
    int			has_footer = FALSE;

    
    if (xv_get(frame, WIN_TYPE) != (Xv_opaque)FRAME_TYPE)
        return;
        
    DRAWABLE_INFO_MACRO(frame, info);
    display = (Xv_object)xv_display(info);
    win_xid = xv_xid(info);

    atom =(Atom *)xv_get(xv_server(info), SERVER_WM_BORDER_SIZES);


    status = XGetWindowProperty(display, win_xid, atom,
				0, 5, False, XA_INTEGER,
				&type, &format, &nitems, &bytes, &prop);

    if ((status != Success) || (!prop)) {
	XEvent		xevent;
	
	xevent.type = ConfigureNotify;

	if (XCheckIfEvent(display, &xevent, 
			  check_for_configure_event, NULL)) {
            status = XGetWindowProperty(display, win_xid, atom,
				0, 4, False, XA_INTEGER,
				&type, &format, &nitems, &bytes, &prop);
	}
    }
    
    if ((int) xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_BASE)) {
        Frame_base_info	*frame_private = FRAME_BASE_PRIVATE(frame);
        has_footer = status_get(frame_private, show_footer);        
    } else if ((int) xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_CMD)) {
        Frame_cmd_info	*frame_private = FRAME_CMD_PRIVATE(frame);
        has_footer = status_get(frame_private, show_footer);
    }
        
    if (prop) {
        top = prop[0];
        bottom = has_footer ? prop[4] : prop[1];
        left = prop[2];
        right = prop[3];
    } else {
	/* This is a gross hack for working with OLWM */
        top = 28;
        bottom = has_footer ? 22 : 14;
        left = right = 6;
    }
        
    XGetWindowAttributes(display,  win_xid, &xwin_attr);
    
    if (xwin_attr.map_state == IsViewable) {
        int		x, y;
        XID		dummy;
        XTranslateCoordinates(display, win_xid, 
        		      (XID) xv_get(xv_root(info), XV_XID), 
        		      0, 0, &x, &y, &dummy);
        xwin_attr.x = x - left;
        xwin_attr.y = y - top;
    }
    			  
    rect->r_left = xwin_attr.x;
    rect->r_top = xwin_attr.y;
    rect->r_width = xwin_attr.width + left + right;
    rect->r_height = xwin_attr.height + top + bottom;  
}




extern void
frame_set_rect(frame, rect)
    Frame           frame;
    Rect           *rect;
{
    
    Xv_Drawable_info	*info;
    XID   		win_xid;
    register Xv_object	display; 
    int			status;
    Atom		*atom;
    Atom		type;
    int			format;
    long		nitems;
    long		bytes;
    int			*prop;
    int			top, bottom, left, right;
    int			has_footer = FALSE;

    
    
    if (xv_get(frame, WIN_TYPE) != (Xv_opaque)FRAME_TYPE)
        return;
        
    DRAWABLE_INFO_MACRO(frame, info);
    display = (Xv_object)xv_display(info); 
    win_xid = xv_xid(info);
    atom = (Atom *)xv_get(xv_server(info), SERVER_WM_BORDER_SIZES);

    status = XGetWindowProperty(display, win_xid, atom,
				0, 5, False, XA_INTEGER,
				&type, &format, &nitems, &bytes, &prop);

    if ((status != Success) || (!prop)) {
	XEvent		xevent;
	
	xevent.type = ConfigureNotify;

	if (XCheckIfEvent(display, &xevent, 
			  check_for_configure_event, NULL)) {
            status = XGetWindowProperty(display, win_xid, atom,
				0, 4, False, XA_INTEGER,
				&type, &format, &nitems, &bytes, &prop);
	}
    }
    
    if ((int) xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_BASE)) {
            Frame_base_info	*frame_private = FRAME_BASE_PRIVATE(frame);
            has_footer = status_get(frame_private, show_footer);        
    } else if ((int) xv_get(frame, XV_IS_SUBTYPE_OF, FRAME_CMD)) {
            Frame_cmd_info	*frame_private = FRAME_CMD_PRIVATE(frame);
            has_footer = status_get(frame_private, show_footer);
    }
    
    if (prop) {
        top = prop[0];
        bottom = has_footer ? prop[4] : prop[1];
        left = prop[2];
        right = prop[3];
    } else {
	/* This is a gross hack for working with OLWM */
        top = 28;
        bottom = has_footer ? 22 : 14;
        left = right = 6;
    }


    /*
     *  This is setting the frame x, y relative to the root,
     *  and the width and height adjust for the outer rect.
     *  This will result in setting the window decor with the
     *  correct rect. 
     */
    XMoveResizeWindow(xv_display(info),  xv_xid(info), 
      rect->r_left,
      rect->r_top,
      rect->r_width - (left + right), 
      rect->r_height - (top + bottom));

    
}
