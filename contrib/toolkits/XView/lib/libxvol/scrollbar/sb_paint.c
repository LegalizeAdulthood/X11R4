#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb_paint.c 1.43 89/11/14";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb_paint.c
 * 
 * Description:
 * 
 * Maps events into actions
 * 
 */


/*
 * Include files:
 */

#include <xview_private/sb_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/rectlist.h>
#include <xview/window.h>
#include <xview/svrimage.h>
#include <xview_private/xv_color.h>
#include <X11/Xlib.h>

#define	BITWIDTH	16
#define	BITHEIGHT	16

/* 50% gray starting with white pixel -- converted from short to char */
static char     gray_offset_pattern[] = {
    0x54, 0x55, 0xaa, 0xaa, 0x54, 0x55, 0xaa, 0xaa, 0x54, 0x55, 0xaa, 0xaa,
    0x54, 0x55, 0xaa, 0xaa, 0x54, 0x55, 0xaa, 0xaa, 0x54, 0x55, 0xaa, 0xaa,
    0x54, 0x55, 0xaa, 0xaa, 0x54, 0x55, 0xaa, 0xaa
};

/* 50% gray starting with black pixel -- converted from short to char */
static char     gray_normal_pattern[] = {
    0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
    0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55,
    0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55
};

/*
 * Declaration of Functions Defined in This File (in order):
 */

Xv_public void  scrollbar_paint();

Pkg_private void scrollbar_clear_damage();
Pkg_private void scrollbar_paint_elevator_move();
Pkg_private void scrollbar_invert_region();
Pkg_private void scrollbar_paint_page_number();

static void     scrollbar_elevator_paint();
static void     scrollbar_invert_anchor();
static void     scrollbar_paint_cable();
static void     scrollbar_paint_marker();
static int      scrollbar_invert_elevator_region();

/*
 * these four GCs are setup at initialization an used as needed. Note that
 * these don't pickup changes that the user may make to the external
 * environment, meaning color changes are not recalculated when the colors
 * get changed. Note also that the terms `white' and `black' are used to mean
 * foreground and background..
 */
static GC       scrollGC_offset;/* offset gray Stipple */
static GC       scrollGC_normal;/* normal gray Stipple */
static GC       scrollGC_black;	/* `black' foreground  */
static short    firsttime = TRUE;	/* alloc only firsttime */

/******************************************************************/


Xv_public void
scrollbar_paint(sb_public)
    Scrollbar sb_public;
{
    Xv_scrollbar_info *sb = SCROLLBAR_PRIVATE(sb_public);
    Rect            r;

    /* Paint top cable anchor */
    scrollbar_to_start_marker_rect(sb, &r);
    scrollbar_paint_marker(sb, &r, sb->top_anchor_glyph);


    /* Calculate correct position of elevator.  Paint cable and elevator. */
    scrollbar_position_elevator(sb, TRUE, TRUE);

    /* Paint bottom cable anchor */
    scrollbar_to_end_marker_rect(sb, &r);
    scrollbar_paint_marker(sb, &r, sb->bottom_anchor_glyph);
}

Pkg_private void
scrollbar_clear_damage(window, rl)
    Xv_Window       window;
    Rectlist       *rl;
{
    if (rl) {
	scrollbar_clear_rect(window, &rl->rl_bound);
    }
}

static void
scrollbar_elevator_paint(sb)
    Xv_scrollbar_info *sb;
{
    Rect            r;

    scrollbar_elevator_rect(sb, &r);
    sb_normalize_rect(sb, &r);
    sb_render_image(sb, r.r_left, r.r_top, r.r_width, r.r_height,
		    sb->elevator_glyph);
}

Pkg_private void
scrollbar_paint_elevator_move(sb, new_top, dragging_elevator)
    Xv_scrollbar_info *sb;
    int             new_top;
    int             dragging_elevator;	/* TRUE or FALSE */
{
    Rect            r, r2;
    Scrollbar       scrollbar_public = SCROLLBAR_PUBLIC(sb);
    int		    top_diff;

    scrollbar_elevator_rect(sb, &r);

    if (sb->elevator_top > new_top) {
	r.r_top = new_top + r.r_height;
	r.r_height = sb->elevator_top - new_top;
    } else {

	r.r_top = sb->elevator_top;
	top_diff = new_top - sb->elevator_top;
	/*
	 * Note - XClearArea() assumes zero height to be the entire height 
	 * 	  of the window minus the y offset.
	 */
	if (top_diff && (top_diff < r.r_height)) {
	    r.r_height = top_diff;
	}
    }

    r2 = r;
    sb_normalize_rect(sb, &r2);
    sb->elevator_top = new_top;

    /*
     * Clear the rectangle containing the portion of elevator that will now
     * be cable.
     */
    scrollbar_clear_rect(scrollbar_public, &r2);

    /* Repaint cable */
    scrollbar_paint_cable(sb, dragging_elevator);

    /* Paint the elevator */
    scrollbar_elevator_paint(sb);
}

