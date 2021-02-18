#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb_pos.c 1.31 89/11/14";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb_pos.c
 * 
 * Description:
 * 
 * positions elements in the scrollbar
 * 
 */


/*
 * Include files:
 */
#include <xview_private/sb_impl.h>

/*
 * Declaration of Functions Defined in This File (in order):
 */

Xv_private int  scrollbar_width_for_scale();

Pkg_private void scrollbar_init_positions();
Pkg_private void scrollbar_position_elevator();
Pkg_private void scrollbar_absolute_position_elevator();
Pkg_private void scrollbar_to_start_marker_rect();
Pkg_private void scrollbar_to_end_marker_rect();
Pkg_private void scrollbar_to_start_marker_innerrect();
Pkg_private void scrollbar_to_end_marker_innerrect();
Pkg_private void sb_normalize_rect();
Pkg_private void scrollbar_elevator_rect();
Pkg_private void scrollbar_available_cable();
Pkg_private void sb_line_backward_region();
Pkg_private void sb_absolute_region();
Pkg_private void sb_reduced_elevator();
Pkg_private void sb_fline_place_rect();
Pkg_private void sb_rescale();
Pkg_private void sb_line_backward_region();
Pkg_private void sb_line_forward_region();
Pkg_private void sb_absolute_region();
Pkg_private int sb_margin();
Pkg_private int sb_marker_height();
Pkg_private int sb_elevator_height();

static void     sb_forward_region();
static void     sb_full_elevator();

/******************************************************************/

Pkg_private void
scrollbar_init_positions(sb)
    Xv_scrollbar_info *sb;
{
    Rect            r;

    r = *(Rect *) xv_get(SCROLLBAR_PUBLIC(sb), WIN_RECT);

    if (sb->object_length == SCROLLBAR_DEFAULT_LENGTH) {
	sb->object_length =
	    (sb->direction == SCROLLBAR_VERTICAL) ? r.r_height / sb->pixels_per_unit
	    : r.r_width / sb->pixels_per_unit;
    }
    if (sb->view_length == SCROLLBAR_DEFAULT_LENGTH) {
	sb->view_length =
	    (sb->direction == SCROLLBAR_VERTICAL) ? r.r_height / sb->pixels_per_unit
	    : r.r_width / sb->pixels_per_unit;
    }

    sb->scale = (int) xv_get(xv_get(SCROLLBAR_PUBLIC(sb), XV_FONT), FONT_SCALE);

    sb->reduced = FALSE;
    sb->cable_start = sb_marker_height(sb) + SCROLLBAR_CABLE_GAP;
    sb->cable_height = (sb->direction == SCROLLBAR_VERTICAL) ? r.r_height : r.r_width;
    sb->cable_height -= sb->cable_start + sb_marker_height(sb) + SCROLLBAR_CABLE_GAP + 1;

    sb->elevator_top = sb->cable_start;

    sb_full_elevator(sb);	/* paints the elevator into the pixrect */

    /* set default sizes and positions */
    xv_set(SCROLLBAR_PUBLIC(sb),
	   (sb->direction == SCROLLBAR_VERTICAL) ? WIN_WIDTH : WIN_HEIGHT,
	   scrollbar_width_for_scale(sb->scale), 0);
}

Pkg_private void
scrollbar_position_elevator(sb, paint, use_true_elevator_position)
    Xv_scrollbar_info *sb;
    int             paint;	/* TRUE or FALSE */
    int             use_true_elevator_position;
{
    int             available_cable;
    int             new_top;
    int             old_reduced;

    old_reduced = sb->reduced;
    sb->reduced = FALSE;
    scrollbar_available_cable(sb, &available_cable);
    sb->reduced = old_reduced;

    if (sb->reduced && available_cable >= 4) {
	sb_full_elevator(sb);
    } else if (!sb->reduced && available_cable < 4) {
	sb_reduced_elevator(sb);
    }
    scrollbar_available_cable(sb, &available_cable);
    if (available_cable < 0) {
	available_cable = 0;
    }
    /* Bounds checking */
    if (sb->view_start > Max_offset(sb)) {
	sb->view_start = Max_offset(sb);
    }
    if (use_true_elevator_position) {
	if (sb->view_start == 0) {
	    /* At beginning */
	    new_top = sb->cable_start;
	} else {
	    new_top = sb->view_start * available_cable / Max_offset(sb);
	    if (new_top < 3) {
		/* Not at beginning, leave anchor a little */
		new_top = (available_cable < 3) ? available_cable : 3;
	    } else if (sb->view_start < Max_offset(sb)
		  && new_top > available_cable - 3 && available_cable > 3) {
		/* Not at end, leave anchor a little */
		new_top = available_cable - 3;
	    }
	    new_top += sb->cable_start;
	}
    } else
	/*
	 * User has positioned (e.g., dragged) the elevator to a specific
	 * point.  We don't want to jump the elevator to the "true" position
	 * (to indicate the view start) because this is poor user interface.
	 */
	new_top = sb->elevator_top;

    if (paint)
	/*
	 * Set clipping area to size of elevator plus motion and then
	 * repaint.
	 */
	scrollbar_paint_elevator_move(sb, new_top, FALSE);
    else
	sb->elevator_top = new_top;
}

