#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)win_damage.c% 20.24 89/10/31";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Win_damage.c: Implement the exposure fixup (damage) and clipping control
 * functions of the win_struct.h interface.
 */

#include <stdio.h>
#include <xview_private/draw_impl.h>
#include <xview/window.h>
#include <xview/win_input.h>
#include <xview/pkg.h>
#include <xview/rect.h>
#include <xview/rectlist.h>

extern Xv_object win_data();

static Rectlist damaged;
static XID      damaged_drawable;	/* = 0 == None */
static XID      pending_drawable;	/* = 0 == None */


void
win_clear_damage(window)
    Xv_object       window;
{

    register Xv_Drawable_info *info;
    XID             xid;

    DRAWABLE_INFO_MACRO(window, info);
    xid = xv_xid(info);
    if (xid == pending_drawable) {
	pending_drawable = (XID) 0;
	rl_free(&damaged);
    } else {
	xv_error(window, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, "xid not pending drawable",
		 XV_ERROR_PKG, ATTR_PKG_WIN,
		 0);
    }
}

void
win_set_damage(window, rl)
    Xv_object       window;
    Rectlist        *rl;
{
    register Xv_Drawable_info *info;
 
    DRAWABLE_INFO_MACRO(window, info);
    if (pending_drawable != (XID) 0) { 
        xv_error(window, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,   
                 XV_ERROR_STRING, "Pending drawable has not been cleared", 
                 XV_ERROR_PKG, ATTR_PKG_WIN, 
                 0); 
        return; 
    }                     
    pending_drawable = xv_xid(info); 
    rl_copy(rl, &damaged); 
}

Rectlist       *
win_get_damage(window)
    Xv_object       window;
{
    register Xv_Drawable_info *info;
    XID             xid;

    DRAWABLE_INFO_MACRO(window, info);
    xid = xv_xid(info);
    return ((xid == pending_drawable && !rl_empty(&damaged))
	    ? &damaged : RECTLIST_NULL);
}

Xv_private int
win_convert_to_x_rectlist(rl, xrect_array, xrect_count)
    Rectlist       *rl;
    XRectangle     *xrect_array;
{
    register XRectangle *xrect;
    register Rectnode *rect;

    xrect = xrect_array;
    rect = rl->rl_head;
    if (rl->rl_head == RECTNODE_NULL) {
	/* Treat rl == rl_null as single rectangle of zero dimensions. */
	xrect->x = xrect->y = xrect->width = xrect->height = 0;
	xrect++;
    } else
	for (; xrect < &xrect_array[xrect_count]; rect = rect->rn_next) {
	    xrect->x = rect->rn_rect.r_left;
	    xrect->y = rect->rn_rect.r_top;
	    xrect->width = rect->rn_rect.r_width;
	    xrect->height = rect->rn_rect.r_height;
	    xrect++;		/* Incr. before test to simplify result
				 * computation */
	    if (rect == rl->rl_tail)
		break;
	}
    /* BUG ALERT!  Too many rectangles. */
    if (xrect == &xrect_array[xrect_count]) {
	xv_error(rl, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING, "too many rectangles",
		 XV_ERROR_PKG, ATTR_PKG_WIN,
		 0);
    }
    return ((xrect - xrect_array));
}

Xv_public int
win_set_clip(window, rl)
    register Xv_object window;
    Rectlist       *rl;
/*
 * rl == RECTLIST_NULL is indication to reset clipping, but rl_empty(rl) is
 * indication to set clipping to "nothing visible".
 */
{
    register Xv_Drawable_info *info;
    XRectangle      xrect_array[256];
    int             rect_count;
    Xv_Screen       screen;

    DRAWABLE_INFO_MACRO(window, info);
    screen = xv_screen(info);
    info->new_clipping = TRUE;
    if (rl) {
	rect_count = win_convert_to_x_rectlist(rl, xrect_array, 256);
	screen_set_clip_rects(screen, xrect_array, rect_count);
    } else {
	screen_set_clip_rects(screen, NULL, 0);
    }
    return (0);
}

#ifdef COLLAPSE_MULTIPLE_EXPOSURE_GROUPS
static int
win_do_expose_event_predicate(display, event, args)
    Display        *display;
    XEvent         *event;
    char           *args;
{
    XExposeEvent   *e;

    if (event->type == Expose) {
	e = (XExposeEvent *) event;
	return (e->window == (XID) args);
    }
    return (FALSE);
}

