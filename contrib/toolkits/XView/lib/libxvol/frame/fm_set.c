#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fm_set.c 20.46 89/11/20";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <X11/Xlib.h>
#include <xview_private/fm_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/server.h>

#ifdef PRE_R4_ICCCM
#define XIconifyWindow	xv_iconify_window
#endif

static void     frame_change_state();
static short    default_frame_icon_image[256] = {
#include	<images/default.icon>
};

mpr_static(default_frame_icon_mpr, ICON_DEFAULT_WIDTH, ICON_DEFAULT_HEIGHT, 1,
	   default_frame_icon_image);

Pkg_private     Xv_opaque
frame_set_avlist(frame_public, avlist)
    Frame           frame_public;
    Frame_attribute avlist[];
{
    Frame_attribute *attrs;
    Frame_class_info *frame = FRAME_PRIVATE(frame_public);
    Xv_object       owner;
    int             is_subframe;
    int             result = XV_OK;
    static int	    set_icon_rect;
    static Rect	    icon_rect;	

    for (attrs = avlist; *attrs; attrs = frame_attr_next(attrs)) {
	switch (attrs[0]) {
	  case FRAME_CLOSED:
	  case FRAME_CLOSED_RECT:
	    owner = xv_get(frame_public, XV_OWNER);
	    is_subframe = (int) xv_get(owner, XV_IS_SUBTYPE_OF, FRAME_CLASS);
	    switch (attrs[0]) {
	      case FRAME_CLOSED:
		/* subframes can't be iconic */
		if (!is_subframe) {
		    status_set(frame, icon_at_start, (int) attrs[1]);
		    frame_change_state(frame, attrs[1]);
		}
		break;
		/* SunView1 compatibility only */
	      case FRAME_CLOSED_RECT: 
		/* can't set closed rect if subframe */
		if (!is_subframe) {
		    if (frame->icon) {
			Xv_Drawable_info *info;

			DRAWABLE_INFO_MACRO(frame_public, info);

		        (void) win_setrect(frame->icon, (Rect *) attrs[1]);
#ifndef NO_WM
	    		frame->wmhints.flags |= IconPositionHint;
	    		frame->wmhints.icon_x = ((Rect *)attrs[1])->r_left;
	    		frame->wmhints.icon_y = ((Rect *)attrs[1])->r_top;
		        frame_setwmhints(xv_display(info), xv_xid(info), &(frame->wmhints));

#endif
		    } else {
		        set_icon_rect = TRUE;
			icon_rect = *(Rect *) attrs[1];
		    }
		}
		break;
	    }
	    break;

	    /* SunView1 compatibility only */
	  case FRAME_CURRENT_RECT:
	    (void) xv_set((frame_is_iconic(frame)) ? frame->icon : frame_public,
			  WIN_RECT, (Rect *) attrs[1], 0);
	    break;

	  case FRAME_DEFAULT_DONE_PROC:
	    frame->default_done_proc = (void (*) ()) attrs[1];
	    if (!frame->default_done_proc)
		frame->default_done_proc = frame_default_done_func;
	    break;

	  case FRAME_DONE_PROC:
	    frame->done_proc = (void (*) ()) attrs[1];
	    break;

	  case FRAME_NO_CONFIRM:
	    status_set(frame, no_confirm, (int) attrs[1]);
	    break;

	  case FRAME_INHERIT_COLORS:
	    xv_set(frame_public, WIN_INHERIT_COLORS, TRUE, 0);
	    break;

	  case FRAME_FOREGROUND_COLOR:
	    frame_set_color(frame, (struct xv_singlecolor *) attrs[1], NULL);
	    break;

	  case FRAME_BACKGROUND_COLOR:
	    frame_set_color(frame, NULL, (struct xv_singlecolor *) attrs[1]);
	    break;

	  case FRAME_SUBWINDOWS_ADJUSTABLE:
	    status_set(frame, bndrymgr, (int) attrs[1]);
	    break;

	  case FRAME_LEFT_FOOTER:
	    *attrs = (Frame_attribute) ATTR_NOP(*attrs);
	    if (frame->leftfooter)
		free(frame->leftfooter);
	    if ((char *) attrs[1]) {
		frame->leftfooter = (char *)
		    calloc(1, strlen((char *) attrs[1]) + 1);
		strcpy(frame->leftfooter, (char *) attrs[1]);
	    } else {
		frame->leftfooter = NULL;
	    }
	    (void) frame_display_footer(frame);
	    break;

	  case FRAME_RIGHT_FOOTER:
	    *attrs = (Frame_attribute) ATTR_NOP(*attrs);
	    if (frame->rightfooter)
		free(frame->rightfooter);
	    if ((char *) attrs[1]) {
		frame->rightfooter = (char *)
		    calloc(1, strlen((char *) attrs[1]) + 1);
		strcpy(frame->rightfooter, (char *) attrs[1]);
	    } else {
		frame->rightfooter = NULL;
	    }
	    (void) frame_display_footer(frame);
	    break;


	  case FRAME_ICON:
	    {
		unsigned long   icon_id;
		Xv_Drawable_info *info;
		short           sameicon = FALSE;

		if (frame->icon) {
		    if (frame->icon == (Icon) attrs[1])
			/*
			 * this will prevent notifying WM with the same icon
			 * window. The current WM will destroy the window
			 * even though it is the same one.  This will cause
			 * no icon to appear.  If Stuart's WM works
			 * different, then maybe this code can be taken out
			 * and a bug to be filed against generic WM.
			 */
			sameicon = TRUE;
		    else if (frame->icon == frame->default_icon)
			/* Destroy only the default icon */
			xv_destroy(frame->icon);
		}
		frame->icon = (Icon) attrs[1];
		xv_set(frame->icon, XV_OWNER, frame_public, 0);
		icon_id = (unsigned long) xv_get(frame->icon, XV_XID);
#ifdef ATOM_ICON
		win_change_property(frame_public, ATOM_ICON, ATOM_ICON_TYPE,
				    32, &icon_id, 1);
#endif
		if (set_icon_rect) {
		    (void) win_setrect(frame->icon, icon_rect);
		}

#ifndef NO_WM
		if (!sameicon) {
		    frame->wmhints.flags |= IconWindowHint;
		    frame->wmhints.icon_window = icon_id;
		    if (set_icon_rect) {
		        frame->wmhints.flags |= IconPositionHint;
	    	        frame->wmhints.icon_x = icon_rect.r_left;
	    	        frame->wmhints.icon_y = icon_rect.r_top;
		    }
		    DRAWABLE_INFO_MACRO(frame_public, info);
		    frame_setwmhints(xv_display(info), xv_xid(info), &(frame->wmhints));
		}
#endif
		set_icon_rect = FALSE;
		break;
	    }

	  case FRAME_BUSY:{

		status_set(frame, busy, FALSE);
		if ((int) attrs[1]) {
		    frame_display_busy(frame, WMWindowIsBusy);
		    status_set(frame, busy, TRUE);
		} else {
		    frame_display_busy(frame, WMWindowNotBusy);
		    status_set(frame, busy, FALSE);
		}

		break;
	    }

	  case FRAME_SCALE_STATE:
	    /*
	     * set the local rescale state bit, then tell the WM the current
	     * state, and then set the scale of our subwindows
	     */
	    /*
	     * WAIT FOR NAYEEM window_set_rescale_state(frame_public,
	     * attrs[1]);
	     */
	    wmgr_set_rescale_state(frame_public, attrs[1]);
	    frame_rescale_subwindows(frame_public, attrs[1]);
	    break;

	  case XV_SHOW:
	    /*
	     * Map the icon. The FRAME pkg should not let the window package
	     * know about icons and wmgr.
	     */
	    if (frame_is_iconic(frame) && (int) attrs[1]) {
		attrs[0] = (Frame_attribute) WIN_MAP;
		wmgr_top(frame_public);
	    }
	    break;

	  case XV_END_CREATE:{
		unsigned long   icon_id;
		Xv_Drawable_info *info;

		DRAWABLE_INFO_MACRO(frame_public, info);

		owner = xv_get(frame_public, XV_OWNER);
		is_subframe = (int) xv_get(owner, XV_IS_SUBTYPE_OF, FRAME);

		/*
		 * if this is a subframe, we need to reparent it to be a
		 * child of the root window and not of the main frame
		 */
		if (is_subframe) {
		    xv_set(frame_public,
			   WIN_PARENT, xv_get(frame_public, XV_ROOT),
			   0);
		} else if (!frame->icon) {
		    /* Create a default icon for a main frame */
		    if ((xv_depth(info) > 1) && (xv_dynamic_color(info))) {
			/* icon inherits frame color properties */
			frame->icon = frame->default_icon = 
			    xv_create(owner, ICON,
				XV_OWNER, frame_public,
				ICON_IMAGE, &default_frame_icon_mpr,
				WIN_DYNAMIC_VISUAL, TRUE,
				0);
		    } else {
		        frame->icon = frame->default_icon = 
			    xv_create(owner, ICON,
				XV_OWNER, frame_public,
				ICON_IMAGE, &default_frame_icon_mpr,
				0);
		    }

		    icon_id = (unsigned long) xv_get(frame->icon, XV_XID);
#ifdef ATOM_ICON
		    win_change_property(frame_public, ATOM_ICON, ATOM_ICON_TYPE,
					32, &icon_id, 1);
#endif
		    if (set_icon_rect) {
		        (void) win_setrect(frame->icon, icon_rect);
		    }
#ifndef NO_WM
		    frame->wmhints.flags |= IconWindowHint;
		    frame->wmhints.icon_window = icon_id;
		    if (set_icon_rect) {
		        frame->wmhints.flags |= IconPositionHint;
	    	        frame->wmhints.icon_x = icon_rect.r_left;
	    	        frame->wmhints.icon_y = icon_rect.r_top;
		    }
		    frame_setwmhints(xv_display(info), xv_xid(info), &(frame->wmhints));
#endif
		    set_icon_rect = FALSE;
		}
		if (set_icon_rect) {
		    set_icon_rect = FALSE;
		    (void) win_setrect(frame->icon, icon_rect);
#ifndef NO_WM
	    	    frame->wmhints.flags |= IconPositionHint;
	    	    frame->wmhints.icon_x = icon_rect.r_left;
	    	    frame->wmhints.icon_y = icon_rect.r_top;
		    frame_setwmhints(xv_display(info), xv_xid(info), &(frame->wmhints));

#endif
		}

		/*
		 * Deal with command line and set size and position if this
		 * is the first time set is called. Command line only applies
		 * to the first frame.
		 */
		if (frame_notify_count == 1) {
		    /*
		     * set the command line options.  This gives the command
		     * line the highest precedence: default, client attrs,
		     * command line.
		     */
		    if (frame_set_cmdline_options(frame_public, avlist) != XV_OK)
			result = XV_ERROR;
		    if (frame_set_icon_cmdline_options(frame_public, avlist)
			!= XV_OK)
			result = XV_ERROR;
		}
		/*
		 * Now position the frame if its position is not fixed yet.
		 */
		frame_set_position(owner, frame);

		/*
		 * On color check to see if fg & bg have been set. Make icon
		 * inherit frame's fg/bg.
		 */
		if (xv_depth(info) > 1) {
		    frame_check_fg_bg(frame_public);
		    if (frame->icon) {
                        Xv_Colormap_Seg     *cms;

                        /*
                         * Dont override icon's colormap segment if it has been
                         * set programatically.
                         */
                        cms = (Xv_Colormap_Seg *)xv_get(frame->icon, WIN_CMS);
                        if (XV_IS_DEFAULT_CMS(cms) && (cms != xv_cms(info))) {
                            xv_set(frame->icon, WIN_CMS, xv_cms(info), 0);
                        }
                    }
		}
		break;
	    }

	  default:
	    xv_check_bad_attr(&xv_frame_class_pkg, attrs[0]);
	    break;

	}
    }

    return (Xv_opaque) result;
}


/*
 * Change the state of frame to iconic or open.
 */
static void
frame_change_state(frame, to_iconic)
    Frame_class_info *frame;
    int             to_iconic;
{
    Frame           frame_public = FRAME_PUBLIC(frame);
    Xv_Drawable_info *info;

    status_set(frame, iconic, to_iconic);

    DRAWABLE_INFO_MACRO(frame_public, info);

    /* icon is dealt with in wmgr_close/open() */
    if (to_iconic)
	XIconifyWindow(xv_display(info), xv_xid(info), DefaultScreen(xv_display(info)));
    else
	XMapWindow(xv_display(info), xv_xid(info));
}