Pkg_private void
scrollbar_invert_region(sb, motion)
    Xv_scrollbar_info *sb;
    Scroll_motion   motion;
{

    if (motion == SCROLLBAR_TO_START || motion == SCROLLBAR_TO_END) {
	scrollbar_invert_anchor(sb, motion);
    } else {
	scrollbar_invert_elevator_region(sb, motion);
    }
}

static void
scrollbar_invert_anchor(sb, motion)
    Xv_scrollbar_info *sb;
    Scroll_motion   motion;
{
    char           *anchor_glyph;
    Rect            r;

    switch (motion) {
      case SCROLLBAR_TO_END:
	anchor_glyph = &sb->bottom_anchor_glyph;
	scrollbar_to_end_marker_rect(sb, &r);
	break;
      case SCROLLBAR_TO_START:
	anchor_glyph = &sb->top_anchor_glyph;
	scrollbar_to_start_marker_rect(sb, &r);
	break;
    }
    if (sb->direction == SCROLLBAR_VERTICAL) {
	if (*anchor_glyph == OLG_VSB_ANCHOR)
	    *anchor_glyph = OLG_VSB_ANCHOR_INVERTED;
	else
	    *anchor_glyph = OLG_VSB_ANCHOR;
    } else {
	if (*anchor_glyph == OLG_HSB_ANCHOR)
	    *anchor_glyph = OLG_HSB_ANCHOR_INVERTED;
	else
	    *anchor_glyph = OLG_HSB_ANCHOR;
    }
    scrollbar_paint_marker(sb, &r, *anchor_glyph);
}

static int
scrollbar_invert_elevator_region(sb, motion)
    Xv_scrollbar_info *sb;
    Scroll_motion   motion;
{

    if (sb->reduced) {
	switch (sb->direction) {
	  case SCROLLBAR_VERTICAL:
	    switch (motion) {
	      case SCROLLBAR_LINE_FORWARD:
		if (sb->elevator_glyph == OLG_VSB_REDUCED_ELEVATOR)
		    sb->elevator_glyph = OLG_VSB_REDUCED_ELEVATOR_LINE_FORWARD;
		else
		    sb->elevator_glyph = OLG_VSB_REDUCED_ELEVATOR;
		break;
	      case SCROLLBAR_LINE_BACKWARD:
		if (sb->elevator_glyph == OLG_VSB_REDUCED_ELEVATOR)
		    sb->elevator_glyph = OLG_VSB_REDUCED_ELEVATOR_LINE_BACKWARD;
		else
		    sb->elevator_glyph = OLG_VSB_REDUCED_ELEVATOR;
		break;
	      case SCROLLBAR_ABSOLUTE:
		/* There is no absolute region in a reduced elevator. */
		break;
	    }
	    break;
	  case SCROLLBAR_HORIZONTAL:
	    switch (motion) {
	      case SCROLLBAR_LINE_FORWARD:
		if (sb->elevator_glyph == OLG_HSB_REDUCED_ELEVATOR)
		    sb->elevator_glyph = OLG_HSB_REDUCED_ELEVATOR_LINE_FORWARD;
		else
		    sb->elevator_glyph = OLG_HSB_REDUCED_ELEVATOR;
		break;
	      case SCROLLBAR_LINE_BACKWARD:
		if (sb->elevator_glyph == OLG_HSB_REDUCED_ELEVATOR)
		    sb->elevator_glyph = OLG_HSB_REDUCED_ELEVATOR_LINE_BACKWARD;
		else
		    sb->elevator_glyph = OLG_HSB_REDUCED_ELEVATOR;
		break;
	      case SCROLLBAR_ABSOLUTE:
		/* There is no absolute region in a reduced elevator. */
		break;
	    }
	    break;
	}
    } else {
	switch (sb->direction) {
	  case SCROLLBAR_VERTICAL:
	    switch (motion) {
	      case SCROLLBAR_LINE_FORWARD:
		if (sb->elevator_glyph == OLG_VSB_ELEVATOR)
		    sb->elevator_glyph = OLG_VSB_ELEVATOR_LINE_FORWARD;
		else
		    sb->elevator_glyph = OLG_VSB_ELEVATOR;
		break;
	      case SCROLLBAR_LINE_BACKWARD:
		if (sb->elevator_glyph == OLG_VSB_ELEVATOR)
		    sb->elevator_glyph = OLG_VSB_ELEVATOR_LINE_BACKWARD;
		else
		    sb->elevator_glyph = OLG_VSB_ELEVATOR;
		break;
	      case SCROLLBAR_ABSOLUTE:
		if (sb->elevator_glyph == OLG_VSB_ELEVATOR)
		    sb->elevator_glyph = OLG_VSB_ELEVATOR_ABSOLUTE;
		else
		    sb->elevator_glyph = OLG_VSB_ELEVATOR;
		break;
	    }
	    break;
	  case SCROLLBAR_HORIZONTAL:
	    switch (motion) {
	      case SCROLLBAR_LINE_FORWARD:
		if (sb->elevator_glyph == OLG_HSB_ELEVATOR)
		    sb->elevator_glyph = OLG_HSB_ELEVATOR_LINE_FORWARD;
		else
		    sb->elevator_glyph = OLG_HSB_ELEVATOR;
		break;
	      case SCROLLBAR_LINE_BACKWARD:
		if (sb->elevator_glyph == OLG_HSB_ELEVATOR)
		    sb->elevator_glyph = OLG_HSB_ELEVATOR_LINE_BACKWARD;
		else
		    sb->elevator_glyph = OLG_HSB_ELEVATOR;
		break;
	      case SCROLLBAR_ABSOLUTE:
		if (sb->elevator_glyph == OLG_HSB_ELEVATOR)
		    sb->elevator_glyph = OLG_HSB_ELEVATOR_ABSOLUTE;
		else
		    sb->elevator_glyph = OLG_HSB_ELEVATOR;
		break;
	    }
	    break;
	}
    }
    scrollbar_elevator_paint(sb);
}


