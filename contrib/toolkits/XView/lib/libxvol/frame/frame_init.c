#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)frame_init.c 1.11 89/11/28";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <xview_private/draw_impl.h>
#include <xview/cursor.h>
#include <xview_private/fm_impl.h>
#include <xview/server.h>
#include <xview_private/svr_atom.h>

extern Attr_avlist attr_find();

/*
 * This counter is incremented each time a frame is created and decremented
 * each time a frame is destroyed.  When the value reaches zero, the notifier
 * is stopped, see frame_free().
 */
/* Pkg_private */
int             frame_notify_count = 0;


Pkg_private void
frame_default_done_func(frame)
    Frame           frame;
{
    (void) window_set(frame, XV_SHOW, FALSE, 0);
}


Pkg_private int
frame_init(owner, frame_public, avlist)
    Xv_Window       owner;
    Frame           frame_public;
    Attr_avlist	    avlist;
{
    static          pid;
    Xv_frame_class *frame_object = (Xv_frame_class *) frame_public;
    Frame_class_info *frame;
    int             is_subframe = owner &&
    xv_get(owner, XV_IS_SUBTYPE_OF, FRAME_CLASS);
    Attr_avlist	    attrs;
    Menu            menu;
    XSizeHints      normal_hints;
#ifndef NO_WM
    Xv_Drawable_info *owner_info;
    Xv_Drawable_info *info;
    Atom            property_array[3];

    DRAWABLE_INFO_MACRO(frame_public, info);
#endif

    frame = xv_alloc(Frame_class_info);

    /* link to object */
    frame_object->private_data = (Xv_opaque) frame;
    frame->public_self = frame_public;

    /* Tell window manager to leave our input focus alone */
    frame->wmhints.input = FALSE;
    frame->wmhints.flags |= InputHint;
    normal_hints.flags = 0;

    property_array[0] = (Atom) xv_get(xv_server(info), SERVER_WM_TAKE_FOCUS);
    property_array[1] = (Atom) xv_get(xv_server(info), SERVER_WM_DELETE_WINDOW);
    property_array[2] = (Atom) xv_get(xv_server(info), SERVER_WM_SAVE_YOURSELF);

    win_change_property(frame_public, SERVER_WM_PROTOCOLS, XA_ATOM, 32, property_array, 3);

    /* set default frame flags */


    if (is_subframe) {
#ifdef SUNVIEW1
	/* Mark this window as subframe */
	user_flags = (win_getuserflags(win_fd) | WMGR_SUBFRAME);
	win_setuserflags(win_fd, user_flags);
#endif
#ifndef NO_WM
	DRAWABLE_INFO_MACRO(owner, owner_info);
	frame->wmhints.window_group = xv_xid(owner_info);
#endif
	normal_hints.flags = PPosition | PSize;
    } else {
	/*
	 * Need to register pid w/ the server as a server property, mainly
	 * for toolplaces.
	 */
	if (!pid)
	    pid = getpid();

#ifdef ATOM_ICON
	win_change_property(frame_public, ATOM_PID, ATOM_PID_TYPE, 32, &pid, 1);
#endif

#ifndef NO_WM
	frame->wmhints.window_group = xv_xid(info);
	if (getenv("XV_CLIENT_ICON_X") != NULL) {
	    frame->wmhints.icon_x = atoi(getenv("XV_CLIENT_ICON_X"));
	    frame->wmhints.flags |= IconPositionHint;
	}
	if (getenv("XV_CLIENT_ICON_Y") != NULL) {
	    frame->wmhints.icon_y = atoi(getenv("XV_CLIENT_ICON_Y"));
	}
	
#endif
	if (getenv("XV_CLIENT_PANE_POSITION") != NULL) {
	    normal_hints.flags = USPosition;
	    unsetenv("XV_CLIENT_PANE_POSITION");
	} 

	if (getenv("XV_CLIENT_PANE_SIZE") != NULL) {
	    normal_hints.flags |= USSize;
	    unsetenv("XV_CLIENT_PANE_SIZE");
	} else
	    normal_hints.flags |= PSize;
    }
    frame->wmhints.flags |= WindowGroupHint;

    frame->default_done_proc = frame_default_done_func;

    /*
     * OPEN LOOK change: no cofirmer by default
     */
    status_set(frame, no_confirm, (int) TRUE);

    if (getenv("XV_CLIENT_ICONIC")) {
	    status_set(frame, icon_at_start, TRUE);
#ifndef NO_WM
	    frame->wmhints.initial_state = IconicState;
	    frame->wmhints.flags |= StateHint;
#endif
    }

    /*
     * Iconic state and name stripe have to be determined before any size
     * parameters are interpreted, so the attribute list is mashed and
     * explicitly interrogated for them here.
     */
    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) *attrs) {

	  case FRAME_CLOSED:
	    status_set(frame, icon_at_start, (int) attrs[1]);
	    status_set(frame, iconic, (int) attrs[1]);
#ifndef NO_WM
	    frame->wmhints.initial_state = ((int) attrs[1] ?
						   IconicState : NormalState);
	    frame->wmhints.flags |= StateHint;
#endif
	    ATTR_CONSUME(*attrs);
	    break;
	  case XV_X:
	  case XV_Y:
	  case XV_RECT:
	    normal_hints.flags |= PPosition;
	    break;
	  default:
	    break;
	}
    }

    (void) xv_set(frame_public,
		  XV_SHOW, FALSE,
		  WIN_LAYOUT_PROC, frame_layout,
		  WIN_NOTIFY_SAFE_EVENT_PROC, frame_input,
		  WIN_NOTIFY_IMMEDIATE_EVENT_PROC, frame_input,
		  WIN_TOP_MARGIN, FRAME_BORDER_WIDTH,
		  WIN_BOTTOM_MARGIN, FRAME_BORDER_WIDTH,
		  WIN_LEFT_MARGIN, FRAME_BORDER_WIDTH,
		  WIN_RIGHT_MARGIN, FRAME_BORDER_WIDTH,
		  WIN_ROWS, 34,
		  WIN_COLUMNS, 80,

    /*
     * clear rect info since we are the ones setting (rows, columns). this
     * will let us check later (frame_set_avlist()) if the client has set the
     * position or size.
     */

		  WIN_RECT_INFO, 0,

		  WIN_CONSUME_EVENTS,
		  WIN_MAP_NOTIFY, WIN_UNMAP_NOTIFY,
		  0,
		  WIN_IGNORE_EVENT, WIN_REPAINT,
		  0);

    /* Set cached version of rect */
    (void) win_getsize(frame_public, &frame->rectcache);

    if (!is_subframe) {
	Xv_Drawable_info *info;
	int             frame_count;
	DRAWABLE_INFO_MACRO(frame_public, info);

	frame_count = (int) xv_get(xv_server(info), XV_KEY_DATA, FRAME_COUNT);
	xv_set(xv_server(info), XV_KEY_DATA, FRAME_COUNT, ++frame_count, 0);
    }

    /* Initialise default foreground and background */
    frame->fg.red = frame->fg.green = frame->fg.blue = 0;
    frame->bg.red = frame->bg.green = frame->bg.blue = 255;

    /* created another frame */
    frame_notify_count++;

#ifndef NO_WM
    XSetWMHints(xv_display(info), xv_xid(info), &(frame->wmhints));

		      /* Use old XSetNormalHints function for non-ICCCM wm's */
    if (!defaults_get_boolean("xview.icccmcompliant",
			      "XView.ICCCMCompliant", TRUE))
    	XSetNormalHints(xv_display(info), xv_xid(info), &normal_hints);
    else
#ifndef PRE_R4_ICCCM
    	XSetWMNormalHints(xv_display(info), xv_xid(info), &normal_hints);
#else   PRE_R4_ICCCM
    	XSetNormalHints(xv_display(info), xv_xid(info), &normal_hints);
#endif  PRE_R4_ICCCM

#endif
    return XV_OK;

/*  Failure:
    free((char *) frame);
    return XV_ERROR;  */
}