Pkg_private void
scrollbar_absolute_position_elevator(sb, pos)
    Xv_scrollbar_info *sb;
    int             pos;
{
    int             available_cable;

    scrollbar_available_cable(sb, &available_cable);

    if (pos < 0 || available_cable <= 0) {
	pos = 0;
    } else if (pos > available_cable) {
	pos = available_cable;
    }
    /* add back in cable_start offset */
    pos += sb->cable_start;

    /*
     * pos is the new top of the elevator.  We are dragging the elevator at
     * this point, so don't paint a black proportional indicator.
     */
    scrollbar_paint_elevator_move(sb, pos, TRUE);
}

Pkg_private void
scrollbar_to_start_marker_innerrect(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{

    scrollbar_to_start_marker_rect(sb, r);
    rect_marginadjust(r, -1);
}

Pkg_private void
scrollbar_to_start_marker_rect(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{
    Rect            win_rect;

    win_rect = *(Rect *) xv_get(SCROLLBAR_PUBLIC(sb), WIN_RECT);
    sb_normalize_rect(sb, &win_rect);

    r->r_left = sb_margin(sb);
    r->r_top = 0;
    r->r_width = win_rect.r_width - (2 * sb_margin(sb));
    r->r_height = sb_marker_height(sb);
}

Pkg_private void
scrollbar_to_end_marker_innerrect(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{

    scrollbar_to_end_marker_rect(sb, r);
    rect_marginadjust(r, -1);
}

Pkg_private void
scrollbar_to_end_marker_rect(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{
    Rect            win_rect;

    win_rect = *(Rect *) xv_get(SCROLLBAR_PUBLIC(sb), WIN_RECT);
    sb_normalize_rect(sb, &win_rect);

    r->r_left = sb_margin(sb);
    r->r_top = win_rect.r_height - sb_marker_height(sb);
    r->r_width = win_rect.r_width - (2 * sb_margin(sb));
    r->r_height = sb_marker_height(sb);
}

Pkg_private void
sb_normalize_rect(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{
    int             temp;

    if (sb->direction != SCROLLBAR_VERTICAL) {
	temp = r->r_top;
	r->r_top = r->r_left;
	r->r_left = temp;
	temp = r->r_width;
	r->r_width = r->r_height;
	r->r_height = temp;
    }
}

Pkg_private void
scrollbar_elevator_rect(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{

    r->r_top = sb->elevator_top;
    r->r_left = sb_margin(sb);
    r->r_width =
	scrollbar_width_for_scale(sb->scale) - (2 * sb_margin(sb));
    r->r_height = sb_elevator_height(sb->scale, sb->reduced);
}

Pkg_private void
scrollbar_available_cable(sb, available)
    Xv_scrollbar_info *sb;
    int            *available;
{
    Rect            br;

    scrollbar_elevator_rect(sb, &br);
    *available = sb->cable_height - br.r_height;
}


Pkg_private void
sb_reduced_elevator(sb)
    Xv_scrollbar_info *sb;
{
    sb->reduced = TRUE;
    switch (sb->elevator_glyph) {
      case OLG_VSB_ELEVATOR:
	sb->elevator_glyph = OLG_VSB_REDUCED_ELEVATOR;
	break;
      case OLG_HSB_ELEVATOR:
	sb->elevator_glyph = OLG_HSB_REDUCED_ELEVATOR;
	break;
      default:
	/*
	 * Note: It's impossible to resize the window while the SELECT button
	 * is down over the elevator, so the only glyphs that could be in use
	 * is the vertical or horizontal "normal" state.  If they're already
	 * in the reduced state, then fine.
	 */
	break;
    }
}

static void
sb_full_elevator(sb)
    Xv_scrollbar_info *sb;
{
    sb->reduced = FALSE;
    switch (sb->elevator_glyph) {
      case OLG_VSB_REDUCED_ELEVATOR:
	sb->elevator_glyph = OLG_VSB_ELEVATOR;
	break;
      case OLG_HSB_REDUCED_ELEVATOR:
	sb->elevator_glyph = OLG_HSB_ELEVATOR;
	break;
      default:
	/*
	 * Note: It's impossible to resize the window while the SELECT button
	 * is down over the elevator, so the only glyphs that could be in use
	 * is the vertical or horizontal "normal" state.  If they're already
	 * in the full state, then fine.
	 */
	break;
    }
}


Pkg_private int
sb_margin(sb)
    Xv_scrollbar_info *sb;
{
    switch (sb->scale) {
      case WIN_SCALE_SMALL:
	return SB_SMALL_MARGIN;
	break;
      case WIN_SCALE_MEDIUM:
	return SB_MEDIUM_MARGIN;
	break;
      case WIN_SCALE_LARGE:
	return SB_LARGE_MARGIN;
	break;
      case WIN_SCALE_EXTRALARGE:
	return SB_XLARGE_MARGIN;
	break;
    }
}

Pkg_private int
sb_marker_height(sb)
    Xv_scrollbar_info *sb;
{
    switch (sb->scale) {
      case WIN_SCALE_SMALL:
	return SB_SMALL_MARKER_HEIGHT;
	break;
      case WIN_SCALE_MEDIUM:
	return SB_MEDIUM_MARKER_HEIGHT;
	break;
      case WIN_SCALE_LARGE:
	return SB_LARGE_MARKER_HEIGHT;
	break;
      case WIN_SCALE_EXTRALARGE:
	return SB_XLARGE_MARKER_HEIGHT;
	break;
    }
}

Pkg_private void
sb_fline_place_rect(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{
    switch (sb->scale) {
      case WIN_SCALE_SMALL:
	r->r_left = SB_SMALL_LINE_IMAGE_LEFT_MARGIN;
	r->r_top = sb->reduced ? SB_REDUCED_SMALL_LINE_IMAGE_TOP_MARGIN
	    : SB_SMALL_LINE_IMAGE_TOP_MARGIN;
	r->r_width = SB_SMALL_FLINE_IMAGE_WIDTH;
	r->r_height = SB_SMALL_FLINE_IMAGE_HEIGHT;
	break;
      case WIN_SCALE_MEDIUM:
	r->r_left = SB_MEDIUM_LINE_IMAGE_LEFT_MARGIN;
	r->r_top = sb->reduced ? SB_REDUCED_MEDIUM_LINE_IMAGE_TOP_MARGIN
	    : SB_MEDIUM_LINE_IMAGE_TOP_MARGIN;
	r->r_width = SB_MEDIUM_FLINE_IMAGE_WIDTH;
	r->r_height = SB_MEDIUM_FLINE_IMAGE_HEIGHT;
	break;
      case WIN_SCALE_LARGE:
	r->r_left = SB_LARGE_LINE_IMAGE_LEFT_MARGIN;
	r->r_top = sb->reduced ? SB_REDUCED_LARGE_LINE_IMAGE_TOP_MARGIN
	    : SB_LARGE_LINE_IMAGE_TOP_MARGIN;
	r->r_width = SB_LARGE_FLINE_IMAGE_WIDTH;
	r->r_height = SB_LARGE_FLINE_IMAGE_HEIGHT;
	break;
      case WIN_SCALE_EXTRALARGE:
	r->r_left = SB_XLARGE_LINE_IMAGE_LEFT_MARGIN;
	r->r_top = sb->reduced ? SB_REDUCED_XLARGE_LINE_IMAGE_TOP_MARGIN
	    : SB_XLARGE_LINE_IMAGE_TOP_MARGIN;
	r->r_width = SB_XLARGE_FLINE_IMAGE_WIDTH;
	r->r_height = SB_XLARGE_FLINE_IMAGE_HEIGHT;
	break;
    }
}

Pkg_private void
sb_rescale(sb, scale)
    Xv_scrollbar_info *sb;
    Frame_rescale_state scale;
{
    if (sb->scale != scale) {
	sb->scale = scale;
	if (sb->reduced) {
	    sb_reduced_elevator(sb);
	} else {
	    sb_full_elevator(sb);
	}
    }
}

Pkg_private void
sb_line_backward_region(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{
    r->r_left = SB_ELEVATOR_INNER_LEFT;
    r->r_top = SB_ELEVATOR_INNER_TOP;
    r->r_width = sb_elevator_inner_width(sb->scale);
    switch (sb->scale) {
      case WIN_SCALE_SMALL:
	r->r_height = SB_SMALL_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_MEDIUM:
	r->r_height = SB_MEDIUM_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_LARGE:
	r->r_height = SB_LARGE_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_EXTRALARGE:
	r->r_height = SB_XLARGE_ELEVATOR_BOX_HEIGHT;
	break;
    }
}

Pkg_private void
sb_line_forward_region(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{
    Rect            ar, br;

    sb_absolute_region(sb, &ar);
    sb_line_backward_region(sb, &br);
    switch (sb->scale) {
      case WIN_SCALE_SMALL:
	r->r_height = SB_SMALL_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_MEDIUM:
	r->r_height = SB_MEDIUM_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_LARGE:
	r->r_height = SB_LARGE_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_EXTRALARGE:
	r->r_height = SB_XLARGE_ELEVATOR_BOX_HEIGHT;
	break;
    }
    r->r_left = SB_ELEVATOR_INNER_LEFT;
    r->r_top = sb->reduced ? (br.r_top + br.r_height)
	: (ar.r_top + ar.r_height);
    r->r_width = sb_elevator_inner_width(sb->scale);
}

Pkg_private void
sb_absolute_region(sb, r)
    Xv_scrollbar_info *sb;
    Rect           *r;
{
    Rect            br;

    sb_line_backward_region(sb, &br);

    r->r_left = SB_ELEVATOR_INNER_LEFT;
    r->r_width = sb_elevator_inner_width(sb->scale);

    switch (sb->scale) {
      case WIN_SCALE_SMALL:
	r->r_height = sb->reduced ? 0 : SB_SMALL_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_MEDIUM:
	r->r_height = sb->reduced ? 0 : SB_MEDIUM_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_LARGE:
	r->r_height = sb->reduced ? 0 : SB_LARGE_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_EXTRALARGE:
	r->r_height = sb->reduced ? 0 : SB_XLARGE_ELEVATOR_BOX_HEIGHT;
	break;
    }
    r->r_top = sb->reduced ? br.r_top : (br.r_top + br.r_height);
}

Xv_private int
scrollbar_width_for_scale(scale)
    Frame_rescale_state scale;
{
    switch (scale) {
      case WIN_SCALE_SMALL:
	return SCROLLBAR_SMALL_THICKNESS;
      case WIN_SCALE_MEDIUM:
	return SCROLLBAR_MEDIUM_THICKNESS;
      case WIN_SCALE_LARGE:
	return SCROLLBAR_LARGE_THICKNESS;
      case WIN_SCALE_EXTRALARGE:
	return SCROLLBAR_XLARGE_THICKNESS;
      default:
	return SCROLLBAR_MEDIUM_THICKNESS;
    }
}

Pkg_private int
sb_elevator_height(scale, reduced)
    Frame_rescale_state scale;
    int             reduced;
{
    int             h;

    switch (scale) {
      case WIN_SCALE_SMALL:
	h = SB_SMALL_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_MEDIUM:
	h = SB_MEDIUM_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_LARGE:
	h = SB_LARGE_ELEVATOR_BOX_HEIGHT;
	break;
      case WIN_SCALE_EXTRALARGE:
	h = SB_XLARGE_ELEVATOR_BOX_HEIGHT;
	break;
    }
    if (reduced) {
	h = h * 2 + 2;
    } else {
	h = h * 3 + 2;
    }
    return (h);
}

static int
sb_elevator_inner_width(scale)
    Frame_rescale_state scale;
{
    switch (scale) {
      case WIN_SCALE_SMALL:
	return (SCROLLBAR_SMALL_THICKNESS - 2 * SB_SMALL_MARGIN - 3);
      case WIN_SCALE_MEDIUM:
	return (SCROLLBAR_MEDIUM_THICKNESS - 2 * SB_MEDIUM_MARGIN - 3);
      case WIN_SCALE_LARGE:
	return (SCROLLBAR_LARGE_THICKNESS - 2 * SB_LARGE_MARGIN - 3);
      case WIN_SCALE_EXTRALARGE:
	return (SCROLLBAR_XLARGE_THICKNESS - 2 * SB_XLARGE_MARGIN - 3);
    }
}