static void
scrollbar_paint_marker(sb, r, anchor_glyph)
    Xv_scrollbar_info *sb;
    Rect           *r;
    char            anchor_glyph;
{
    sb_normalize_rect(sb, r);
    sb_render_image(sb, r->r_left, r->r_top, r->r_width, r->r_height,
		    anchor_glyph);
}


static void
scrollbar_paint_cable(sb, dragging_elevator)
    register Xv_scrollbar_info *sb;
    int             dragging_elevator;	/* TRUE or FALSE */
{
    int             black_start, black_end;	/* for positioning the black
						 * cable */
    GC              renderGC;
    Rect            cable_rect;	/* rectangle describing entire cable */
    Rect            dr;		/* dest rect */
    Rect            elevr;	/* elevator rect */
    Rect            nr;		/* normalized rect */
    Scrollbar       win = SCROLLBAR_PUBLIC(sb);

    /* Get rectangle describing entire cable */
    cable_rect = *(Rect *) xv_get(win, WIN_RECT);
    sb_normalize_rect(sb, &cable_rect);	/* reverse norm. */
    cable_rect.r_left = (cable_rect.r_width - SCROLLBAR_CABLE_THICKNESS) / 2;
    cable_rect.r_width = SCROLLBAR_CABLE_THICKNESS;
    cable_rect.r_top = sb->cable_start;
    cable_rect.r_height = sb->cable_height;
    if (rect_isnull(&cable_rect))
	return;

    /* Get rectangle describing elevator */
    scrollbar_elevator_rect(sb, &elevr);

    /* Determine start and end of black proportional indicator */
    if (dragging_elevator) {
	black_start = elevr.r_top;
	black_end = rect_bottom(&elevr);
    } else {
	if (sb->object_length == 0) {
	    black_start = 0;
	    black_end = sb->cable_height - 1;
	} else {
	    black_start = sb->view_start * sb->cable_height /
		sb->object_length;
	    black_end = (sb->view_start + sb->view_length) *
		(sb->cable_height - 1) / sb->object_length;
	    if (black_end >= sb->cable_height)
		black_end = sb->cable_height - 1;
	}
	black_start += sb->cable_start;
	black_end += sb->cable_start;
	if (black_start >= elevr.r_top &&
	    black_end < (elevr.r_top + elevr.r_height)) {
	    /*
	     * Proportional indicator is completely within elevator: If the
	     * elevator is more than 3 pixels from the top, extend the
	     * indicator to 3 pixels above the elevator; else, if the
	     * indicator is at least 3 pixels from the bottom, then extend
	     * the indicator 3 pixels below the elevator.  Otherwise, we're
	     * out of luck.
	     */
	    if (elevr.r_top - sb->cable_start >= 3)
		black_start = elevr.r_top - 3;
	    else if (rect_bottom(&cable_rect) - rect_bottom(&elevr) >= 3)
		black_end = elevr.r_top + elevr.r_height + 2;
	}
	/*
	 * Insure that the indicator does not stick out from either end of
	 * the elevator less than 3 pixels, if possible.
	 */
	if (elevr.r_top - black_start > 0 &&
	    elevr.r_top - black_start < 3)
	    black_start = MAX((elevr.r_top - 3), sb->cable_start);
	if (black_end - rect_bottom(&elevr) > 0 &&
	    black_end - rect_bottom(&elevr) < 3)
	    black_end = MIN((rect_bottom(&elevr) + 3),
			    (sb->cable_start + sb->cable_height - 1));

	/*
	 * Note:  We will assume that the proportional indicator is touching
	 * the elevator (as required by OPEN LOOK).  If the calculations to
	 * this point would place the indicator totally outside the elevator
	 * and not touching, then this is corrected in the software below
	 * which makes this assumption.
	 */
    }

    /* Get gray image GC pointer and setup stipple render offsets */
    if (sb->direction == SCROLLBAR_VERTICAL) {
	renderGC = scrollGC_offset;

/* this appears unneeded! 
	sb_set_stipple_offset(win, renderGC,
			      0, (cable_rect.r_top - sb->cable_start) % 2);
*/  } else {
	renderGC = scrollGC_normal;

/* this appears unneeded! 
	sb_set_stipple_offset(win, renderGC,
			      (cable_rect.r_left - sb->cable_start) % 2, 0);
*/    }

    /* set the offset for the stipple as defined */

    /* Paint cable above elevator */
    dr = cable_rect;
    dr.r_height = MIN(black_start, elevr.r_top) - cable_rect.r_top;
    if (dr.r_height > 0) {
	/* Paint the gray above the elevator */
	nr = dr;
	sb_normalize_rect(sb, &nr);
	sb_fill_area(win, renderGC,
		     nr.r_left, nr.r_top, nr.r_width, nr.r_height);
    }
    dr.r_top += dr.r_height;
    dr.r_height = elevr.r_top - dr.r_top;
    if (dr.r_height > 0) {
	/* Paint the black above the elevator */
	sb_normalize_rect(sb, &dr);
	sb_fill_area(win, scrollGC_black,
		     dr.r_left, dr.r_top, dr.r_width, dr.r_height);
    }
    /* Paint cable below elevator */
    dr = cable_rect;
    dr.r_top = elevr.r_top + elevr.r_height;
    dr.r_height = black_end - dr.r_top + 1;
    if (dr.r_height < 0)
	dr.r_height = 0;
    if (dr.r_height > 0) {
	/* Paint the black below the elevator */
	nr = dr;
	sb_normalize_rect(sb, &nr);
	sb_fill_area(win, scrollGC_black,
		     nr.r_left, nr.r_top, nr.r_width, nr.r_height);
    }
    dr.r_top += dr.r_height;
    dr.r_height = cable_rect.r_top + cable_rect.r_height - dr.r_top;
    if (dr.r_height > 0) {
	/* Paint the gray below the elevator */
	sb_normalize_rect(sb, &dr);
	sb_fill_area(win, renderGC,
		     dr.r_left, dr.r_top, dr.r_width, dr.r_height);
    }
}