#endif

Pkg_private int
win_do_expose_event(display, event, e, window, collapse_exposures)
    Display        *display;
    Event          *event;
    register XExposeEvent *e;
    Xv_opaque      *window;
    short	    collapse_exposures;
{
    XEvent          queued;
    Rect            rect;

    if (damaged_drawable == None) {
	/* Initialize damaged and damaged_drawable */
	if (pending_drawable != None || damaged.rl_head) {
	    /* BUG: Someone did not clear the pending drawable.  So we 
	     *      clear it for them.  This can happen if they use
	     *      input_readevent() and get an Expose event.  The whole
	     *      idea of a pending_drawable on a per-process basis needs to
	     *      be scraped.
	     */
	    pending_drawable = (XID) 0;
	    rl_free(&damaged);
	}
	damaged = rl_null;
	damaged_drawable = e->window;
    }
    if (e->window == damaged_drawable) {
	queued.type = 0;	/* Flag to detect use of XCheckIfEvent */
More_Damage:
	/* Accumulate damage */
	rect.r_left = e->x;
	rect.r_top = e->y;
	rect.r_width = e->width;
	rect.r_height = e->height;
	rl_rectunion(&rect, &damaged, &damaged);
#ifdef COLLAPSE_MULTIPLE_EXPOSURE_GROUPS
	/*
	 * BUG ALERT! Suppose Server generated events: Configure-1
	 * Exposures-1 ... Configure-2 Exposures-2 Following will result in
	 * WIN_RESIZE for Configure-1, then WIN_REPAINT for all Exposures,
	 * then WIN_RESIZE for Configure-2 and that completely screws up
	 * XView code expecting pairs of WIN_RESIZE followed by
	 * WIN_REPAINT.
	 */
	if ((e->count == 0) || (queued.type != 0)) {
	    /*
	     * Check for another group of exposures enqueued for this window,
	     * and coalesce with them if any found.
	     */
	    if (XCheckIfEvent(display, &queued,
			      win_do_expose_event_predicate,
			      (char *) damaged_drawable)) {
		e = (XExposeEvent *) & queued;
		goto More_Damage;
	    }
	}
#endif
	if ((e->count == 0) || (!collapse_exposures)) {
	    pending_drawable = damaged_drawable;
	    damaged_drawable = None;
	    if (collapse_exposures) { /* Pass the bounding box of the damage */
				      /* to the client.                      */
		e->x = damaged.rl_bound.r_left;
		e->y = damaged.rl_bound.r_top;
		e->width = damaged.rl_bound.r_width;
		e->height = damaged.rl_bound.r_height;
	    }
	} else {
	    return (TRUE);
	}
    } else {
	/* Send damaged_drawable the partial damage now. */
	pending_drawable = damaged_drawable;
	*window = win_data(display, pending_drawable);
	damaged_drawable = None;
	XPutBackEvent(display, e);
#ifdef _XV_DEBUG
	fprintf(stderr,
		"Partial damage switch: old %X, new %X\n",
		pending_drawable, e->window);
#endif
    }
    return (FALSE);
}

#ifdef _XV_DEBUG
static int
win_expose_status_predicate(display, event, args)
    Display        *display;
    XEvent         *event;
    char           *args;
{
    XExposeEvent   *e;

    if (event->type == Expose) {
	e = (XExposeEvent *) event;
	return (e->window == (XID) args);
    }
    return (FALSE);
}

win_expose_status(display, d)
    Display        *display;
    XID             d;
{
    XEvent          event;

    if (d == pending_drawable) {
	fprintf(stderr, "Specified drawable is current pending_drawable.\n");
    }
    if (d == damaged_drawable) {
	fprintf(stderr, "Specified drawable is current damaged_drawable.\n");
    }
    if (XCheckIfEvent(display, &event, win_expose_status_predicate,
		      (char *) d)) {
	XPutBackEvent(display, &event);
	fprintf(stderr, "There are enqueued exposures.\n");
    } else {
	fprintf(stderr, "No enqueued exposures.\n");
    }
    fflush(stderr);
}

#endif