static int
scrollbar_clear_rect(sb_public, r)
    Xv_Window       sb_public;
    Rect           *r;
{
    if (r && sb_public) {
	Xv_Drawable_info    *info;
	DRAWABLE_INFO_MACRO(sb_public, info);
	XClearArea(xv_display(info), xv_xid(info), r->r_left, r->r_top,
		   r->r_width, r->r_height, False);
    }
}

Pkg_private void
scrollbar_paint_page_number(sb, paint)
    Xv_scrollbar_info *sb;
    short           paint;	/* whether to paint or to remove */
{
}


/*
 * Code to make the scrollbar logic closer to X. There are four specific GCs
 * that are setup and used for the rendering of the scrollbars. This enables
 * us to do all rendering w/o having to dynamically setup any additional
 * information for the rendering flavor. Note that this is being somewhat GC
 * happy, as it is easy to simply adjust the GC, but the tradeoff is being
 * made because the scrollbar is something that is very visible to the
 * customer as far as response. Hence we are using more GCs than needed, at
 * the benifit of not having to dynamically adjust any of them. This may
 * later be trimmed to two or one if needed. jcb -- 4/17/89
 */

Pkg_private void
sb_init_GCs(sb)
    Xv_scrollbar_info *sb;
{
    Scrollbar       sb_public = SCROLLBAR_PUBLIC(sb);
    Xv_Drawable_info *info;
    Display        *display;
    Window          window;
    Pixmap          gray_offset;
    Pixmap          gray_normal;
    XGCValues       gcvalues;
    long            gcvaluemask;
    int             foreground, background;

    if (firsttime == FALSE)
	return;

    /* get the root of X information */
    DRAWABLE_INFO_MACRO(sb_public, info);
    display = xv_display(info);
    window = xv_xid(info);
    foreground = xv_fg(info);
    background = xv_bg(info);

    /* create stipples for filling the areas */
    gray_normal = XCreateBitmapFromData(display, window,
					gray_normal_pattern,
					BITWIDTH, BITHEIGHT);

    gray_offset = XCreateBitmapFromData(display, window,
					gray_offset_pattern,
					BITWIDTH, BITHEIGHT);

    /*
     * create three GCs for rendering into the area, different stipple
     * patterns
     */
    gcvalues.function = GXcopy;
    gcvalues.plane_mask = AllPlanes;
    gcvalues.foreground = foreground;
    gcvalues.background = background;
    gcvalues.fill_style = FillOpaqueStippled;
    gcvalues.font = (Font) xv_get(sb->glyph_font, XV_XID);

    gcvaluemask = GCFunction | GCPlaneMask |
	GCForeground | GCBackground |
	GCFont | GCFillStyle |
	GCStipple;

    /* gray offset creation */
    gcvalues.stipple = gray_offset;
    scrollGC_offset = XCreateGC(display, window, gcvaluemask, &gcvalues);

    /* gray normal creation */
    gcvalues.stipple = gray_normal;
    scrollGC_normal = XCreateGC(display, window, gcvaluemask, &gcvalues);

    /* black areas are filled with this one */
    gcvalues.fill_style = FillSolid;
    scrollGC_black = XCreateGC(display, window, gcvaluemask, &gcvalues);

    /* don't call this again! */
    firsttime = FALSE;
}

/*
 * output the `glyph' to the window, create world if needed
 */

sb_render_image(sb, X, Y, W, H, c)
    Xv_scrollbar_info *sb;
    int             X, Y, W, H;
    char            c;
{
    Scrollbar       sb_public = SCROLLBAR_PUBLIC(sb);
    Xv_Drawable_info *info;
    Display        *display;
    Window          window;
    char            str[2];
    XGCValues       gcvalues, *gv;
    int             new_fg, new_bg;

    /* get to the X information */
    DRAWABLE_INFO_MACRO(sb_public, info);
    display = xv_display(info);
    window = xv_xid(info);

    /* put char into temp string and render it */
    str[0] = c;

    gv = &scrollGC_black->values;
    new_fg = xv_fg(info);
    new_bg = xv_bg(info);
    if ((new_fg != gv->foreground) || (new_bg != gv->background)) {
	gcvalues.foreground = new_fg;
	gcvalues.background = new_bg;
	XChangeGC(display, scrollGC_black, GCForeground | GCBackground, &gcvalues);
    }
    /* clear area first, then render character */
    XClearArea(display, window, X, Y, W, H, False);
    XDrawString(display, window, scrollGC_black, X, Y, str, 1);
}


/*
 * fill the area defined using the default GC setup of the passed GC.
 */
sb_fill_area(sb_public, gc, X, Y, W, H)
    Scrollbar       sb_public;
    GC              gc;
    int             X, Y, W, H;
{
    Xv_Drawable_info *info;
    Display        *display;
    Window          window;
    XGCValues       gcvalues, *gv;
    int             new_fg, new_bg;

    /* get to the X information */
    DRAWABLE_INFO_MACRO(sb_public, info);
    display = xv_display(info);
    window = xv_xid(info);

    gv = &gc->values;
    new_fg = xv_fg(info);
    new_bg = xv_bg(info);
    if ((new_fg != gv->foreground) || (new_bg != gv->background)) {
	gcvalues.foreground = new_fg;
	gcvalues.background = new_bg;
	XChangeGC(display, gc, GCForeground | GCBackground, &gcvalues);
    }
    /* it's this easy! */
    XFillRectangle(display, window, gc, X, Y, W, H);
}

/*
 * sets up stipple for aligned filling w/ pattern
 */
sb_set_stipple_offset(sb_public, gc, offsetX, offsetY)
    Scrollbar       sb_public;
    GC              gc;
    int             offsetX, offsetY;
{
    Xv_Drawable_info *info;
    Display        *display;

    /* get to the X information */
    DRAWABLE_INFO_MACRO(sb_public, info);
    display = xv_display(info);

    /* Set the offset for the tile/stipple pattern */
    XSetTSOrigin(display, gc, offsetX, offsetY); 
}
